#	Makefile for client and server c program
#
#	Christian Mundwiler
#	CSCI 3800/Users/christianmundwiler/Documents/school/spring22/csci3800/lab1/Makefile
#	Written: 1.24.22

#Compile with gcc
CC = gcc
OBJCS = client.c
OBJCSS = server.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall

# setup for the system
nLIBS =

all: client server

client: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
	
server: $(OBJCSS)
	$(CC) $(CFLAGS) -o $@ $(OBJCSS) $(LIBS)
		
clean:
	rm client server
