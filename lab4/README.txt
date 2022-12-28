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
It loops, prompting user to input strings. It then sends those strings to the nodes located at the addresses 
listed in the config.txt file, until user types 'STOP'. Once user types 'STOP', client exits, but does not 
send 'STOP' to the listening nodes. The client also has the ability to listen to the network, and receive 
messages from other nodes. It does this via the select() function.

The program also incorporates the class-defined protocol for message sending. The protocol defines that 
messages be sent with this format: (version):(message type):(sender location):(message) 
and that messages are limited to 100bytes, including the header.  

**************************************************************************************************************

File names:  

client4.c

**************************************************************************************************************

Status of programs:

The program were written in c and created on Xcode for Mac. Program runs as intended. 
