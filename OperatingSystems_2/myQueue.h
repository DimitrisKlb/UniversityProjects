/*****************************************************************
*  A custom made queue that is placed in an already allocated    *
* memory and stores a PizzaOrder struct in each node             *
*****************************************************************/
#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdio.h>
#include <stdlib.h>

#include "pizzeriaInfo.h"

typedef struct order_node{
	PizzaOrder theOrder;
	struct order_node *next;
}OrderNode;

//Struct that contains the info of the queue
typedef struct{
	OrderNode *head, *tail;
	//The addreress-limits of the queue in the memory (shared)
	OrderNode *initialPosition, *lastPosition; 
	OrderNode *nextPosition;  //Memory address in which the next node will be stored
	int size, currentSize;
	OrderNode *nextOrder; //Points to the node which contains the next order 
}QueueInfo;                 //that needs "dealing with" (baking, delivering, etc)

//Initializes the queue theQueue
void queueInit(QueueInfo *theQueue, int size, OrderNode *initialPos); 
PizzaOrder popOrder(QueueInfo *theQueue);
void pushOrder(QueueInfo *theQueue, PizzaOrder order);

#endif // MYQUEUE_H	