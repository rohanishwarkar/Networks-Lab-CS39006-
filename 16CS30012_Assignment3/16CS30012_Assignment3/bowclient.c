// Ishwarkar Rohan Shankar
// Roll No:- 16CS30012
// Networks Lab Assignment 3


// TCP Client program for bag of words fetch
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <netdb.h> 

// Change port and buffer size
#define PORT 5000 
#define MAXLINE 1024 

// This is individual word size for printing each word
#define WORDSIZE 1024

int main() 
{ 
    int sockfd; 
    char buffer[MAXLINE];  
    struct sockaddr_in servaddr; 
  
    int n, len; 
    // Creating TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("Failed to create socket!\n"); 
        exit(0); 
    } 
    
    // Setting 
    memset(&servaddr, 0, sizeof(servaddr)); 
  
    // Server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

  
    if (connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) { 
        printf("Connection Failed! \n");
        exit(0);
    } 
    int count=0,size1,wl=0;
    char singleword[WORDSIZE];
    while((size1=recv(sockfd,buffer,sizeof(buffer),0))>0){
        buffer[size1]='\0';
        // Counting number of words
        for(int i=0;i<size1;i++){
            if(buffer[i]=='\0'){
                singleword[wl]='\0';
                printf("Received word: %s\n",singleword);
                count++;
                wl=0;
            }
            else{
                singleword[wl]=buffer[i];
                wl++;
            }
        }
    }
    printf("Number of words received: %d\n",count);
    close(sockfd); 
} 
