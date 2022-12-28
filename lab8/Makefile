#	Makefile for client program
#
#	Christian Mundwiler
#	CSCI 3800
#	Written: 4.7.22

#Compile with gcc
CC = gcc
OBJCS = client8.c

#use -g for gnu debugger and -Wall for displaying warnings
CFLAGS = -g -Wall -lm -D__GNU_SOURCE

# setup for the system
nLIBS =

all: client8

client8: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)
	
clean:
	rm client9
