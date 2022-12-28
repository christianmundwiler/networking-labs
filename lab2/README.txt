**************************************************************************************************************

*  Name      :  Christian Mundwiler
*  Student ID:  109498954
*  Class     :  CSCI 3800

**************************************************************************************************************

READ ME

**************************************************************************************************************

Description:

This project consists of two programs: a server and a client. These programs are implemented and communicate
via STREAM sockets.

The server program is opened in a terminal window and asks for a port number from the user and binds a 
socket to that port number. It then loops, waiting for data to be sent from client. It prints out the 
data that is sent. Server stays open until user types control-c on server side.

The client program is also opened in a terminal window, but the user will input the IP address of the server
in addition to the port number. It loops, prompting user to input strings. It then sends those strings to the 
server until user types 'STOP'. Once user types 'STOP', client exits, but does not send 'STOP' to server. 

Since program is using STREAM sockets, server is aware of client closing. When that occurs, server waits for new connection on initial port number.  

**************************************************************************************************************

File names:  

server2.c, client2.c

**************************************************************************************************************

Status of programs:

The programs were written in c and created on Xcode for Mac. Programs run as intended. 
