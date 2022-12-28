#	Makefile for client program
#
#	Christian Mundwiler
#	CSCI 3800
#	Written: 5.1.22

#Compile with gcc
CC = gcc
OBJCS = client10.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall -lm -D__GNU_SOURCE

# setup for the system
nLIBS =

all: client10

client10: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
	
clean:
	rm client10
