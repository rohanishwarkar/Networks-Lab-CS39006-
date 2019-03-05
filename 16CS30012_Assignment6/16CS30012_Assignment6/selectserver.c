// Ishwarkar Rohan Shankar
// Roll No:- 16CS30012
// Networks Lab Assignment 6


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
#include <fcntl.h>

// Change port and buffer size
#define PORT 50000 
#define MAXLINE 1024 
#define WORDSIZE 1024

// Start of main program
int main() 
{ 
    // Socket descriptors
    int tcpfd,udpfd,status; 
    char buffer[MAXLINE]; 
    socklen_t len;      
    struct sockaddr_in cli_addr, serv_addr; 

    //************************************ TCP Socket ************************************//

    tcpfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(tcpfd<0){
        printf("Socket creation failed!\n");
        exit(0);
    }
    // Set Non blocking socket
   	status = fcntl(tcpfd, F_SETFL, fcntl(tcpfd, F_GETFL, 0) | O_NONBLOCK);
	if (status == -1){
	  perror("Error setting non blocking!");
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
    // Set non blocking socket
    status = fcntl(udpfd, F_SETFL, fcntl(udpfd, F_GETFL, 0) | O_NONBLOCK);
	if (status == -1){
	  perror("Error setting non blocking!");
	  exit(0);
	  
	}

    while(1) { 
    	ssize_t n;
        len = sizeof(cli_addr); 
        memset(buffer,0,sizeof(buffer)) ;
        // Receive domain name from client and dont wait
        n = recvfrom(udpfd, buffer, sizeof(buffer), MSG_DONTWAIT,(struct sockaddr*)&cli_addr, &len);
        // If UDP Client ie data is available to receive
        if(n!=-1){
        	// Fork Child process for futher processing
        	if (fork() == 0) { 
        	buffer[n]='\0' ;
	        printf("Recieved domain Name: %s\n",buffer); 
	        char *name;
	        // Using gethostbyname to get the ip
	        struct hostent *add=gethostbyname(buffer);
	        name = inet_ntoa(*((struct in_addr*)add->h_addr_list[0]));            
	        strcpy(buffer,name);
	        printf("IP: %s\n",buffer);
	        sendto(udpfd, buffer, sizeof(buffer), 0,(struct sockaddr*)&cli_addr, sizeof(cli_addr));
	        exit(0);
	    	}

        }         
        // Check for tcp connection
        len = sizeof(cli_addr); 
        int newfd = accept(tcpfd, (struct sockaddr*)&cli_addr, &len); 
        if(newfd!=-1){
        	// Creating new process using fork
            if (fork() == 0) { 
                // Child process
                close(tcpfd);  
                // Closing the original tcpfd in child process
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
            
    } 
    return 0;
} 
