//
//  test.c
//  
//
//  Created by Christian Mundwiler on 2/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>


#define MAX_SERVERS 30

int main(int argc, char *argv[]) {
    
    int count = 0;
    
    struct Servers {
        int portNum;
        char serverIP[14];
    };
    
    struct sockaddr_in mystruct_array[MAX_SERVERS];
    
    FILE *fp;
    fp = fopen("config.txt", "r");
    
    if (fp == NULL)
    {
        perror("Error reading file");
    }

    char buf[29];
    
    while (fgets(buf, 29, fp)) {
        // cut newline from string
        
        //buf[strlen(buf) - 1] = '\0';
        
        int slen = strlen(buf);
        
        char *str = (char *)malloc((slen) * sizeof(char));
        
        // copy string from buffer to string
        strncpy(str, buf, strlen(buf));
        
        char *ptr;
        ptr = strtok(str, " ");
        
        mystruct_array[count].sin_addr.s_addr = str[0];
        
        ptr = strtok(NULL, "\n");
        
        mystruct_array[count].sin_port = atoi(ptr);
        
        count++;
        
        free(str);
    }
    
    
    for(int i = 0; i < MAX_SERVERS; i++)
    {
        printf("%u %d\n", mystruct_array[i].sin_addr.s_addr, mystruct_array[i].sin_port);
    }
    
    return(0);
}

