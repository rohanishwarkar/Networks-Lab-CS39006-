// Client Side Code

// Importing header files
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
	struct sockaddr_in servaddr; 

	// Creating socket file descriptor 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sockfd < 0 ) { 
		perror("Socket Creation Failed!"); 
		exit(EXIT_FAILURE); 
	} 

	// Reset to 0 initially
	memset(&servaddr, 0, sizeof(servaddr)); 

	// Server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(8181); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 	

	int n;
	socklen_t len; 

	// Getting input from user and sending to server
	printf("Enter filename(ex. sampletext(uploaded sample file): ");
	char filename[100]; 
	scanf("%s",filename);  
	sendto(sockfd, (const char *)filename, strlen(filename), 0,(const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	printf("File name sent to server! \n"); 
	
	// Receive feedback from server regarding filename
	char buffer[MAXLINE];
	len = sizeof(servaddr);
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,( struct sockaddr *) &servaddr, &len); 
	buffer[n] = '\0';  

	// if message is  "NOTFOUND" then exit
	if(buffer[0]=='N'){
		printf("File not found!\n");
		exit(EXIT_FAILURE);
	}
	// File exists so we receive 
	else{
		printf("%s\n",buffer);
		char newfilename[100];
		printf("This program will automatically create a file and write the received words to it.\nEnter filename to store words: ");
		scanf("%s",newfilename);
		// Opening new file to write the words received from server
		FILE *fp;
		fp=fopen(newfilename,"w");

		while(1){
			// Send WORD1, WORD2 ...
			printf("Enter word no. Ex(WORD1,WORD2): ");
			char words[100];
			scanf("%s",words);
			sendto(sockfd, (const char *)words, strlen(words), 0,(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

			// Receive the words from server
			char received[MAXLINE];
			len = sizeof(servaddr);
			n = recvfrom(sockfd, (char *)received, MAXLINE, 0,( struct sockaddr *) &servaddr, &len); 
			received[n] = '\0';

			// If received word is END stop the program
			if(received[0]=='E'&&received[1]=='N'&&received[2]=='D')
				exit(EXIT_FAILURE);
			printf("Received word: %s. Written to file!!\n",received);
			fprintf(fp,"%s\n",received);  // Write the received word to file
			printf("\n");
			
		}
		fclose(fp);
	}
	close(sockfd); 
	return 0; 
}
