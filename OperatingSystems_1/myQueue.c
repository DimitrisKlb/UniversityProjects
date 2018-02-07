#include "myQueue.h"

void queueInit(QueueInfo *q, int theSize, OrderNode *initialPos){
	q->head = NULL; 
	q->tail = NULL; 
	q->nextOrder = NULL;
	q->size = theSize;
	q->currentSize = 0;
	q->initialPosition = initialPos;
	q->nextPosition = q->initialPosition;
	q->lastPosition = q->initialPosition + q->size;
}

PizzaOrder popOrder(QueueInfo *theQueue){
 	PizzaOrder order;
 	order.N_pizza = -1;
	if(theQueue->head == NULL){
		return order;
	}
	order = theQueue->head->theOrder;
	theQueue->head = theQueue->head->next;
	if(theQueue->head == NULL){
		theQueue->tail = NULL;
		theQueue->nextPosition = theQueue->initialPosition;	
	}
	theQueue->currentSize--;
	return order;
}

void pushOrder(QueueInfo *theQueue, PizzaOrder order){
	if(theQueue->currentSize == theQueue->size)
		return;
			
	OrderNode node;
	node.theOrder = order;
	node.next = NULL;
	if(theQueue->head == NULL){
		theQueue->head = theQueue->initialPosition;
		theQueue->tail = theQueue->initialPosition;
		*(theQueue->head) = node;		
	}else{
		theQueue->tail->next = theQueue->nextPosition;
		theQueue->tail = theQueue->nextPosition;
		*(theQueue->tail) = node;
	}
	theQueue->currentSize++;
	if(theQueue->nextPosition == theQueue->lastPosition){
		theQueue->nextPosition = theQueue->initialPosition;
	}else{
		theQueue->nextPosition++;
	}	
}



