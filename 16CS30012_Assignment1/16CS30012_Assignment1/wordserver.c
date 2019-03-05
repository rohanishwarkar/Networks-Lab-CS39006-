// Server side code

// Importing server side code
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#define MAXLINE 1024 

// Main function
int main() { 

	int sockfd; 
	struct sockaddr_in servaddr, cliaddr; 

	// Create socket file descriptor 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sockfd < 0 ) { 
		perror("Socket Creation Failed!"); 
		exit(EXIT_FAILURE); 
	} 

	// Initiallysetting to zero
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 

	// Putting the server details
	servaddr.sin_family    = AF_INET; 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(8181); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 ) { 
		perror("Bind Failed"); 
		exit(EXIT_FAILURE); 
	} 

	printf("\nServer Running....\n");

	int n; 
	socklen_t len;

	// Receive file name from client
	char buffer[MAXLINE]; 
	len = sizeof(cliaddr);
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,( struct sockaddr *) &cliaddr, &len); 
	buffer[n] = '\0'; 
	printf("Filename recieved from client: %s\n",buffer);

	FILE *ptr;
	ptr=fopen(buffer,"r");

	// When file does not exist
	if(ptr==NULL){
		printf("File not found!\n");	
		char notf[100]="NOTFOUND";
		sendto(sockfd, (const char *)notf, strlen(notf), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
	}

	// When file with specified name exists
	else{
		printf("File found!\n");
		char ww[100];
		// Get first word from file ie. HELLO and send it to client
		fscanf(ptr,"%s",ww);
		printf("Read word: %s. Sending it to client!\n",ww);
		sendto(sockfd, (const char *)ww, strlen(ww), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
		while(fscanf(ptr,"%s",ww)==1){
			// Till we reach end of file
			// Get response of client
			char received[MAXLINE]; 
			len = sizeof(cliaddr);
			n = recvfrom(sockfd, (char *)received, MAXLINE, 0,( struct sockaddr *) &cliaddr, &len); 
			received[n] = '\0';
			// Send the word from file
			sendto(sockfd, (const char *)ww, strlen(ww), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
			printf("Read word: %s. Sending it to client!\n",ww);
		}
		fclose(ptr);
	}

	return 0; 
}
