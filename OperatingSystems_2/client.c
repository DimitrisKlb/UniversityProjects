/********************************************************************
 *  Each client will connect and give an order to the server using  *
 * the info specified in the pizzeriaInfo.h. The order will either  *
 * be user-specified if no arguments were passed during the main    *
 * call, or be random in any other case.                            *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "pizzeriaInfo.h"

int main(int argc, char* argv[]){
	int i, r;
	int sd;
	struct sockaddr_un serverAddress;	
	int serverAddrLength;	
	PizzaOrder order;
	char msg[MESSAGE_SIZE]; //Message recieved from server
	char msg_code[2]; //Code recieved from the server
	strcpy(msg_code, MSG_OTHER);

	if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ){
		printf("Error creating socket\n");    
		exit(1);
	}
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, SOCKET_PATH);
	serverAddrLength = strlen(serverAddress.sun_path) + sizeof(serverAddress.sun_family);

	if( connect(sd, (struct sockaddr *)&serverAddress, serverAddrLength ) == -1){
		printf("Error connecting to the socket \n");
		exit(1);
	}
	
	if( argc==1 ){ 
		order = takeOrder();	
	}else{
		srand((int)(time(NULL)*getpid()));  //Mult with pid to create a "random-er" seed
		order.N_pizza = rand() % MAX_PIZZA + 1;
		order.distance = rand() % 2;
		for(i=0; i<order.N_pizza; i++){
			order.types[i] = rand() % 3;
		}				
	}
	write(sd, &(order), sizeof(PizzaOrder));	//Send the order ot the server
	//Client will wait connected to server, printing the received
	//messages till the code MSG_OK is received
	while( strcmp(msg_code, MSG_OK) != 0 ){
		r = read(sd, msg_code, sizeof(msg_code)); //Sleep and wait for messages
		if(r <= 0){
			printf("Problem in reading from server \n");
			printf("Connection closing... \n");
			exit(1);
		}
		read(sd, msg, sizeof(msg));
		printf("%s \n",msg);
	}
	close(sd);	 
	return 0;
}












