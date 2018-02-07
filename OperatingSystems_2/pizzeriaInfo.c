#include "pizzeriaInfo.h"

#include <stdio.h>

char* pizzaNames[] = {"Margarita", "Pepperoni", "Special  "};

void printOrder(PizzaOrder order){
	int i;
	printf(" _________________________\n");  
	printf("|An order with %d pizza:   |\n", order.N_pizza);
	for(i=0; i<order.N_pizza; i++){
		printf("| Pizza %d: %s      |\n", i+1, pizzaNames[ order.types[i] ]);		
	}
	printf("|Delivery distance is: %d  |\n", order.distance);
	
	printf("|_________________________|\n");  
}

PizzaOrder takeOrder(void){
	PizzaOrder theOrder;
	int i;	

	printf("Welcome to %s \n", PIZZERIA);
	printf("You can order up to %d pizza. How many do you want? ", MAX_PIZZA);
	scanf("%hd", &theOrder.N_pizza);
	
	while(  theOrder.N_pizza < 1 || theOrder.N_pizza > MAX_PIZZA){
		printf("Sorry. We can only deliver up to %d pizza. So, how many? ",MAX_PIZZA);
		scanf("%hd", &theOrder.N_pizza);
	}	
	printf("Select the type of each pizza: \n");
	printf("Margarita 0 - Pepperoni 1 - Special 2 \n");
	for(i=0; i<theOrder.N_pizza; i++){		
		printf("Pizza %d: ", i+1);			
		scanf("%hd", &theOrder.types[i]);
		if(theOrder.types[i]<0 || theOrder.types[i]>2)
			theOrder.types[i] = PIZZA_MARGARITA;
	}
	printf("Close 0 - Far 1 \n");
	printf("How far are you? ");
	scanf("%hd", &theOrder.distance);
	if(theOrder.distance<0 || theOrder.distance>1)
		theOrder.distance = CLOSE;

	printf("Delivery is on the way. \n");
	return theOrder;
}


