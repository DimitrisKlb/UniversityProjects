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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "pizzeriaInfo.h"
#include "myQueue.h"

#define N_PSISTES      10  //Number of the bakers
#define N_DIANOMEIS    10  //Number of the delivery-boys
//Default sizes of the queues that orders are stored till they get baked - delivered
#define BAKE_QUEUE_DEF     100 
#define DELIVER_QUEUE_DEF  100 
#define QUEUE_SIZE_MAX     500 //Max size of each queue

#define SOCKET_QUEUE   30  //Socket's queue length    
#define BAKE_SEM       "semaphore_bake"
#define DELIVER_SEM    "semaphore_delivery"
#define BAKE_MUTEX     "mutex_bake"
#define DELIVER_MUTEX  "mutex_delivery"

#define A_MILLION 1000000

//Signal handling functions
void catch_int(int sig_num);
void catch_deadChild(int sig_num);
void catch_orderArrived(int sig_num);
void catch_orderLate(int sig_num);
void catch_alarm(int sig_num);

void baker(void);         //Code executed by each baker child
void delivery_boy(void);  //Code executed by each delivery-boy child

enum { STATUS_UNBAKED, STATUS_BAKING, STATUS_READY}; //Possible status for each pizza
// Times in msecs
const int bakingTimes[] = { 100, 120, 150};
const int distanceTimes[] = { 50, 100};
const int tVeryLong = 500;

pid_t parentPid;   //Pid of the parent server
int shm_id;
char *shm_data;       
sem_t *bake_sem, *deliver_sem; 
sem_t *bake_mutex, *deliver_mutex;
int sd, conSd; //Socket descriptor of the socket between each client

QueueInfo *bakeQ, *deliverQ; //Queues to store orders that need baking or delivering

int main(int argc, char* argv[]){
	parentPid = getpid();
	int i;
	PizzaOrder order;

	struct sockaddr_un serverAddress;
	int serverAddrLength;  
	struct sockaddr_un clientAddress;
	socklen_t clientAddrLength; 
	int pid = 0;	
	key_t shm_key = 11880;    // tixaio?
	size_t shm_size;
//------------------------------------------------------------
// SIGNAL HANDLERS
//------------------------------------------------------------	
	//Set the signal handlers for each signal using the sigaction method and struct
	struct sigaction sigint, sigchild, sig1, sig2, sigalarm;
	sigset_t set1, set2;
	
	//Mask SIGUSR1 and SIGUSR2 signals so that each cannot interrupt the other
	sigemptyset(&set1); 	
	sigaddset(&set1, SIGUSR1);
	sigaddset(&set1, SIGUSR2);
	//Mask SIGCHLD and SIGALRM signals so that each cannot interrupt the other
	sigemptyset(&set2);	
	sigaddset(&set2, SIGCHLD);
	sigaddset(&set2, SIGALRM);	
	
	sig1.sa_mask = set1;
	sig2.sa_mask = set1;
	sigchild.sa_mask = set2;
	sigalarm.sa_mask = set2;
	
	sigint.sa_handler = catch_int;
	sigchild.sa_handler = catch_deadChild;
	sig1.sa_handler = catch_orderArrived;
	sig2.sa_handler = catch_orderLate;
	sigalarm.sa_handler = catch_alarm;
	
	sigint.sa_flags = 0;
	sigchild.sa_flags = 0;
	sig1.sa_flags = 0;
	sig2.sa_flags = 0;
	sigalarm.sa_flags = 0;
	
	i = sigaction(SIGINT, &sigint, NULL);
	i += sigaction(SIGCHLD, &sigchild , NULL);
	i += sigaction(SIGUSR1, &sig1, NULL);
	i += sigaction(SIGUSR2, &sig2, NULL);
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
	t.it_value.tv_sec = 0;
	t.it_value.tv_nsec = tVeryLong * A_MILLION;
	t.it_interval.tv_sec = t.it_value.tv_sec;
	t.it_interval.tv_nsec = t.it_value.tv_nsec;
	
	if( timer_create(CLOCK_REALTIME, &event ,&timer) == -1){
		printf("Error creating the timer \n");
		exit(1);
	}
//------------------------------------------------------------
// INITIALIZING SHARED MEMORY AND ITS CONTENTS
//------------------------------------------------------------
	int bQSize = 0;
	int dQSize = 0;
	//Check whether an argument was passed to main, to specify the queues' sizes
	//If not set them to their default values
	if(argc>1){
		bQSize = dQSize = atoi(argv[1]);
	}
	if(bQSize <= 0 ||bQSize > QUEUE_SIZE_MAX ){		
		bQSize = BAKE_QUEUE_DEF; 
		dQSize = DELIVER_QUEUE_DEF;
	}
	
	//Shared memory will store the 2 queues deliverQ and bakeQ
	shm_size = 2 * sizeof(QueueInfo); 
	shm_size += (bQSize + dQSize) * sizeof(OrderNode);
	if( (shm_id = shmget(shm_key, shm_size, 0600 | IPC_CREAT )) == -1){
		printf("Error creating shared memory \n");
		exit(1);
	}
	if( (shm_data = shmat(shm_id, NULL, 0)) == (char*)-1){
		printf("Error attaching to shared memory \n");
		exit(1);
	}
	char* shm_ptr = shm_data;
	bakeQ    = (QueueInfo *)shm_ptr;
	shm_ptr += sizeof(QueueInfo);
	deliverQ = (QueueInfo *)shm_ptr;
	shm_ptr += sizeof(QueueInfo);	
	
	queueInit(bakeQ, bQSize, (OrderNode *)shm_ptr);
	shm_ptr += (bQSize + 1) * sizeof(OrderNode);
	queueInit(deliverQ, dQSize, (OrderNode *)shm_ptr);
//------------------------------------------------------------
// CREATING CHILDREN PROCESSES AND CORRESPONDING SEMAPHORES
//------------------------------------------------------------
	//2 mutex semaphores for accessing each of the 2 queues in the shared memory
	bake_mutex    = sem_open(BAKE_MUTEX,    O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
	deliver_mutex = sem_open(DELIVER_MUTEX, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
	//A counting semaphores to indicate how many pizzas need baking 
	bake_sem    = sem_open(BAKE_SEM,    O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
	//A counting semaphores to indicate how many orders need delivering
	deliver_sem = sem_open(DELIVER_SEM, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
	//Initial values above are 0 so that all baker and deliver processes
	//will be created and fall asleep till an order comes
	if(bake_sem == SEM_FAILED || deliver_sem == SEM_FAILED ){	
		printf("Error creating a semaphore \n");
		exit(1);
	}
	if(bake_mutex == SEM_FAILED || deliver_mutex == SEM_FAILED ){	
		printf("Error creating a mutex semaphore \n");
		exit(1);
	}	
	//Spawn the baker and delivery_boy children and start their respective code
	for(i=0; i<N_PSISTES; i++){ 
		pid = fork();
		if(pid == 0){
			baker();
		}
	}	
	for(i=0; i<N_DIANOMEIS; i++){
		pid = fork();
		if(pid == 0){
			delivery_boy();
		}
	}
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
	if( listen(sd, SOCKET_QUEUE) == -1){
		printf("Error listening from socket \n");  
		exit(1);
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
		
		pid = fork(); // Spawn a child to deal with each new order
		if(pid == 0){
			int r;
			close(sd);
			r = read(conSd, &(order), sizeof(PizzaOrder));
			if(r<=0){
				printf("Problem in reading from a client \n");
				printf("Connection closing... \n");
				close(conSd); 
				kill(getpid(), SIGINT); //Exit and deallocate resources
			}
			if(bakeQ->currentSize == bQSize){ // Check for queue overflow
				char msg[]="Server is currently busy. Try again later";
				write(conSd, MSG_OK, sizeof(MSG_OK));
				write(conSd, msg, sizeof(msg));
				close(conSd);
				kill(getpid(), SIGINT); //Exit and deallocate resources
			}
				
			printOrder(order); //DEBUG
			for(i=0; i < order.N_pizza; i++){ 
				//Set each pizza of the new order to be unbaked
				order.status[i] = STATUS_UNBAKED; 
			}
			order.readyPizza = 0;
			order.nextPizza = 0;
			//Store the pid of the procces responsible for the order
			order.orderPid = getpid(); 
			order.delays = -1;
			
			sem_wait(bake_mutex);	
			pushOrder(bakeQ, order); //Push the order to the queue			
			if(bakeQ->nextOrder == NULL){
				bakeQ->nextOrder = bakeQ->tail;
			}
			sem_post(bake_mutex);			
			//UP the semaphore as many times as the number of the pizzas in the new
			//order so that the correct number of baker-proccesses can be awake
			for(i=0; i < order.N_pizza; i++){
				sem_post(bake_sem);
			}
			//Pause the currect process until the correct signal is send 
			//(SIGUSR1 - indicating that the order has arrived) which will end the 
			//process correctly. In case of another signal (SIGUSR2 - indicating
			//that the order is late), the process will pause again after the
			//invoked signal handling function
			while(1){ 
				pause();
			}				
		}
		close(conSd);
	}		
	
	return 0;
}

void baker(void){
	OrderNode *curNode=NULL;
	int curPizza;
	struct timespec sleep_t;
	sleep_t.tv_sec = 0;
	while(1){
		sem_wait(bake_sem);  //One pizza less to bake	
			
		sem_wait(bake_mutex);			
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
		sem_post(bake_mutex);				
		
		sleep_t.tv_nsec = bakingTimes[ curNode->theOrder.types[curPizza]] * A_MILLION;
		nanosleep(&sleep_t, NULL); //Bake it!	
		
		sem_wait(bake_mutex);			
		curNode->theOrder.status[curPizza] = STATUS_READY;
		curNode->theOrder.readyPizza++;
		//If the order your are dealing with is in the head of the queue,
		//and if all the pizza in this order are ready, pop it from the
		//baking-queue, push it to the delivering-queue and UP the semaphore
		//to indicate that another delivery-boy process needs to be awake
		//Do the same for every next order that is ready (till one that
		// is not ready is found)
		while(curNode == bakeQ->head && curNode != NULL){
			if(curNode->theOrder.readyPizza == curNode->theOrder.N_pizza){
				pushOrder(deliverQ, popOrder(bakeQ));
				sem_post(deliver_sem);
				curNode = curNode->next;
			}else{
				break;
			}
		}	
		sem_post(bake_mutex);
	}
	exit(1);
}
void delivery_boy(void){
	PizzaOrder deliverOrder;
	int timeToDeliver;
	struct timespec sleep_t; 
	sleep_t.tv_sec = 0;
	while(1){
		sem_wait(deliver_sem);	//One order less to deliver	
		
		sem_wait(deliver_mutex);  					
		deliverOrder = popOrder(deliverQ); //Take the first order
		//Calculare the time to deliver it depending on the distance
		timeToDeliver = distanceTimes[deliverOrder.distance]; 
		sem_post(deliver_mutex);		
		
		sleep_t.tv_nsec = timeToDeliver * A_MILLION;
		nanosleep(&sleep_t, NULL); //Deliver it!	
		//Notify the responsible process that the order has arrived		
		kill(deliverOrder.orderPid, SIGUSR1);
	}	
	exit(1); 
}
//Used to shut down the server and its children while deallocating 
//the appropriate system resources including shared memory and semaphores
void catch_int(int sig_num){
	close(conSd);
	close(sd);
	if( shmdt(shm_data) == -1){
		printf("Error detaching from shared memory\n");
		exit(1);
	}
	if(sem_close(bake_sem) + sem_close(deliver_sem)< 0){
		printf("Error closing a semaphore \n");
		exit(1);
	}
	if(sem_close(bake_mutex) + sem_close(deliver_mutex)< 0){
		printf("Error closing a mutex semaphore \n");
		exit(1);
	}
	if(parentPid == getpid()){
		if( shmctl(shm_id, IPC_RMID, NULL) == -1){
			printf("Error deleting shared memory \n");
			exit(1);
		}
		if( sem_unlink(BAKE_SEM) + sem_unlink(DELIVER_SEM) < 0){
			printf("Error unlinking a semaphore name \n");
			exit(1);
		}
		if( sem_unlink(BAKE_MUTEX) + sem_unlink(DELIVER_MUTEX) < 0){
			printf("Error unlinking a mutex semaphore name \n");
			exit(1);
		}
		printf("\n%s has closed for today! \n\n", PIZZERIA);
	}	
	exit(0);
}
//Used to avoid creating zombie processes
void catch_deadChild(int sig_num){	
	pid_t pid;
	int status;
	while( (pid = waitpid(-1, &status, WNOHANG)) > 0);	
}
//Used to notify the client that his order has arrived
void catch_orderArrived(int sig_num){
	char msg[MESSAGE_SIZE];
	strcpy(msg, "Your food is here! Thanks for prefering us.");
	write(conSd, MSG_OK, sizeof(MSG_OK));
	write(conSd, msg, sizeof(msg));
	close(conSd);
	kill(getpid(), SIGINT);
}
//Used to notify the client that his order will be late
void catch_orderLate(int sig_num){
	char msg[MESSAGE_SIZE];
	strcpy(msg,"Your delivery will be late. We apologize and we offer a coca cola for free");
	write(conSd, MSG_OTHER, sizeof(MSG_OTHER));
	write(conSd, msg, sizeof(msg));
}
//Used to traverse the queues periodically and check for delayed orders.
void catch_alarm(int sig_num){
	OrderNode* temp;
	int d;
	sem_wait(bake_mutex);
	sem_wait(deliver_mutex);
	temp = bakeQ->head;
	while(temp!=NULL){
		d = ++(temp->theOrder.delays);		
		if( d == 1 ){
			kill(temp->theOrder.orderPid, SIGUSR2);
		}
		temp = temp->next;
	}
	temp = deliverQ->head;
	while(temp!=NULL){
		d = ++(temp->theOrder.delays);		
		if( (d == 1) ){
			kill(temp->theOrder.orderPid, SIGUSR2);
		}
		temp = temp->next;
	}
	sem_post(bake_mutex);
	sem_post(deliver_mutex);
}



