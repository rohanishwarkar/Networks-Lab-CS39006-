// Ishwarkar Rohan Shankar
// Roll :- 16CS30012
// Networks Lab Assignment 5
// Server side implementation


// Header Files
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <sys/stat.h>
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




#define PORT 		50000 
#define MAXLINE 	1024 
#define BLOCKSIZE 	10


// Main function
int main() 
{ 
    // Declaration of variables
    int tcpfd,filesize,l; 
    char buffer[MAXLINE],filename[MAXLINE]; 

    socklen_t len;
    socklen_t clilen;
     
    struct sockaddr_in cli_addr, serv_addr; 
	struct stat st;

    
    // Socket Creation
    tcpfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(tcpfd<0){
        printf("Socket creation failed!\n");
        exit(0);
    }

    int option = 1;
	setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	// Set to zero initially
    memset(&serv_addr,0,sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(PORT); 
  
    // Binding
    int b = bind(tcpfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(b<0){
        printf("Bind failed!\n");
        exit(0);
    }

    listen(tcpfd, 5); 

	while (1) {
		clilen = sizeof(cli_addr);
		int newsockfd = accept(tcpfd, (struct sockaddr *) &cli_addr, &clilen) ;

		// Accept new connection
		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0); 
		}	
		// Receive filename
		l = recv(newsockfd, filename, MAXLINE, 0);
		// Open file if exists
		int fp = open(filename, O_RDONLY);
		// If file does not exists
		if(fp<0){
			// Send letter
			strcpy(buffer,"E");
			send(newsockfd, buffer, sizeof(buffer) , 0);
		}
		else{
			// Send letter
			strcpy(buffer,"L");
			send(newsockfd, buffer, 1 , 0);
			// Find size of file
			stat(filename, &st);
			filesize = st.st_size;			
			// Send filesize
			send(newsockfd,&filesize,sizeof(filesize),0);
			int no_of_blocks=0;
			// Send file
			while( (l = read(fp,buffer,BLOCKSIZE))>0){
				no_of_blocks++;
				send(newsockfd, buffer, l, 0);
			}
			int complete_blocks = filesize/BLOCKSIZE,last_block_size = filesize%BLOCKSIZE;
			// Printing the calculated things
			printf("File size is: %d\n",filesize);
			printf("Blocksize is: %d\n",BLOCKSIZE);
			printf("No of blocks is: %d\n",no_of_blocks);
			if(last_block_size==0&&filesize!=0)
				printf("Last block size is: %d\n\n",BLOCKSIZE);
			else
				printf("Last block size is: %d\n\n",last_block_size);
		}
		// Close file pointer
		close(fp);
		// Close connection
		close(newsockfd);	
	}
	return 0;	
} 
