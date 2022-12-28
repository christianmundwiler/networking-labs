//  client.c
//  CSCI 3800
//  Lab 1
//  Created by Christian Mundwiler on 1/22/22.
/*
    This program is a DGRAM socket client. It asks user to specify IP address and port number of server, and then sends user specified
    strings to the server. It will loop, continuously asking for data to send until user types 'STOP'. Once user types 'STOP',
    client exits, but does not send 'STOP' to server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket decriptor
    struct sockaddr_in server_addr; // struct for server data
    char buff[100]; // buffer to send data
    int portNum;  // port number of server
    char serverIP[29]; // IP address of server
    int rc = 0; // variable to hold bytes sent
    char stopStr[] = "STOP\n";
    bool userStop = false;
    
    if (argc < 3) {
        // inform user to enter the IP address and port number of server, exit
        printf ("ERROR: usage is client <ipaddr> <port>. Try again.\n");
        exit(1);
    }
    
    // create socket; specify IPv4 address family with AF_INET and datagram socket with SOCK_DGRAM
    sockD = socket(AF_INET, SOCK_DGRAM, 0);
    
    // grab user input for server IP and port #, from command line
    portNum = strtol(argv[2], NULL, 10);
    strcpy(serverIP, argv[1]);
    
    // modify server data structure
    // set address family
    server_addr.sin_family = AF_INET;
    // get port number in correct byte order, set port to user input
    server_addr.sin_port = htons(portNum);
    // convert IP to address with inet_addr, set IP address
    server_addr.sin_addr.s_addr = inet_addr(serverIP);
    
    // make sure socket created successfully
    if(sockD == -1) {
        perror("Error creating socket");
        exit(1);
    }
    
    // enter loop if user hasn't entered 'STOP' and socket was created successfully
    while (!userStop && sockD != -1) {
        // prompt user for string
        printf("Enter your string:\n");
        
        // get and place string in buffer
        fgets(buff, 100, stdin);
        
        // check user input
        if(strcmp(buff, stopStr) == 0) {
            userStop = true;
        }
        
        // if user didn't type 'STOP', send data
        if(!userStop) {
            // send string to server
            rc = sendto(sockD, buff, strlen(buff), 0, (struct sockaddr *) & server_addr, sizeof(server_addr));
            
            // cut newline char from string
            buff[strcspn(buff, "\n")] = 0;
            
            // check if string sent
            if(rc < 0) {
                // if not, close socket, exit program
                perror ("Error sending data");
                close(sockD);
                exit(1);
            }
            
            // display number of bytes sent
            printf("\nYou sent: '%s', which is %lu bytes.\n\n", buff, strlen(buff));
            memset(buff, 0, 100);
        }
    }
    
    // if user entered 'STOP', close socket, exit program
    if(userStop) {
        close(sockD);
        printf("\nYou typed: 'STOP'. Client is closed.\n");
        exit(1);
    }
    
    return 0;
}
