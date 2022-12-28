//  client8.c
//  CSCI 3800
//  Lab 8
//  Created by Christian Mundwiler on 4/7/22.
/*
    This program is a DGRAM socket client. It asks user to specify IP address & port number, and then asks user for a destination port and message.
    It then sends the message to all nodes in a text file. It will loop, continuously asking for data to send until user exits.
    When a node receives a message it parses the protocol, determines where it was sent from, and finds the distance to the sending node. If the
    node is 2 or less spaces away and the message is for that node, it will print the message. If the message isn't for that node, it will forward
    it to all nodes not in the path list. If the node is more than 2 spaces away, it does nothing. If the sending node's location isn't in the grid
    the program prints 'NOT IN GRID'. This program also incorporates the concept of "hop count". When a message is sent, it has a hop count of 4.
    Before each forwarding of the message, the hop count is decremented by one. When the hop count reaches 0 it can't be forwarded again. When a node
    receives a message for itself, it sends an ack back to that original sender. When a node sends or forwards along a message or an ack, it appends
    its port number onto the path. This program also incorporates functionality to handle MOV commands; these commands include a location to move to.
    When a node receives this command, they move to the new location and update their coordinates accordingly.
    It runs on the class-defined protocol where messages are sent in this format:
    <version>:<msg type>:<sender location>:<origin port>:<destination port>:<hop count>:<msg id>:<path>:<msg>
    Max size for the message is 100bytes, including the header.
*/

#define __STDC_WANT_LIB_EXT2__ 1
#define _GNU_SOURCE
#define STDIN 0
#define MAX_NODES 30
#define MAX_SIZE 100
#define MAX_WIDTH 50
#define VERSION "4"

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

// struct to hold node data
struct Nodes {
    int portNum;
    char nodeIP[29];
    int location;
    int seqSent;
    int seqRec;
    int ackSent;
    int ackRec;
};

// struct to hold array of structs
struct Array {
    struct Nodes nodeInfo[MAX_NODES];
};

// struct for message data
typedef struct message {
    int locationFrom;
    int originPort;
    int destPort;
    int hopCount;
    int msgID;
    char versionNum[2];
    char path[45];
    char message[100];
    char msgType[5];

} message;

// function declarations
int readFile(struct Array *, int *, int *);
int sendString(int , char *, struct sockaddr_in);
int getCoord(int , int , int *, int *, int );
int getDistance(int , int , int , int );
void concat2(char *, struct message *);
struct message * fillMessage(char *, char *, int , int , int , int , int , char *, char *);
char* checkPath(char *, int );

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket decriptor
    char networkBuffer[MAX_SIZE]; // buffer to hold incoming data
    char protocolBuffer[MAX_SIZE]; // buffer to hold protocol data
    int flags = 0; // flag data
    int rc = 0; // returned code
    socklen_t fromLength; // length of received data
    struct sockaddr_in node_addr, from_addr;
    struct Array myNodes; // arrays of structs
    int portNumInt; // user input port #
    char myPortNum[5];
    int maxSD; // tells OS how many sockets are set
    fd_set socketFDS; // socket descriptor set
    int sockSuccess; // bind() checking
    char msgType1[] = "INFO";
    char msgType2[] = "ACK";
    char msgType3[] = "MOV";
    char myLocation[5]; // user-input node location
    int location = 0;
    int numRows = 0, numCols = 0; // user input for grid dimensions
    int myRow = 0, myCol = 0; // coordinates for my grid location
    int incomingRow = 0, incomingCol = 0; // coordinates for incoming message
    int distance = 0; // distance between nodes
    int destPort; // user input for destination port
    char userInput[100]; // user input buffer
    char userMsg[91]; // user msg buffer
    int msgID = 0; // sequence id
    int partners = 0; // number of nodes in file
    bool duplicate;
    int copyID = 0, copyPort = 0; // variables to check dups
    int copyIDF = 0, copyPortF = 0;
    
    if(argc < 3) {
        // inform user to enter port number, exit
        printf("ERROR: usage is client <port> <location>. \n");
        exit(1);
    }
    
    // grab user input for port #
    portNumInt = strtol(argv[1], NULL, 10);
    sprintf(myPortNum, "%d", portNumInt);
    
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
    node_addr.sin_port = htons(portNumInt);
    node_addr.sin_addr.s_addr = INADDR_ANY;
    
    // bind socket
    sockSuccess = bind(sockD, (struct sockaddr *)&node_addr, sizeof(node_addr));
    if (sockSuccess != 0) { // if bind is successful, set corresponding boolean value
        perror("Error binding socket"); // else inform user, exit
        exit(1);
    }
    
    // open and read from file
    partners = readFile(&myNodes, &numRows, &numCols);
    
    printf("My location is %s\n", myLocation);
    // get rows and columns
    getCoord(numRows, numCols, &myRow, &myCol, location);
    printf("My row is: %d. My column is: %d.\n", myRow, myCol);
    
    // prompt user for string
    printf("\nEnter: <destination port> & <message>:\n");
   
    for(;;) {
        // zero out buffers
        fflush(stdout);
        memset(networkBuffer, 0, MAX_SIZE), memset(protocolBuffer, 0, MAX_SIZE);
        
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
        
        // call select()
        rc = select(maxSD+1, &socketFDS, NULL, NULL, NULL);
        if(rc == -1) {
            perror("Error selecting");
        }
        
        // receive data from user
        if(FD_ISSET(STDIN, &socketFDS)) {
            // get input from user and place in buffer
            fgets(userInput, 100, stdin);
            // parse input
            if(sscanf(userInput, "%d %100[^\n]", &destPort, userMsg) != 2) {
                printf("Error: usage is <destination port #> <message>\n");
                exit(1);
            }
                        
            // cut newline char from string
            userMsg[strcspn(userMsg, "\n")] = 0;
            // find seqID, increment
            for(int i = 0; i < partners; i++) {
                if (destPort == htons(myNodes.nodeInfo[i].portNum)) {
                    msgID = myNodes.nodeInfo[i].seqSent;
                    myNodes.nodeInfo[i].seqSent++;
                }
            }
            // create message struct
            message *newMsg = NULL;
            // populate struct
            newMsg = fillMessage(VERSION, msgType1, location, portNumInt, destPort, 4, msgID, myPortNum, userMsg);
                        
            // concatenate protocol
            concat2(protocolBuffer, newMsg);
            
            printf("\nSending: %s\n", protocolBuffer);
            
            printf("\nEnter: <destination port> & <message>:\n");
            
            for (int i = 0; i < MAX_NODES; i++) {
                // set node data
                node_addr.sin_family = AF_INET;
                node_addr.sin_port = myNodes.nodeInfo[i].portNum;
                node_addr.sin_addr.s_addr = inet_addr(myNodes.nodeInfo[i].nodeIP);
                // send string to all nodes except own
                if(htons(portNumInt) != myNodes.nodeInfo[i].portNum) {
                    sendString(sockD, protocolBuffer, node_addr);
                }
            }
        }
        
        // receive data from network
        if(FD_ISSET(sockD, &socketFDS)) {
            // variables to hold received data
            char versionNumFrom[5];
            char msgTypeFrom[5];
            char locationFrom[5];
            char msgFrom[90];
            char portFrom[6];
            char portTo[6];
            char hopFrom[2];
            char hopTo[2];
            char msgIDFrom[2];
            char path[45];
            int hop;
            
            // receive data
            rc = recvfrom(sockD, &networkBuffer, MAX_SIZE, flags, (struct sockaddr *)&from_addr, &fromLength);
            // check value of recvfrom for errors
            if (rc < 0) {
                perror("Error receiving data");
                exit(1);
            }
            // print and parse received data
            if(sscanf(networkBuffer, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%100[^\n]", versionNumFrom, msgTypeFrom, locationFrom, portFrom, portTo, hopFrom, msgIDFrom, path, msgFrom) == 9) {
                // cut newline char from string
                networkBuffer[strcspn(networkBuffer, "\n")] = 0;

                // check version number
                if (strtol(versionNumFrom, NULL, 10) != strtol(VERSION, NULL, 10)) {
                    printf("%s", networkBuffer);
                    printf("\nIncompatable version number\n");
                    // prompt user for string
                    printf("\nEnter: <destination port> & <message>:\n");
                }
                
                // check if MOV msg
                if (strcmp(msgTypeFrom, msgType3) == 0) {
                    if (strtol(portTo, NULL, 10) == portNumInt) {
                        // update location and coordinates
                        location = strtol(msgFrom, NULL, 10);
                        getCoord(numRows, numCols, &myRow, &myCol, location);
                        printf("\nReceived: %s\n", networkBuffer);
                        printf("MOV command.\nMy new location: %d\n", location);
                        printf("My row is: %d. My column is: %d.\n", myRow, myCol);
                        printf("\nEnter: <destination port> & <message>:\n");
                    }
                }
                else {
                    // get coordinates of sender
                    getCoord(numRows, numCols, &incomingRow, &incomingCol, strtol(locationFrom, NULL, 10));
                    // get distance
                    distance = getDistance(myCol, incomingCol, myRow, incomingRow);
                    // decrement hop count
                    hop = strtol(hopFrom, NULL, 10) - 1;

                    sprintf(hopTo, "%d", hop);
                    // check if msg from close enough node and hop count is not 0
                    if (distance <= 2 && hop >= 0) {

                        // check if msg is for me
                        if (strtol(portTo, NULL, 10) == portNumInt) {
                            //get int of msgID
                            int msgIDFromInt = strtol(msgIDFrom, NULL, 10);
                            // check if INFO
                            if (strcmp(msgTypeFrom, msgType1) == 0) {
                                                                
                                // variables for ACK
                                int pFrom, pTo;
                                pTo = strtol(portFrom, NULL, 10);
                                pFrom = strtol(portTo, NULL, 10);
                                                                
                                // find sending node, set seqRec, ackSent
                                for(int i = 0; i < partners; i++) {
                                    if (pTo == htons(myNodes.nodeInfo[i].portNum)) {
                                        myNodes.nodeInfo[i].ackSent = msgIDFromInt;
                                        myNodes.nodeInfo[i].seqRec = msgIDFromInt;
                                    }
                                }
                                printf("\nRECEIVED:   '%s'\n", networkBuffer);
                                printf("INFO for me:\n");
                                // parse msg
                                printf("Source port: %s - ID: %d - Path: '%s'\nMsg: '%s'\n", portFrom, msgIDFromInt, path, msgFrom);
                                                                
                                // new message for sending ACK
                                message *ACK = NULL;
                                // fill msg
                                ACK = fillMessage(versionNumFrom, msgType2, location, pFrom, pTo, 4, msgIDFromInt, myPortNum, msgFrom);

                                memset(protocolBuffer, 0, MAX_SIZE);
                                // concatenate protocol
                                concat2(protocolBuffer, ACK);
                                
                                printf("\nYou are sending ACK:\n");
                                printf("%s\n", protocolBuffer);
                                
                                for (int i = 0; i < MAX_NODES; i++) {
                                    // set node data
                                    node_addr.sin_family = AF_INET;
                                    node_addr.sin_port = myNodes.nodeInfo[i].portNum;
                                    node_addr.sin_addr.s_addr = inet_addr(myNodes.nodeInfo[i].nodeIP);
                                    // send string to all nodes except own
                                    if(htons(portNumInt) != myNodes.nodeInfo[i].portNum) {
                                        sendString(sockD, protocolBuffer, node_addr);
                                    }
                                }
                                // prompt user for string
                                printf("\nEnter: <destination port> & <message>:\n");
                            }
                            
                            // check if ACK
                            if (strcmp(msgTypeFrom, msgType2) == 0) {
                                // check if duplicate
                                int portFromInt = strtol(portFrom, NULL, 10);
                                if (copyID == msgIDFromInt && copyPort == portFromInt) {
                                    duplicate = true;
                                }
                                else {
                                    duplicate = false;
                                }
                                // find sending node, increment ackRec
                                for(int i = 0; i < partners; i++) {
                                    if (portFromInt == htons(myNodes.nodeInfo[i].portNum)) {
                                        myNodes.nodeInfo[i].ackRec = msgIDFromInt;
                                    }
                                }
                                // check if dup
                                if (!duplicate) {
                                    printf("\nRECEIVED: '%s'\n", networkBuffer);
                                    printf("ACK for me.\n");
                                    // parse msg
                                    printf("Source location: %s - Source port: %s - Sequence ID: %d - Path: %s\nMsg: '%s'\n", locationFrom, portFrom, msgIDFromInt, path, msgFrom);
                                    // prompt user for string
                                    copyID = strtol(msgIDFrom, NULL, 10);
                                    copyPort = strtol(portFrom, NULL, 10);
                                    printf("\nEnter: <destination port> & <message>:\n");
                                }
                            }
                        }
                        // otherwise forward
                        else if (hop != 0) {
                            // clear buffer
                            memset(protocolBuffer, 0, MAX_SIZE);
                            
                            // check if duplicate
                            int msgIDFromIntF = strtol(msgIDFrom, NULL, 10);
                            int origPortF = strtol(portFrom, NULL, 10);
                            if (copyIDF == msgIDFromIntF && copyPortF == origPortF) {
                                duplicate = true;
                            }
                            else {
                                duplicate = false;
                            }
                            if(!duplicate)
                            {
                                // variables for forward
                                int pFromForw, pToForw, mIDForw;
                                pToForw = strtol(portTo, NULL, 10);
                                pFromForw = strtol(portFrom, NULL, 10);
                                mIDForw = strtol(msgIDFrom, NULL, 10);
                                
                                // add to path
                                strcat(path, ",");
                                strcat(path, myPortNum);
                                // new message for forward
                                message *forward = NULL;
                                // fill message
                                forward = fillMessage(versionNumFrom, msgTypeFrom, location, pFromForw, pToForw, hop, mIDForw, path, msgFrom);
                                
                                // concatenate protocol
                                concat2(protocolBuffer, forward);
                                
                                // print out data
                                printf("\nRECEIVED:   '%s'\n", networkBuffer);
                                printf("FORWARDING: '%s'\n", protocolBuffer);
                                
                                // send message
                                for (int i = 0; i < MAX_NODES; i++) {
                                    // set node data
                                    node_addr.sin_family = AF_INET;
                                    node_addr.sin_port = myNodes.nodeInfo[i].portNum;
                                    node_addr.sin_addr.s_addr = inet_addr(myNodes.nodeInfo[i].nodeIP);
                                    // send msg to all nodes except own and any in current path
                                    if(checkPath(path, myNodes.nodeInfo[i].portNum) == NULL) {
                                        sendString(sockD, protocolBuffer, node_addr);
                                    }
                                    else {
                                        printf("Didn't send to %d\n", htons(myNodes.nodeInfo[i].portNum));
                                    }
                                }
                                // prompt user for string
                                printf("Enter: <destination port> & <message>:\n");
                            }
                            //set copy variables
                            copyIDF = strtol(msgIDFrom, NULL, 10);
                            copyPortF = strtol(portFrom, NULL, 10);
                            
                        }
                    }
                    // check if node is out of grid
                    if (strtol(locationFrom, NULL, 10) > numRows*numCols) {
                        printf("\nNOT IN GRID\n");
                        printf("\nMy location: %d\nSending location: %s\nDistance: %d\n", location, locationFrom, distance);
                    }
                }
            }
        }
    }
    return 0;
}

// functions
// func to send message
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
    return 0;
}

// func to read in config file
int readFile(struct Array *structNodes, int *numRows, int *numCols) {
    // buffer to hold data from text file
    char buffer[29];
    // get data from file
    FILE *fp;
    fp = fopen("config.txt", "r");
    if (fp == NULL) {
        perror("Error reading file");
        exit(1);
    }
    int count = 0; // counters
    int count2 = 0;
    
    // extract data from text file
    while (fgets(buffer, 29, fp)!=NULL) {
        // get first string from line and copy to struct
        char *ptr;
        ptr = strtok(buffer, " ");
        // get rows, cols
        if (count2 == 0) {
            *numRows = strtol(&buffer[0], NULL, 10);
            ptr = strtok(NULL, "\n");
            *numCols = strtol(ptr, NULL, 10);
            count2 = 1;
        }
        else {
            strcpy(structNodes->nodeInfo[count].nodeIP, &buffer[0]);
            // get second string and copy to struct
            ptr = strtok(NULL, "\n");
            structNodes->nodeInfo[count].portNum = htons(atoi(ptr));
            // set id and ack sequence nums
            structNodes->nodeInfo[count].seqSent = 1;
            structNodes->nodeInfo[count].seqRec = 1;
            structNodes->nodeInfo[count].ackSent = 1;
            structNodes->nodeInfo[count].ackRec = 1;
            // increment counter
            count++;
        }
        memset(buffer, 0, 29);
    }
    printf("Done with config file\n");
    // close file
    fclose(fp);
    return count;
}

// func to find coords of node
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
    return (1);
}

// func to find distance btwn two nodes
int getDistance(int x1, int x2, int y1, int y2) {
    int dist = 0;
    dist = sqrt(pow((x2-x1), 2) + pow((y2-y1), 2));
    return dist;
}

// concat protocol
void concat2(char *protBuff, struct message *msg) {
    char *ptr;
    strcat(protBuff, msg->versionNum);
    strcat(protBuff, ":");
    strcat(protBuff, msg->msgType);
    strcat(protBuff, ":");
    asprintf(&ptr, "%d", msg->locationFrom);
    strcat(protBuff, ptr);
    strcat(protBuff, ":");
    asprintf(&ptr, "%d", msg->originPort);
    strcat(protBuff, ptr);
    strcat(protBuff, ":");
    asprintf(&ptr, "%d", msg->destPort);
    strcat(protBuff, ptr);
    strcat(protBuff, ":");
    asprintf(&ptr, "%d", msg->hopCount);
    strcat(protBuff, ptr);
    strcat(protBuff, ":");
    asprintf(&ptr, "%d", msg->msgID);
    strcat(protBuff, ptr);
    strcat(protBuff, ":");
    strcat(protBuff, msg->path);
    strcat(protBuff, ":");
    strcat(protBuff, msg->message);
    free(ptr);
}

// func to fill struct that holds message data
struct message * fillMessage(char *versionNum, char *msgT, int loc, int myPort, int theirPort, int hop, int mID, char *path, char *_msg) {
    struct message *msg = malloc(sizeof(struct message));
    strcpy(msg->versionNum, versionNum);
    strcpy(msg->msgType, msgT);
    msg->locationFrom = loc;
    msg->originPort = myPort;
    msg->destPort = theirPort;
    msg->hopCount = hop;
    msg->msgID = mID;
    strcpy(msg->path, path);
    strcpy(msg->message, _msg);
    return msg;
}

char* checkPath(char *path, int portNum) {
    char str[80];
    sprintf(str, "%d", htons(portNum));
    return(strstr(path, str));
}
