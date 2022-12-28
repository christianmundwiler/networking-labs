//  server.c
//  CSCI 3800
//  Lab 1
//  Created by Christian Mundwiler on 1/22/22.
/*
    This program is a DGRAM socket server. It receives a port number from the user and binds a datagram socket to that port number. It then
    loops, waiting for data to be sent from client. It prints out the data that is sent. Server stays open until user types control-c on
    server side.
 */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket descriptor
    int rc; // code returned from receiving function
    struct sockaddr_in server_addr; // address of server
    struct sockaddr_in client_addr; // address of client
    int flags = 0; // flag data
    int portNum;    // user input port #
    char buff[100]; // buffer to hold received data
    socklen_t fromlength; // length of received data
    bool serverOpen = false; // bool for
    int sockSuccess;
    
    if(argc < 2) {
        // inform user to enter port number, exit
        printf("ERROR: usage is server <port>. Try again.\n");
        exit(1);
    }
    
    // create socket; specify IPv4 address family with AF_INET and datagram socket with SOCK_DGRAM
    sockD = socket(AF_INET, SOCK_DGRAM, 0);
    
    // grab user input for port # from command line
    portNum = strtol(argv[1], NULL, 10);
    
    // create structure to hold client data
    // set address family
    server_addr.sin_family = AF_INET;
    // get port number in correct byte order, set port to user input
    server_addr.sin_port = htons(portNum);
    // set to accept data from any of machine's interfaces with INADDR_ANY
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // make sure socket created successfully
    if (sockD == -1) {
        perror("Error creating socket");
        exit(1);
    }
    
    // bind socket
    sockSuccess = bind(sockD, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // if bind is successful, set corresponding boolean value
    if (sockSuccess == 0) {
        serverOpen = true;
    }
    
    // else inform user, exit
    else {
        perror("Error binding socket");
        exit(1);
    }
    
    // if bind was successful, wait for data to be received
    while(serverOpen) {
        // receive data from socket
        rc = recvfrom(sockD, &buff, 100, flags, (struct sockaddr *)&client_addr, &fromlength);
        
        // check value of recvfrom for errors
        if (rc < 0) {
            perror("Error receiving data");
            exit(1);
        }
        
        // cut newline char from string
        buff[strcspn(buff, "\n")] = 0;
        
        // print received string
        printf("I received the following: '%s', which is %lu bytes.\n\n", buff, strlen(buff));
        
        // clear buffer
        memset(buff, 0, 100);
    }
    
    return 0;
}
