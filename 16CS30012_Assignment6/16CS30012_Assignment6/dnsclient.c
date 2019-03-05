// Ishwarkar Rohan Shankar
// Roll No:- 16CS30012
// Networks Lab Assignment 6

// DNS client program for requesting IP using datagram sockets
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <string.h>


// Change port and buffer size
#define PORT 50000 
#define MAXLINE 1024 

int main() 
{ 
    int sockfd; 
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr; 
  
    int n, len; 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        printf("Failed to create socket!\n"); 
        exit(0); 
    } 
    
    // Setting initially to zero
    memset(&servaddr, 0, sizeof(servaddr)); 
  
    // Server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    // Domain name 
    char buf[MAXLINE]="www.google.com";

    // Send to server
    sendto(sockfd, buf, strlen(buf),0,(struct sockaddr*)&servaddr,sizeof(servaddr)); 
    printf("Sent: %s\n",buf);

    // Receive ip from server
    len=sizeof(servaddr);
    n = recvfrom(sockfd, buffer, MAXLINE,0,(struct sockaddr*)&servaddr,&len); 
    buffer[n]='\0';

    // Printing the received IP address
    printf("IP:%s\n", buffer); 
    close(sockfd); 
    return 0; 
} 
