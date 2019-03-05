// Ishwarkar Rohan Shankar
// Roll :- 16CS30012
// Networks Lab Assignment 5
// Client side implementation


// Header files
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/wait.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <netdb.h>    
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>


#define PORT 		50000 
#define MAXLINE 	1024 
#define BLOCKSIZE 	10




// Main function
int main() 
{ 

    // Declaration of variables
	int sockfd1,filesize,fp; 
    char buffer[MAXLINE],filename[MAXLINE]; 
    struct sockaddr_in servaddr;   
    int n, len;

    // Set to zero initially
    memset(&servaddr, 0, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Socket creation
    if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("Failed to create socket!\n"); 
        exit(0); 
	}
	int option = 1;
	setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	// Connect to server
	if (connect(sockfd1,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) { 
        printf("Connection Failed! \n");
        exit(0);
	}   

	// Get filename from client
	printf("Enter file name: ");
	scanf("%[^\n]s", filename);

	// Send filename to server
	send(sockfd1, filename, strlen(filename)+1, 0);
	for(int i=0; i < MAXLINE; i++) buffer[i] = '\0';

	// Wait till one character is received using MSG_WAITALL
	len = recv(sockfd1, buffer, 1, MSG_WAITALL);
	// File does not exists
	if(buffer[0]=='E'){
		printf("File not found on server!\n");
	}
	else{
		// Receive filesize and wait till it is received using MSG_WAITALL
		len = recv(sockfd1, &filesize, 4, MSG_WAITALL);
		// Create or overwrite file
		fp = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		// Find number of complete_blocks and last_block_size
		int complete_blocks = filesize/BLOCKSIZE,last_block_size = filesize%BLOCKSIZE;
		for(int i =0; i < complete_blocks ; i++){
			// Receive complete blocks using MSG_WAITALL
			len = recv(sockfd1, buffer, BLOCKSIZE, MSG_WAITALL);
			// Write to file
			write(fp,buffer,len);
		}
		// Receive last block
		len = recv(sockfd1,buffer,last_block_size,MSG_WAITALL);
		write(fp,buffer,len);
		// Printing 
		printf("The file transfer is successful!\n");
		printf("File size is: %d\n",filesize);
		printf("Blocksize is: %d\n",BLOCKSIZE);
		printf("No of blocks is: %d\n",complete_blocks+((last_block_size==0)?0:1));
		if(last_block_size==0&&filesize!=0)
			printf("Last block size is: %d\n",BLOCKSIZE);
		else
			printf("Last block size is: %d\n",last_block_size);
		
	}
	close(sockfd1);
	return 0; 
}
