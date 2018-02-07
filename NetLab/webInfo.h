#ifndef WEBINFO_H
#define WEBINFO_H

#define MAX_ID_LENGTH 7 //Length of the GETNEW string code 
#define MAX_WEBPAGE_LENGTH 2083 //Webpage's max length 

enum{
	ID_GET, 
	ID_GETNEW
};

enum{
	ERROR_NONE,
	ERROR_CONTACT_WEB,
	ERROR_CONNECT_WEB,
	ERROR_COMMUNICATE,
	ERROR_OTHER
};

char *error_messages[]={ 
	"No error",
	"Error contacting the web server",
	"Error connecting to the web server",
	"Error communicating with the web server",
	"Unexpected Error ",
};

typedef struct{
	char getId;
	char webpage[MAX_WEBPAGE_LENGTH];
}WebRequest;


#endif // WEBINFO_H	
