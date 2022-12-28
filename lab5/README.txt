**************************************************************************************************************

*  Name      :  Christian Mundwiler
*  Student ID:  109498954
*  Class     :  CSCI 3800

**************************************************************************************************************

READ ME

**************************************************************************************************************

Description:

This project consists of one client program. This program is implemented and communicates via DGRAM sockets.

The client program is opened in a terminal window, with user specifying the port number and node location, 
reads in a text file named "config.txt" that lists various IP addresses and port number, one each to a line.
The program then prompts the user to enter the dimensions of the grid the nodes are located in, in rows and 
columns. It then loops, prompting user to input strings. It then sends those strings to the nodes located at 
the addresses listed in the config.txt file, until user types 'STOP' or closes the program. When a node 
receives a message it parses the protocol, determines where it was sent from, and finds the distance to the 
sending node. If the node is more than 2 spaces away, it prints 'OUT OF RANGE'. If the node is 2 or less 
spaces away, it prints 'IN RANGE. IF the sending node's location isn't in the grid the program prints 'NOT IN 
GRID'. Once user types 'STOP', client exits, but does not send 'STOP' to the listening nodes. The client also 
has the ability to listen to the network, and receive messages from other nodes. It does this via the select() 
function. 

The program also incorporates the class-defined protocol for message sending. The protocol defines that 
messages be sent with this format: (version):(message type):(sender location):(message) 
and that messages are limited to 100bytes, including the header.  

**************************************************************************************************************

File names:  

client4.c

**************************************************************************************************************

Status of programs:

The program were written in c and created on Xcode for Mac. Program runs as intended. 
