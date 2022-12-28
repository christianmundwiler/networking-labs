**************************************************************************************************************

*  Name      :  Christian Mundwiler
*  Student ID:  109498954
*  Class     :  CSCI 3800

**************************************************************************************************************

READ ME

**************************************************************************************************************

Description:

This project consists of one client program. This program is implemented and communicates via DGRAM sockets.

The client program is opened in a terminal window, with user specifying the port number, then
reads in a text file named "config.txt" that lists various IP addresses and port number, one each to a line.
The program then prompts the user to enter the dimensions of the grid the nodes are located in, in rows and 
columns. It then loops, prompting user to input a destination port and message. It then sends those strings 
to all nodes located at the addresses listed in the config.txt file. When a node receives a message it parses 
the protocol, determines where it was sent from, and finds the distance to the sending node. If the node is 
more than 2 spaces away, it does nothing. If the node is 2 or less spaces away and the message is for itself, 
it prints the message and doesn't forward the message on. If the message isn't for the node but is from at 
most two spaces away, it then forwards the message to all of the nodes.
If the sending node's location isn't in the grid the program prints 'NOT IN GRID'.

The program also incorporates the class-defined protocol for message sending. The protocol defines that 
messages be sent with this format:
<version>:<message type>:<sender location>:<origin port>:<destination port>:<hop count>:<message>

**************************************************************************************************************

File names:  

client6.c

**************************************************************************************************************

Status of programs:

The program were written in c and created on Xcode for Mac. Program runs as intended. 
