// Ishwarkar Rohan Shankar
// 16CS30012
// Networks Lab Assignment 2

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <unistd.h>



// Defining server bufferfer lengths
// You can change the respective buffersizes from here
#define filenamelength 100
#define serverbufferfersize 10


// Main function
int main()
{
	int	sockfd, newsockfd,f; 
	socklen_t clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	
	int i;
	char buffer[serverbufferfersize+1];	
	char filename[filenamelength+1];
	
	// Creating sockets
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
	
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port			= htons(20000);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); 

	while (1) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen) ;

		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0); 
		}
		
		// Receive name from client
		recv(newsockfd, filename, filenamelength, 0);
		printf("\nFile name received from client: %s\n", filename);
		
		
		// Try opening the file and return 
		int l,fd;
		fd=open(filename,O_RDONLY);		
		if (fd < 0) { perror("r1"); close(newsockfd); } 		
		
		// Reading from file 
		while((l=read(fd,buffer,serverbufferfersize))>0){
			buffer[l]='\0';
			printf("%s",buffer);
			// Send to client
			send(newsockfd,buffer,strlen(buffer),0);
		}
		close(newsockfd);
	}
}
			

