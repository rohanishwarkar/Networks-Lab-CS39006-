// Ishwarkar Rohan Shankar
// Roll No:- 16CS30012
// Networks Lab Assignment 3


// Server program for both UDP and TCP clients
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
#define WORDSIZE 1024

// Start of main program
int main() 
{ 
    // Socket descriptors
    int tcpfd,udpfd; 
    char buffer[MAXLINE]; 
    socklen_t len;
    fd_set rs;      
    struct sockaddr_in cli_addr, serv_addr; 

    //************************************ TCP Socket ************************************//

    tcpfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(tcpfd<0){
        printf("Socket creation failed!\n");
        exit(0);
    }

    memset(&serv_addr,0,sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(PORT); 
  
    // Binding to local address 
    int b = bind(tcpfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(b<0){
        printf("Bind failed!\n");
        exit(0);
    }
    listen(tcpfd, 10); 
  
    //************************************ UDP Socket ************************************//

    udpfd = socket(AF_INET, SOCK_DGRAM, 0); 
    b = bind(udpfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(b<0){
        printf("Bind failed!\n");
        exit(0);
    }
    // Set descriptors to zero initially 
    FD_ZERO(&rs); 
  
    // Get maximum of two descriptors to set the limit 
    int maxi = (tcpfd>udpfd)?tcpfd:udpfd+ 1; 

    while(1) { 
  
        // Set tcpfd and udpfd 
        FD_SET(tcpfd, &rs); 
        FD_SET(udpfd, &rs); 
  
        // Using select statement to select ready descriptor 
        select(maxi, &rs, NULL, NULL, NULL); 
  
        //************************************ TCP part ************************************//
        if (FD_ISSET(tcpfd, &rs)) { 
            printf("\n\nTCP running!\n");
            len = sizeof(cli_addr); 
            int newfd = accept(tcpfd, (struct sockaddr*)&cli_addr, &len); 
            // Creating new process using fork
            if (fork() == 0) { 
                // Child process
                close(tcpfd);  // Closing the original tcpfd in child process
                FILE *ptr;      
                ptr = fopen("word.txt","r");        // Open file
                if(ptr==NULL){
                    printf("File not found on server!\n");
                    exit(0);
                }
                printf("File opened! Reading words!\n");
                char word[WORDSIZE];
                while(fscanf(ptr,"%s",word)==1){
                    // Read words one by one and send by appending \0 in the end to mark the end.
                    word[strlen(word)]='\0';
                    printf("Read word: %s. Sending it to client!\n",word);
                    send(newfd,word, strlen(word)+1, 0);                    
                }
                fclose(ptr);
                close(newfd); 
                exit(0); 
            } 
            close(newfd); 
        } 
        //************************************ UDP part ************************************// 
        if (FD_ISSET(udpfd, &rs)) { 
	    printf("\n\nUDP Running!\n");
            ssize_t n;
            len = sizeof(cli_addr); 
            memset(buffer,0,sizeof(buffer)) ;
            // Receive domain name from client
            n = recvfrom(udpfd, buffer, sizeof(buffer), 0,(struct sockaddr*)&cli_addr, &len);
            buffer[n]='\0' ;
            printf("Recieved domain Name: %s\n",buffer); 
            char *name;
            // Using gethostbyname to get the ip
            struct hostent *add=gethostbyname(buffer);
            name = inet_ntoa(*((struct in_addr*)add->h_addr_list[0]));            
            strcpy(buffer,name);
            printf("IP: %s\n",buffer);
            sendto(udpfd, buffer, sizeof(buffer), 0,(struct sockaddr*)&cli_addr, sizeof(cli_addr)); 
        } 
    } 
    return 0;
} 
