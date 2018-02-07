#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/un.h>
#include <netinet/in.h>  
#include <netdb.h>

#include "webInfo.h"

void catch_int(int sig_num);  
int sd;

int main(int argc, char* argv[]){
	struct sockaddr_in serverAddress;	
	int serverAddrLength;
	int server_port;	
	struct hostent *server;	
	
	WebRequest theRequest;
	
	int d;
	char code;
	char theKeyword[MAX_ID_LENGTH];
	char theWebpage[MAX_WEBPAGE_LENGTH];
	
	signal(SIGINT, catch_int);
//------------------------------------------------------------
// CHEKING ARGUMENTS
//------------------------------------------------------------	
	if(argc != 4){
		printf("Usage: client –s <server_IP> <server_port> \n");
		exit(1);
	}	
	server_port = atoi(argv[3]);
	server = gethostbyname(argv[2]);	
//------------------------------------------------------------
// INITIALIZING TCP SOCKET 
//------------------------------------------------------------
	if( (sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("Error creating socket\n");    
		exit(1);
	}
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;	
	serverAddress.sin_port = htons(server_port);
	serverAddrLength = sizeof(serverAddress);
	bcopy((char *)server->h_addr,(char *)&serverAddress.sin_addr.s_addr, server->h_length);	

	if( connect(sd, (struct sockaddr *)&serverAddress, serverAddrLength ) == -1){
		printf("Error connecting to the socket \n");
		exit(1);
	}
//------------------------------------------------------------
// CLIENT OPERATIONAL
//------------------------------------------------------------	
	printf("You have been connected to the server \n");	
	printf("You can exit any time by pressing Ctrl+C \n");
	while(1){		
		printf("Enter your HTTP Request: \n");

		while(1){	
			scanf("%s %s", theKeyword, theWebpage);
	
			if( strcmp(theKeyword, "GET") == 0 ){
				theRequest.getId = ID_GET;
				break;
			}else if( strcmp(theKeyword, "GETNEW") == 0 ){
				theRequest.getId = ID_GETNEW;	
				break;
			}else{
				printf("Wrong code! \n");
				printf("Usage: <GET>|<GETNEW> <web_url> \n");
				printf("Try again: \n");
			}
		}	
		strcpy(theRequest.webpage, theWebpage);
		code = ERROR_OTHER;
		write(sd, &(theRequest), sizeof(WebRequest));	
	
	//Read and output the reply	
		read(sd, &code, sizeof(code));
		if( code == ERROR_NONE){
			printf("O server mou steile code: %d\n", code);
			printf("HTML Reply:\n");
			printf("----------------------------------------\n");		
			char g = '\n';
			while( g != EOF ){
				printf("%c", g);
				if( (d = read(sd, &g, sizeof(char))) < 0 ){
					printf("\nError reading from server\n");
					kill(getpid(), SIGINT);
				}
			}		
			printf("\n----------------------------------------\n");		
		}else{
			printf("%s \n", error_messages[code]);
			printf("Please confirm the given URL and try again.\n");
			kill(getpid(), SIGINT);
		}	
	}
	
	exit(1); //Execution should not reach this point
}	

void catch_int(int sig_num){
	close(sd);
	printf("\nClient terminated! \n");		
	exit(0);
}

