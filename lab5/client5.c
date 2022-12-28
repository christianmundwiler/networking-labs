//  client4.c
//  CSCI 3800
//  Lab 5
//  Created by Christian Mundwiler on 3/3/22.
/*
    This program is a DGRAM socket client. It asks user to specify IP address, port number, and location of node, and then sends user specified
    strings to the node. It will loop, continuously asking for data to send until user types 'STOP'. Once user types 'STOP', client exits, but
    does not send 'STOP' to node. When a node receives a message it parses the protocol, determines where it was sent from, and finds the distance
    to the sending node. If the node is more than 2 spaces away, it prints 'OUT OF RANGE'. If the node is 2 or less spaces away, it prints
    'IN RANGE. IF the sending node's location isn't in the grid the program prints 'NOT IN GRID'. Once user types 'STOP', client exits, but does
    not send 'STOP' to the listening nodes. It runs on the class-defined protocol where messages are sent in this format:
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
#include <math.h>

#define STDIN 0
#define MAX_NODES 25
#define MAX_SIZE 100
#define MAX_WIDTH 50
#define MAX_POSITION 2500

// struct to hold node data
struct Nodes {
    int portNum;
    char nodeIP[29];
};

// struct to hold array of structs
struct Array {
    struct Nodes nodeInfo[MAX_NODES];
};

// function declarations
void readFile(struct Array *);
int sendString(int , char *, struct sockaddr_in);
int getCoord(int , int , int *, int *, int );
int getDistance(int , int , int , int );


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
    struct sockaddr_in node_addr, from_addr;
    struct Array myNodes; // arrays of structs
    int portNum;    // user input port #
    int maxSD; // tells OS how many sockets are set
    fd_set socketFDS; // socket descriptor set
    int sockSuccess; // bind() checking
    static const char versionNum[4] = "1";
    static const char msgType[] = "INFO";
    char myLocation[5]; // user-input node location
    int location = 0;
    int numRows = 0, numCols = 0; // user input for grid dimensions
    int myRow = 0, myCol = 0; // coordinates for my grid location
    int incomingRow = 0, incomingCol = 0; // coordinates for incoming message
    int distance = 0; // distance berween nodes
    
    if(argc < 3) {
        // inform user to enter port number, exit
        printf("ERROR: usage is client <port> <location>. \n");
        exit(1);
    }
    
    // grab user input for port #
    portNum = strtol(argv[1], NULL, 10);
    
    // grab user input for node location
    strcpy(myLocation, argv[2]);
    
    // copy into int
    location = strtol(myLocation, NULL, 10);
        
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
    
    //grab grid dimensions, check for errors
    printf("\nPlease enter the numbers of rows and columns: ");
    if (scanf("%d %d", &numRows, &numCols) != 2) {
        printf("Error: usage is <rows> <columns>\n");
        exit(1);
    }
    // clear input buffer
    while ((getchar()) != '\n');
    // check for input errors
    if(numRows < 1 || numRows > 25) {
        printf("Error: number of rows and columns must each be between 1 and 25.\n");
        exit(1);
    }
    
    printf("My location is %s\n", myLocation);
    // get rows and columns
    getCoord(numRows, numCols, &myRow, &myCol, location);
   
    for(;;) {
        // zero out buffers
        fflush(stdout);
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
                // send string to all nodes except own // have to revise for later labs
                // if(htons(portNum) != myNodes.nodeInfo[i].portNum && ) {
                sendString(sockD, protocolBuffer, node_addr);
                //}
            }
        }
        
        // receive data from network
        if(FD_ISSET(sockD, &socketFDS)) {
            // variables to hold received data
            char versionNumFrom[5];
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
                // get coordinates of sender
                getCoord(numRows, numCols, &incomingRow, &incomingCol, strtol(locationFrom, NULL, 10));
                // get distance
                distance = getDistance(myCol, incomingCol, myRow, incomingRow);
                
                // print out data
                printf("Version: %s\nCommand: %s\nLocation: %s\nMessage: '%s'\n", versionNumFrom, msgTypeFrom, locationFrom, msgFrom);
                
                // check if node is out of grid
                if (strtol(locationFrom, NULL, 10) > numRows*numCols) {
                    printf("\nNOT IN GRID\n");
                    printf("\nMy location: %d\nSending location: %s\nDistance: %d\n", location, locationFrom, distance);
                }
                // check if node in range
                else if (distance <= 2) {
                    printf("\nIN RANGE My location: %d, Sending location: %s, Distance: %d\n", location, locationFrom, distance);
                }
                else {
                    printf("\nOUT OF RANGE My location: %d, Sending location: %s, Distance: %d\n", location, locationFrom, distance);
                }
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

int getCoord(int m, int n, int * rowC, int * colC, int location) {
    // get row
    if (location % n != 0) {
        *rowC = (location / n) + 1;
    }
    else {
        *rowC = (location / n);
    }
    // get column
    if (location % n != 0) {
        *colC = (location % n);
    }
    else {
        *colC = n;
    }
    // check if in grid
    if (*rowC > m) {
        printf("Your location is not in grid\n");
        return(-1);
    }
    else {
        printf("My row is: %d. My column is: %d.\n", *rowC, *colC);
    }
    return (1);
}

int getDistance(int x1, int x2, int y1, int y2) {
    int dist = 0;
    dist = sqrt(pow((x2-x1), 2) + pow((y2-y1), 2));
    return dist;
}
