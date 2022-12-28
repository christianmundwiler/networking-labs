#	Makefile for client program
#
#	Christian Mundwiler
#	CSCI 3800
#	Written: 3.14.22

#Compile with gcc
CC = gcc
OBJCS = client6.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall -lm

# setup for the system
nLIBS =

all: client6

client6: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
	
clean:
	rm client6
