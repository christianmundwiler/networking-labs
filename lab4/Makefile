#	Makefile for client c program
#
#	Christian Mundwiler
#	CSCI 3800
#	Written: 2.16.22

#Compile with gcc
CC = gcc
OBJCS = client4.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall

# setup for the system
nLIBS =

all: client4

client4: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
		
clean:
	rm client4
