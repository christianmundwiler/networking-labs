#	Makefile for client and server c program
#
#	Christian Mundwiler
#	CSCI 3800/Users/christianmundwiler/Documents/school/spring22/csci3800/lab1/Makefile
#	Written: 2.10.22

#Compile with gcc
CC = gcc
OBJCS = client3.c
OBJCSS = server3.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall

# setup for the system
nLIBS =

all: client3 server3
	
server3: $(OBJCSS)
	$(CC) $(CFLAGS) -o $@ $(OBJCSS) $(LIBS)
	
client3: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
		
clean:
	rm server3 client3
