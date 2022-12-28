//  client.c
//  CSCI 3800
//  Lab 3
//  Created by Christian Mundwiler on 2/10/22.
/*
    This program is a DGRAM socket client. It reads in a text file titled "config.txt" that consists of IP addresses and port numbers,
    one each to a line. It then sends user specified strings to the servers located at the IP addresses in the config.txt file. It will
    loop, continuously asking for data to send until user types 'STOP'. Once user types 'STOP', client exits, but does not send 'STOP'
    to server.
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

#define MAX_SERVERS 4

int main(int argc, char *argv[]) {
    // define variables
    int sockD; // socket decriptor
    char buff[100]; // buffer to send data
    int rc = 0; // variable to hold bytes sent
    char stopStr[] = "STOP\n";
    bool userStop = false;
    int count = 0; // counter
    char buf[29]; // buffer to hold data from text file
    
    // struct to hold server data
    struct Servers {
        int portNum;
        char serverIP[29];
        int hostNum;
    };
    
    // arrays of structs
    struct Servers mystruct_array[MAX_SERVERS];
    struct sockaddr_in serverData[MAX_SERVERS];
    
    // get data from file
    FILE *fp;
    fp = fopen("config.txt", "r");
    if (fp == NULL) {
        perror("Error reading file");
        exit(1);
    }
    
    // extract data from text file
    while (fgets(buf, 29, fp)!=NULL) {
        int slen = strlen(buf);
        
        // allocate memory
        char *str = (char *)malloc((slen) * sizeof(char));
        
        // copy buffer to string
        strncpy(str, buf, strlen(buf));
        
        // get first string from line
        char *ptr;
        ptr = strtok(str, " ");
        
        // copy into struct
        strcpy(mystruct_array[count].serverIP, &str[0]);
                
        // get second string
        ptr = strtok(NULL, "\n");
        
        // copy int into struct
        mystruct_array[count].portNum = htons(atoi(ptr));
        
        // set host number
        mystruct_array[count].hostNum = count+1;
                
        // increment counter
        count++;
    }
    
    fclose(fp);
    
    printf("Done with config file\n");
    // close file
    
    // create socket; specify IPv4 address family with AF_INET and datagram socket with SOCK_DGRAM
    sockD = socket(AF_INET, SOCK_DGRAM, 0);
    
    // make sure socket created successfully
    if(sockD == -1) {
        perror("Error creating socket");
        exit(1);
    }
        
    // set address family, port number, and server IPs for server struct data
    for (int i = 0; i < MAX_SERVERS; i++) {
        serverData[i].sin_family = AF_INET;
        serverData[i].sin_port = mystruct_array[i].portNum;
        serverData[i].sin_addr.s_addr = inet_addr(mystruct_array[i].serverIP);
    }
    
    while (!userStop && sockD != -1) {
        
        // prompt user for string
        printf("\nEnter your string:\n");
        
        // get and place string in buffer
        fgets(buff, 100, stdin);
        
        // check user input
        if(strcmp(buff, stopStr) == 0) {
            userStop = true;
        }
        
        if(!userStop){
        
            for (int i = 0; i < MAX_SERVERS; i++) {
                
                // send string to servers
                rc = sendto(sockD, buff, strlen(buff), 0, (struct sockaddr *) & serverData[i], sizeof(serverData[i]));
                
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
                printf("\nYou sent: '%s', which is %lu bytes.\n", buff, strlen(buff));
            }
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
