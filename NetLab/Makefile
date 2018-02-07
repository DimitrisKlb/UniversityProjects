CC = gcc -Wall

default: server client

server: server.c webInfo.h
	$(CC) server.c -o server

client : client.c webInfo.h
	$(CC) client.c -o client

