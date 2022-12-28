//  server2.c
//  CSCI 3800
//  Lab 2
//  Created by Christian Mundwiler on 2/5/22.
/*
    This program is a STREAM socket server. It receives a port number from the user and binds a STREAM socket to that port number. It then
    performs a listen(), and then loops, waiting for the client to connect to it. Once it connects, it will read data being sent and then print it out.
    It will do this until the client side goes away. It will then close the current connection and wait for another connection request.
 */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket description
    int connected_sockD; // socket description
    int rc = 1; // code returned from receiving function
    struct sockaddr_in server_addr; // address of server
    struct sockaddr_in client_addr; // address of client
    int flags = 0; // flag data
    int portNum;    // user input port #
    char buff[100]; // buffer to hold received data
    socklen_t fromLength; // length of received data
    bool serverOpen = false; // bool for determining socket success
    
    // cosmetic spacing
    printf("\n");
    
    if(argc < 2) {
        // inform user to enter port number, exit
        printf("ERROR: usage is server <port>. Try again.\n");
        exit(1);
    }
    
    // create socket; specify IPv4 address family with AF_INET and STREAM socket with SOCK_STREAM
    sockD = socket(AF_INET, SOCK_STREAM, 0);
    
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
    int sockSuccess = bind(sockD, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
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
    
        // variable to hold listen() command success
        int listenRetVal = listen(sockD, 5);
        
        // check if listen() worked correctly
        if (listenRetVal == 0) {
            printf("Socket is listening...\n\n");
        }
        
        else {
            perror("Error");
            exit(1);
        }
    
        // attempt to accept() connection from client
        connected_sockD = accept(sockD, (struct sockaddr *) &client_addr, &fromLength);
        
        if(connected_sockD > 0) {
            printf("Socket connected...\n\n");
        }
        
        else {
            perror("Error connecting to client");
        }
        
        // reset value for rc in order to enter loop
        rc = 1;
        
        // receive data from socket, enter loop
        while(rc > 0) {
            
            rc = recvfrom(connected_sockD, &buff, 100, flags, (struct sockaddr *)&client_addr, &fromLength);
        
            // cut newline char from string
            buff[strcspn(buff, "\n")] = 0;
        
            // print received string
            if (strlen(buff) != 0) {
                printf("I received the following: '%s', which is %lu bytes.\n\n", buff, strlen(buff));
            }
        
            // clear buffer
            memset(buff, 0, 100);
            
            // check value of recvfrom for errors
            if (rc <= 0) {
                printf("Client closed socket. Waiting for new connection...\n\n");
            }

        }

    }
    
    return 0;
}
