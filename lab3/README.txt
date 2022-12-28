**************************************************************************************************************

*  Name      :  Christian Mundwiler
*  Student ID:  109498954
*  Class     :  CSCI 3800

**************************************************************************************************************

READ ME

**************************************************************************************************************

Description:

This project consists of two programs: a server and a client. These programs are implemented and communicate
via DGRAM sockets.

The server program is opened in a terminal window and asks for a port number from the user and binds a 
datagram socket to that port number. It then loops, waiting for data to be sent from client. It prints out the 
data that is sent. Server stays open until user types control-c on server side.

The client program is also opened in a terminal window, reads in a text file named "config.txt" that lists
Various IP addresses and port number, one each to a line. It loops, prompting user to input strings. It then 
sends those strings to the servers located at the addresses listed in the config.txt file, until user types 
'STOP'. Once user types 'STOP', client exits, but does not send 'STOP' to server. 

**************************************************************************************************************

File names:  

server3.c, client3.c

**************************************************************************************************************

Status of programs:

The programs were written in c and created on Xcode for Mac. Programs run as intended. 
