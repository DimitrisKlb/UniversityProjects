#ifndef PIZZAINFO_H
#define PIZZAINFO_H

#define PIZZERIA "*Perfect Pizza*"   //The Pizzeria's name
#define MAX_PIZZA 3                  //Max pizza for each order
#define SOCKET_PATH "/tmp/socket" 
#define MESSAGE_SIZE 80              //max size of the messages sent to a client
//Codes associated with the server-client communication;
//Server will sent a code to the client before each message
#define MSG_OK "0"   //indicates that the following message will end the communication
#define MSG_OTHER "1" //any other case

extern char* pizzaNames[3]; //Different pizza type strings

enum { PIZZA_MARGARITA, PIZZA_PEPPERONI, PIZZA_SPECIAL}; //Different pizza type values (1)
enum { CLOSE, FAR}; //Different distance values (2)

//struct to store the details of and order
typedef struct{
	short int N_pizza;         //number of pizza
	short int readyPizza;      //number of baked pizza 
	short int types[MAX_PIZZA];   //pizza type values (1)
	short int status[MAX_PIZZA];
	short int distance;   // distance value (2)
	int clientSd;
	short int nextPizza;  //Indicates the next pizza that needs baking
	short int delays;     //Counts how many times the order was found in the queue
}PizzaOrder;                //during server's periodic checks for late orders

void printOrder(PizzaOrder order);
PizzaOrder takeOrder(void);

#endif // PIZZAINFO_H	