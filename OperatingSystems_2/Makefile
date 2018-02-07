CC = gcc -Wall
OBJECTS = server.o myQueue.o pizzeriaInfo.o
FLAGS = -lrt -lpthread
default: server client

server: $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS)  -o server  
server.o: server.c pizzeriaInfo.h myQueue.h
	$(CC) -c server.c
myQueue.o: myQueue.c pizzeriaInfo.h
	$(CC) -c myQueue.c
pizzeriaInfo.o: pizzeriaInfo.c pizzeriaInfo.h
	$(CC) -c pizzeriaInfo.c
client: client.o  pizzeriaInfo.o
	$(CC) client.o pizzeriaInfo.o -o client
client.o: client.c pizzeriaInfo.h
	$(CC) -c client.c
	
clean:
	rm *.o
