#	Makefile for client and server c program
#
#	Christian Mundwiler
#	CSCI 3800/Users/christianmundwiler/Documents/school/spring22/csci3800/lab2/Makefile
#	Written: 2.8.22

#Compile with gcc
CC = gcc
OBJCS = client2.c
OBJCSS = server2.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall

# setup for the system
nLIBS =

all: client2 server2

client2: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
	
server2: $(OBJCSS)
	$(CC) $(CFLAGS) -o $@ $(OBJCSS) $(LIBS)
		
clean:
	rm client2 server2
