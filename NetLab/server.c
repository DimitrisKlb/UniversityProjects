#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/un.h>
#include <netinet/in.h>   
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#include "webInfo.h"

#define SOCKET_QUEUE   30  //Socket's queue length 
#define WEB_PORT 80
#define WEBPAGE_FOLDER "saved_pages"
#define BUFFER_SIZE 1000
#define PATH_SIZE 200

void catch_int(int sig_num);  
void catch_deadChild(int sig_num);  
//This function is used to terminate both client and the associated server subprocess in case
//of an error, as well as inform the former about the specific error that occured
void terminate(int code);
int checkFilePath(char* webpage, char* actualFileName);
char* getFileName(char* webpage);
void printMessage(char* message);

int sd, cliSd, webSd; //Socket descriptors 
char *request = NULL;
char *buffer = NULL;
char *fname = NULL;
pid_t parentPid;	

int main(int argc, char* argv[]){	
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	struct sockaddr_in webServerAddress;	
	struct hostent *webServer;
	
	socklen_t clientAddrLength;	
	socklen_t webServerAddrLength;
	int portno; 
	
	WebRequest theRequest;	
	char *correctUrl;
	FILE *fp;		
	char code = ERROR_NONE;
	int exists = 0;
	char rootWorkingDir[PATH_SIZE];

	int pid = 0;
	parentPid = getpid();
	signal(SIGINT, catch_int);
	signal(SIGCHLD, catch_deadChild);
//------------------------------------------------------------
// CHEKING ARGUMENTS
//------------------------------------------------------------	
	if(argc != 3){
		printf("Usage: server -p <server_port> \n");
		exit(1);
	}
	portno = atoi(argv[2]);	
//------------------------------------------------------------
// INITIALIZING SERVER-CLIENT TCP SOCKET 
//------------------------------------------------------------
	if( (sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("Error creating socket \n");     
		exit(1);
	}
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port   = htons(portno);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	if( bind(sd,(struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1 ){
		printf("Error binding socket \n"); 
		exit(1);
	}
	if( listen(sd, SOCKET_QUEUE) == -1){
		printf("Error listening from socket \n");  
		exit(1);
	}
	
//------------------------------------------------------------
// MAKE THE FOLDER FOR SAVING THE FILES
//------------------------------------------------------------		
	mkdir(WEBPAGE_FOLDER, 0700); 
	chdir(WEBPAGE_FOLDER); 
	getcwd(rootWorkingDir, sizeof(rootWorkingDir));
//------------------------------------------------------------
// SERVER OPERATIONAL 
//------------------------------------------------------------	
	printf("Server is running! \n");  
	
	while(1){	
	//Wait for a client to connect			
		clientAddrLength = sizeof(clientAddress);
		cliSd = accept(sd, (struct sockaddr*)&clientAddress, &clientAddrLength);		
		if(cliSd == -1){
			if(errno == EINTR){
				continue;
			}else{
				printf("Error in accepting from socket \n");
				exit(1);
			}
		}		
		
		pid = fork(); //Spawn a child to deal with each request
		if( pid == 0 ){	
			printMessage("A client was connected!");
			close(sd);	
			while(1){	
				chdir(rootWorkingDir);
				int r;			
				r = read(cliSd, &(theRequest), sizeof(WebRequest));
				
				if(r<=0){
					if( errno == EEXIST || errno == ECHILD ){ //If the client disconnected
						printMessage("The client disconnected");	
					}else{
						printMessage("Problem in reading from a client");
					}
					printMessage("Connection closing...");
					close(cliSd); 
					kill(getpid(), SIGINT); //Exit and deallocate resources
				}
				printMessage("Request received"); 
				printMessage("Processing web request..."); 
				correctUrl = strchr(theRequest.webpage, 'w');	
			//Get the actual name of the file and check if it exists
				fname = getFileName(correctUrl);
				exists = checkFilePath(correctUrl, fname);								
	
				if( theRequest.getId == ID_GETNEW || exists == 0){ //IF GETNEW or file doesn't exist
				//Obtain the host and the specific page from the the sent webpage string	
					char *host = NULL;
					char *page = NULL; 
					host = strtok(correctUrl, "/");
					page = host + strlen(host) + 1;
					if( strlen(page) == 1 ){
						page = "\0";
					}			
					
				//Find the web server			
					if( (webServer = gethostbyname(host)) == NULL ){
						printMessage("Error resolving the host");
						terminate(ERROR_CONTACT_WEB);
					}
	
				//Initialize the web socket and connect to it
					if( (webSd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
						printMessage("Error connecting to the web server");    
						terminate(ERROR_CONNECT_WEB); 
					}
					bzero((char *) &webServerAddress, sizeof(webServerAddress));
					webServerAddress.sin_family = AF_INET;	
					webServerAddress.sin_port = htons(WEB_PORT);
					webServerAddrLength = sizeof(webServerAddress);
					bcopy((char *)webServer->h_addr,(char *)&webServerAddress.sin_addr.s_addr, webServer->h_length);	
	
					if( connect(webSd, (struct sockaddr *)&webServerAddress, webServerAddrLength ) == -1){
						printMessage("Error connecting to the socket");
						terminate(ERROR_CONNECT_WEB); 
					}	
			
				//Create the request
					char *requestForm = "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n";
					request = (char *)malloc(strlen(requestForm) + strlen(host) + strlen(page));
					sprintf(request, requestForm, page, host);	
		
					buffer = (char *)malloc(BUFFER_SIZE);		
					if(buffer == NULL){
						printMessage("Error allocating buffer memory");
						terminate(ERROR_OTHER);
					}
	
				//Send the request
					int d = 0;
					int bytesSent = 0;
					int requestLength = strlen(request);
					while( bytesSent < requestLength ){
						if( (d = write(webSd, request+bytesSent, requestLength-bytesSent)) == -1 ){
							printMessage("Error sending the request");
							terminate(ERROR_COMMUNICATE);
						}else{
							bytesSent += d;
						}			
					}
	
				//Receive html and save to a file
					if( (fp = fopen(fname, "w")) ==  NULL ){
						printMessage("Error creating file");
						terminate(ERROR_OTHER);
					}	
					while( (d = read(webSd, buffer, sizeof(buffer)-1)) > 0 ){					
						fprintf(fp, "%s", buffer); 					
					}
					fclose(fp);
					if( d == -1 ){
						printMessage("Error reading from the web server");
						terminate(ERROR_COMMUNICATE);
					}
				}
			//Now send it to the client			
				if( (fp = fopen(fname, "r")) ==  NULL ){
					terminate(ERROR_OTHER);
				}
				write(cliSd, &code, sizeof(code));  
				char g;	
				while( feof(fp) == 0 ){				
					g = fgetc(fp);
					write(cliSd, &g, sizeof(char));			
				}	
				fclose(fp);	
				printMessage("Reply was sent to the client"); 	
			}	
		} // end of child's code
		
		close(cliSd);				
	} //end while(1)	
	
	exit(1); //Execution should not reach this point
}

int checkFilePath(char* webpage, char* actualFileName){
	int exists = 0;
	char *last = NULL;
	char *tempFolder = NULL;
	
	last = strrchr(webpage, '/');
	
	if(last == NULL){
		mkdir(webpage, 0700);
		chdir(webpage);
		if( access(actualFileName, F_OK) != -1 ){ 
			exists = 1;	   
		}else{
			exists = 0;	
		}		
	}else{	
		*last = '\0';
		if( chdir(webpage) == -1){			//If the folder-path not exist		
			tempFolder = strtok(webpage, "/");
			while( tempFolder != NULL){		//Create the folder-path			
				mkdir(webpage, 0700);
				tempFolder = strtok(NULL, "/");
				if(tempFolder != NULL){
					*(tempFolder - 1) = '/';
				}
			}
			chdir(webpage);
			*last = '/';	
			exists = 0;				//and inform that the file does not exist
		}else{  //Else, if the filepath exists
			if( access(actualFileName, F_OK) != -1 ){ //check for the actual file!
				exists = 1;	   	//It does
			}else{
				exists = 0;			//It does not
			}
		}	
		*last = '/';
	}
	
	return exists;		
}

char* getFileName(char* webpage){
	char *last = NULL;
	char *hostName = "host";
	
	last = strrchr(webpage, '/');
	if(last != NULL){
		last++;
	}

	if(last == NULL || *last == '\0'){
		last = (char *)malloc(sizeof(hostName));		
		strcpy(last, hostName);
		return last;
	}else{
		return last;
	}
	
}

void terminate(int code){
	write(cliSd, &code, sizeof(code));
	kill(getpid(), SIGINT); //Exit and deallocate resources
}

void printMessage(char* message){
	printf("[%d] %s \n", getpid(), message);
}

void catch_int(int sig_num){	
	if(request != NULL){		
		free(request);
	}
	if(buffer != NULL){		
		free(buffer);
	}
	if(buffer != NULL){		
		free(fname);
	}
	close(sd);
	close(cliSd);
	close(webSd);	
	if( parentPid == getpid()){
		printf("\nServer terminated! \n");		
	}
	exit(0);
}

void catch_deadChild(int sig_num){
	pid_t pid;
	int status;
	while( (pid = waitpid(-1, &status, WNOHANG)) > 0);	
}


