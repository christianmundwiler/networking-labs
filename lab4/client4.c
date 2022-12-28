//  client4.c
//  CSCI 3800
//  Lab 4
//  Created by Christian Mundwiler on 2/16/22.
/*
    This program is a DGRAM socket client. It asks user to specify IP address, port number, and location of node, and then sends user specified
    strings to the node. It will loop, continuously asking for data to send until user types 'STOP'. Once user types 'STOP', client exits, but
    does not send 'STOP' to node. It runs on the class-defined protocol where messages are sent in this format:
    (version):(message type):(sender location):(message)
    Max size for the message is 100bytes, including the header.
    The program also incorporates the ability to send and receive data, as opposed to needing separate client and server programs. This is accomplished
    using the select() function.
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
#include <time.h>
#include <sys/select.h>
#define STDIN 0
#define MAX_NODES 4
#define MAX_SIZE 100

// struct to hold node data
struct Nodes {
    int portNum;
    char nodeIP[29];
    int nodeLocation;
};

// struct to hold array of structs
struct Array {
    struct Nodes nodeInfo[MAX_NODES];
};

// function declarations
void readFile(struct Array *);
int sendString(int , char *, struct sockaddr_in);

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket decriptor
    char msgBuffer[89]; // buffer to hold message data
    char networkBuffer[MAX_SIZE]; // buffer to hold incoming data
    char protocolBuffer[MAX_SIZE]; // buffer to hold protocol data
    char stopStr[] = "STOP\n";
    int flags = 0; // flag data
    int rc = 0; // returned code
    socklen_t fromLength; // length of received data
    struct sockaddr_in node_addr;
    struct sockaddr_in from_addr;
    struct Array myNodes; // arrays of structs
    int portNum;    // user input port #
    int maxSD; // tells OS how many sockets are set
    fd_set socketFDS; // socket descriptor set
    int sockSuccess; // bind() checking
    static const char versionNum[4] = "1";
    static const char msgType[] = "INFO";
    char myLocation[5]; // user-input node location
    
    if(argc < 3) {
        // inform user to enter port number, exit
        printf("ERROR: usage is client <port> <location>. \n");
        exit(1);
    }
    
    // grab user input for port #
    portNum = strtol(argv[1], NULL, 10);
    
    // grab user input for node location
    strcpy(myLocation, argv[2]);
    
    // create socket; specify IPv4 address family with AF_INET and datagram socket with SOCK_DGRAM
    sockD = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockD == -1) {
        perror("Error creating socket");
        exit(1);
    }
    
    // set address family, port number, and set permission to accept data from any machine
    node_addr.sin_family = AF_INET;
    node_addr.sin_port = htons(portNum);
    node_addr.sin_addr.s_addr = INADDR_ANY;
    
    // bind socket
    sockSuccess = bind(sockD, (struct sockaddr *)&node_addr, sizeof(node_addr));
    if (sockSuccess != 0) { // if bind is successful, set corresponding boolean value
        perror("Error binding socket"); // else inform user, exit
        exit(1);
    }
    
    // open and read from file
    readFile(&myNodes);
   
    for(;;) {
        // zero out buffers
        memset(networkBuffer, 0, MAX_SIZE);
        memset(protocolBuffer, 0, MAX_SIZE);
        memset(msgBuffer, 0, 89);
        
        // set select() parameters
        FD_ZERO(&socketFDS);
        FD_SET(sockD, &socketFDS);
        FD_SET(STDIN, &socketFDS);
        // determine input source
        if(STDIN > sockD) {
            maxSD = STDIN;
        }
        else {
            maxSD = sockD;
        }
        
        // create protocol string
        strcat(protocolBuffer, versionNum);
        strcat(protocolBuffer, ":");
        strcat(protocolBuffer, msgType);
        strcat(protocolBuffer, ":");
        strcat(protocolBuffer, myLocation);
        strcat(protocolBuffer, ":");
        
        // prompt user for string
        printf("\nEnter your string:\n");
        // call select()
        rc = select(maxSD+1, &socketFDS, NULL, NULL, NULL);
        if(rc == -1) {
            perror("Error selecting");
        }
        
        // receive data from user
        if(FD_ISSET(STDIN, &socketFDS)) {
            // get input from user and place in buffer
            fgets(msgBuffer, 89, stdin);
            // if user entered 'STOP', close socket, exit program
            if(strcmp(msgBuffer, stopStr) == 0) {
                close(sockD);
                printf("\nYou typed: 'STOP': Node is closed.\n");
                exit(1);
            }
            // cut newline char from string
            msgBuffer[strcspn(msgBuffer, "\n")] = 0;
            // print to display message being sent
            printf("\nYou are sending '%s', the length of which is %lu bytes.\n", msgBuffer, strlen(msgBuffer));
            // append message to protocol buffer
            strcat(protocolBuffer, msgBuffer);
            
            for (int i = 0; i < MAX_NODES; i++) {
                // set node data
                node_addr.sin_family = AF_INET;
                node_addr.sin_port = myNodes.nodeInfo[i].portNum;
                node_addr.sin_addr.s_addr = inet_addr(myNodes.nodeInfo[i].nodeIP);
                // send string to all nodes except own
                if(htons(portNum) != myNodes.nodeInfo[i].portNum) {
                    sendString(sockD, protocolBuffer, node_addr);
                }
            }
        }
        
        // receive data from network
        if(FD_ISSET(sockD, &socketFDS)) {
            // variables to hold received data
            char versionNumFrom[4];
            char msgTypeFrom[5];
            char locationFrom[5];
            char msgFrom[90];
            // receive data
            rc = recvfrom(sockD, &networkBuffer, MAX_SIZE, flags, (struct sockaddr *)&from_addr, &fromLength);
            // check value of recvfrom for errors
            if (rc < 0) {
                perror("Error receiving data");
                exit(1);
            }
            // print and parse received data
            printf("\nMessage received:\nParsing: '%s'\n", networkBuffer);
            if(sscanf(networkBuffer, "%[^:]:%[^:]:%[^:]:%100[^\n]", versionNumFrom, msgTypeFrom, locationFrom, msgFrom) == 4) {
                // cut newline char from string
                networkBuffer[strcspn(networkBuffer, "\n")] = 0;
                // print out data
                printf("Version: %s\nCommand: %s\nLocation: %s\nMessage: '%s'\n", versionNumFrom, msgTypeFrom, locationFrom, msgFrom);
            }
        }
    }
    return 0;
}





// functions
int sendString(int sockD, char *buffer, struct sockaddr_in node_addr) {
    int rc = 0;
    // send string to nodes
    rc = sendto(sockD, buffer, strlen(buffer), 0, (struct sockaddr *) & node_addr, sizeof(node_addr));
    // check if string sent
    if(rc < 0) {
        // if not, close socket, exit program
        perror ("Error sending data");
        close(sockD);
        exit(1);
    }
    // display number of bytes sent
    printf("You sent %lu bytes.\n", strlen(buffer));
    return 0;
}

void readFile(struct Array *structNodes) {
    // buffer to hold data from text file
    char buffer[29];
    // get data from file
    FILE *fp;
    fp = fopen("config.txt", "r");
    if (fp == NULL) {
        perror("Error reading file");
        exit(1);
    }
    int count = 0; // counter
    // extract data from text file
    while (fgets(buffer, 29, fp)!=NULL) {
        // get first string from line and copy to struct
        char *ptr;
        ptr = strtok(buffer, " ");
        strcpy(structNodes->nodeInfo[count].nodeIP, &buffer[0]);
        // get second string and copy to struct
        ptr = strtok(NULL, "\n");
        structNodes->nodeInfo[count].portNum = htons(atoi(ptr));
        // increment counter
        count++;
        memset(buffer, 0, 29);
    }
    printf("Done with config file\n");
    // close file
    fclose(fp);
}
