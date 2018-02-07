/***********************************************************************
 *  The server will use 2 queues to store the orders, till each order  *   
 * reaches the client. The default size of each queue is declared in   *   
 * the BAKE_QUEUE_DEF and DELIVER_QUEUE_DEF defines. Those values can  *   
 * be run-time specified by the second argument when calling the       *
 * server, provided it is a positive numeric value, not exceeding the  *
 * value QUEUE_SIZE_MAX.                                               *
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>    

#include "pizzeriaInfo.h"
#include "myQueue.h"

#define N_PSISTES      10  //Number of the bakers
#define N_DIANOMEIS    10  //Number of the delivery-boys
//Default sizes of the queues that orders are stored till they get baked - delivered
#define BAKE_QUEUE_DEF     100
#define DELIVER_QUEUE_DEF  100
#define QUEUE_SIZE_MAX     500 //Max size of each queue

//Signal handling functions
void catch_int(int sig_num);
void catch_alarm(int sig_num);

void* baker(void* p);         //Code executed by each baker child
void* delivery_boy(void* p);  //Code executed by each delivery-boy child
//Code executed by each thread responsible for an order, which includes handling 
//the communication with the client, take the order and add it to the queue
void* processOrder(void* clientSd); 


void orderArrived(int clientSd); 
void orderLate(int clientSd);

enum { STATUS_UNBAKED, STATUS_BAKING, STATUS_READY}; //Possible status for each pizza
// Times in msecs
const int bakingTimes[] = { 1, 2, 3};
const int distanceTimes[] = { 3, 5};
const int tVeryLong = 20;
       
pthread_mutex_t bake_mutex,del_mutex;
pthread_mutex_t bake_count,del_count;
pthread_cond_t bake_cond,del_cond;
int bakersNeeded,delsNeeded;

int sd, conSd; //Socket descriptor of the socket between each client
int socketQ;  //Socket's queue length   

QueueInfo *bakeQ, *deliverQ; //Queues to store orders that need baking or delivering
int bQSize, dQSize;

int main(int argc, char* argv[]){
	int i,r;
	struct sockaddr_un serverAddress;
	int serverAddrLength;  
	struct sockaddr_un clientAddress;
	socklen_t clientAddrLength; 
//------------------------------------------------------------
// SIGNAL HANDLERS    
//------------------------------------------------------------	
	//Set the signal handlers for each signal using the sigaction method and struct
	struct sigaction sigint, sigalarm;	
	
	sigint.sa_handler = catch_int;
	sigalarm.sa_handler = catch_alarm;	
	sigint.sa_flags = 0;
	sigalarm.sa_flags = 0;	
	i = sigaction(SIGINT, &sigint, NULL);
	i += sigaction(SIGALRM, &sigalarm, NULL);
	if(i<0){
		printf("Error initializing a signal handler \n");
		exit(1);
	}
//------------------------------------------------------------
// INITIALIZING THE TIMER    
//------------------------------------------------------------
	//Create a timer to send a SIGALRM event every tVeryLong msecs
	timer_t timer;
	struct sigevent event;
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;
	event.sigev_value.sival_ptr = &timer ;
	
	struct itimerspec t;
	t.it_value.tv_sec = tVeryLong;
	t.it_value.tv_nsec = 0;
	t.it_interval.tv_sec = t.it_value.tv_sec;
	t.it_interval.tv_nsec = t.it_value.tv_nsec;
	
	if( timer_create(CLOCK_REALTIME, &event ,&timer) == -1){
		printf("Error creating the timer \n");
		exit(1);
	}
//------------------------------------------------------------
// INITIALIZING THE 2 QUEUES
//------------------------------------------------------------
	bQSize = dQSize = 0;
	//Check whether an argument was passed to main, to specify the queues' sizes
	//If not set them to their default values
	if(argc>1){
		bQSize = dQSize = atoi(argv[1]);
	}
	if(bQSize <= 0 ||bQSize > QUEUE_SIZE_MAX ){		
		bQSize = BAKE_QUEUE_DEF; 
		dQSize = DELIVER_QUEUE_DEF;
	}
	bQSize++;
	dQSize++;
	socketQ = bQSize;
	//Allocate size for both queues
	bakeQ = (QueueInfo*)malloc(sizeof(QueueInfo));
	deliverQ = (QueueInfo*)malloc(sizeof(QueueInfo));
	if(bakeQ==NULL || deliverQ==NULL){
		printf("Error allocating memory \n");
		exit(1);	
	}	
	OrderNode* ptr;
	ptr = (OrderNode*)malloc(bQSize*sizeof(OrderNode));
	if(ptr==NULL){
		printf("Error allocating memory \n");
		exit(1);	
	}
	queueInit(bakeQ, bQSize, ptr);

	ptr = (OrderNode*)malloc(dQSize*sizeof(OrderNode));
	if(ptr==NULL){
		printf("Error allocating memory \n");
		exit(1);	
	}
	queueInit(deliverQ, dQSize, ptr);		
//------------------------------------------------------------
// INITIALIZING MUTEXES AND CONDITION VARIABLES 
//------------------------------------------------------------
	//2 mutex semaphores for accessing each of the 2 queues
	pthread_mutex_init(&bake_mutex, NULL);   
	pthread_mutex_init(&del_mutex, NULL);
	//A counting semaphore to indicate how many pizzas need baking,
	//which will be implemented by a condition variable and an accosiated
	//mutex depending on the value of an int
	pthread_mutex_init(&bake_count, NULL); 
	pthread_cond_init(&bake_cond, NULL);
	bakersNeeded = 0;	
	//A counting semaphore to indicate how many orders need delivering
	//deliver_sem = sem_open(DELIVER_SEM, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
	pthread_mutex_init(&del_count, NULL); 
	pthread_cond_init(&del_cond, NULL);
	delsNeeded = 0;
	//Initial values above are 0 so that all baker and deliver threads
	//will be created and fall asleep till an order comes
//------------------------------------------------------------
// INITIALIZING SOCKETS   
//------------------------------------------------------------
	if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ){
		printf("Error creating socket \n");     
		exit(1);
	}
	serverAddress.sun_family = AF_LOCAL;
	strcpy(serverAddress.sun_path, SOCKET_PATH);
	unlink(SOCKET_PATH);
 	serverAddrLength = strlen(serverAddress.sun_path) + sizeof(serverAddress.sun_family);
	
	if( bind(sd,(struct sockaddr *)&serverAddress, serverAddrLength) == -1 ){
		printf("Error binding socket \n"); 
		exit(1);
	}
	if( listen(sd, socketQ) == -1){
		printf("Error listening from socket \n");  
		exit(1);
	}
//------------------------------------------------------------
// CREATING THREADS FOR BAKING AND DELIVERING JOBS 
//------------------------------------------------------------	
	pthread_t bakeThreads[N_PSISTES];
	pthread_t delThreads[N_DIANOMEIS];
	//Spawn the baker and delivery_boy threads and start their respective code	
	for(i=0; i<N_PSISTES; i++){ 
		r = pthread_create(&bakeThreads[i], NULL, baker, NULL);
		if(r!=0){
			printf("Error creating a thread\n");
			exit(1);
		}
	}	
	for(i=0; i<N_DIANOMEIS; i++){
		r = pthread_create(&delThreads[i], NULL, delivery_boy, NULL);
		if(r!=0){
			printf("Error creating a thread\n");
			exit(1);
		}
	}
//------------------------------------------------------------
// SERVER-RESTAURANT READY TO OPERATE
//------------------------------------------------------------
	printf("Pizzeria %s is open for bussiness!\n", PIZZERIA);
	printf("Press Ctrl-C to close the restaurant for today.\n\n");	
	if( timer_settime(timer, 0, &t, NULL) == -1){  //Start the timer
		printf("Error starting the timer \n");
		exit(1);
	}	
	
	pthread_t orderThread;
	while(1){		
		clientAddrLength = sizeof(clientAddress);
		//Wait for a client to connect
		conSd = accept(sd, (struct sockaddr*)&clientAddress, &clientAddrLength );
		if(conSd == -1){
			if(errno == EINTR){
				continue;
			}else{
				printf("Error in accepting from socket \n");
				exit(1);
			}
		}
		//nanosleep(&ti,NULL);
		//Create a thread to handle the communication with a client
		r = pthread_create(&orderThread, NULL, processOrder, (void*)conSd);
		if(r!=0){
			printf("\nError creating a thread. Code %d. \n",errno);
			exit(1);
		}		
	}		
	exit(1);
}

void* baker(void* p){
	OrderNode *curNode=NULL;
	int curPizza;
	while(1){
		pthread_mutex_lock(&bake_count);	
		if(bakersNeeded == 0){
			pthread_cond_wait(&bake_cond, &bake_count);
		}
		pthread_mutex_unlock(&bake_count);
			
		pthread_mutex_lock(&bake_mutex);	
		bakersNeeded--;	           //One less pizza to bake		
		curNode = bakeQ->nextOrder;  //See which order to deal with
		curPizza = curNode->theOrder.nextPizza; //See which pizza to take
		//Let the other bakers know that the pizza which needs baking in the
		//current order is the next one. The current is being baked!
		curNode->theOrder.nextPizza++; 
		curNode->theOrder.status[curPizza] = STATUS_BAKING; 	
		//If that pizza was the last of the current order, let the other
		//bakers know to deal with the next order 		 
		if(curNode->theOrder.nextPizza == curNode->theOrder.N_pizza){
			bakeQ->nextOrder = bakeQ->nextOrder->next;
		}		
		pthread_mutex_unlock(&bake_mutex);				
		
		sleep(bakingTimes[ curNode->theOrder.types[curPizza]]); //Bake it!	
		
		pthread_mutex_lock(&bake_mutex);			
		curNode->theOrder.status[curPizza] = STATUS_READY;
		curNode->theOrder.readyPizza++;
		//If the order your are dealing with is in the head of the queue,
		//and if all the pizza in this order are ready, pop it from the
		//baking-queue, push it to the delivering-queue indicate that
		//another delivery-boy process needs to be awake
		//Do the same for every next order that is ready (till one that
		// is not ready is found)		
		while(curNode == bakeQ->head && curNode != NULL){
			if(curNode->theOrder.readyPizza == curNode->theOrder.N_pizza){
				pushOrder(deliverQ, popOrder(bakeQ));
				delsNeeded++;
				pthread_cond_signal(&del_cond);
				curNode = curNode->next;
			}else{
				break;
			}
		}			
		pthread_mutex_unlock(&bake_mutex);
	}
	return NULL;
}
void* delivery_boy(void* p){	
	PizzaOrder deliverOrder;
	int timeToDeliver;
	while(1){
		pthread_mutex_lock(&del_count);	
		if(delsNeeded == 0){
			pthread_cond_wait(&del_cond, &del_count);
		}
		pthread_mutex_unlock(&del_count);
			
		pthread_mutex_lock(&del_mutex); 
		delsNeeded--;   //One order less to deliver					
		deliverOrder = popOrder(deliverQ); //Take the first order
		//Calculare the time to deliver it depending on the distance
		timeToDeliver = distanceTimes[deliverOrder.distance]; 
		pthread_mutex_unlock(&del_mutex); 	
		
		
		sleep(timeToDeliver); //Deliver it!		
		orderArrived(deliverOrder.clientSd);		
	}	
	return NULL;
}
void* processOrder(void* clientSd){
	int cliSd = (int)clientSd;
	PizzaOrder order;
	int i,t;
	t = read(cliSd, &(order), sizeof(PizzaOrder));
	if(t<=0){
		printf("Problem in reading from a client \n");
		printf("Connection closing... \n");
		close(cliSd); 		
		pthread_exit(NULL);
	}
	if(bakeQ->currentSize == bQSize){ // Check for queue overflow
		char msg[]="Server is currently busy. Try again later";
		write(cliSd, MSG_OK, sizeof(MSG_OK));
		write(cliSd, msg, sizeof(msg));
		close(cliSd);
		pthread_exit(NULL);
	}	
	//printOrder(order);
	for(i=0; i < order.N_pizza; i++){ 
		//Set each pizza of the new order to be unbaked
		order.status[i] = STATUS_UNBAKED; 
	}
	order.readyPizza = 0;
	order.nextPizza = 0;
	order.delays = -1;
	order.clientSd = cliSd;
	
	pthread_mutex_lock(&bake_mutex);
	pushOrder(bakeQ, order); //Push the order to the queue
			
	if(bakeQ->nextOrder == NULL){
		bakeQ->nextOrder = bakeQ->tail;
	}
	bakersNeeded += order.N_pizza;
	pthread_mutex_unlock(&bake_mutex);	
	for(i=0; i < order.N_pizza; i++){
		pthread_cond_signal(&bake_cond);	
	}
	pthread_exit(NULL);
}
//Used to shut down the server and its children while deallocating 
//the appropriate system resources
void catch_int(int sig_num){
	close(sd);	
	free(bakeQ->initialPosition);
	free(deliverQ->initialPosition);
	free(bakeQ);
	free(deliverQ);
	
	pthread_mutex_destroy(&bake_mutex);
	pthread_mutex_destroy(&del_mutex);
	pthread_mutex_destroy(&bake_count);
	pthread_mutex_destroy(&del_count);
	pthread_cond_destroy(&bake_cond);
	pthread_cond_destroy(&del_cond);
	printf("\n%s has closed for today! \n\n", PIZZERIA);		
	exit(0);
}
void orderArrived(int clientSd){	
	char msg[MESSAGE_SIZE];
	strcpy(msg, "Your food is here! Thanks for prefering us.");
	write(clientSd, MSG_OK, sizeof(MSG_OK));
	write(clientSd, msg, sizeof(msg));
	close(clientSd);
}
void orderLate(int clientSd){	
	char msg[MESSAGE_SIZE];
	strcpy(msg,"Your delivery will be late. We apologize and we offer a coca cola for free");
	write(clientSd, MSG_OTHER, sizeof(MSG_OTHER));
	write(clientSd, msg, sizeof(msg));
}

//Used to traverse the queues periodically and check for delayed orders.
void catch_alarm(int sig_num){
	OrderNode* temp;
	int d;
	pthread_mutex_lock(&bake_mutex);
	pthread_mutex_lock(&del_mutex); 
	temp = bakeQ->head;
	while(temp!=NULL){
		d = ++(temp->theOrder.delays);	
		if( d == 1 ){
			orderLate(temp->theOrder.clientSd);
		}
		temp = temp->next;
	}
	temp = deliverQ->head;
	while(temp!=NULL){
		d = ++(temp->theOrder.delays);		
		if( (d == 1) ){
			orderLate(temp->theOrder.clientSd);
		}
		temp = temp->next;
	}
	pthread_mutex_unlock(&del_mutex); 
	pthread_mutex_unlock(&bake_mutex);	
}


