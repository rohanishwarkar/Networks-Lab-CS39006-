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



// You can change the respective bufferfersizes from here
#define filenamelength 100
#define clientbufferfersize 5


// Function to check whether a character is delimiter 
// Assumed \n as delimiter in this
int delim(char c){
	if(c==','||c=='.'||c==':'||c==';'||c==' '||c=='\n')
		return 1;
	return 0;
}

// Main function
int main()
{
	// Variable
	int	sockfd ;
	struct sockaddr_in	serv_addr;
	
	
	int i;
	char buffer[clientbufferfersize],filename[filenamelength+1];
	
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}
	
	// Specifications
	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port		= htons(20000);
 
 
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
	
	// Taking file name as input
	printf("Enter file name : ");
	scanf("%s",filename);
	
	printf("\nFile name sent to server: %s\n",filename);
	send(sockfd, filename, strlen(filename) + 1, 0);
	

	// Open file to write received data
	int fd;
	fd=open(filename,O_RDWR|O_CREAT,0640);		
	if (fd < 0) { perror("r1");} 
	
	
	int size1;
	int state=0,noofbytes=0,count=0,found=0;

	for(i=0; i < clientbufferfersize; i++) buffer[i] = '\0';
	
	// Receive data from server
	while((size1=recv(sockfd,buffer,sizeof(buffer),0))>0){
		buffer[size1]='\0';
		// printf("%s",buffer);
		// Write data to file
		write(fd,buffer,strlen(buffer));
		int k=0;
		// Counting number of words
		while(buffer[k]!='\0'){
			noofbytes++;
			if(delim(buffer[k])){
				found=0;
			}
			else if(!found&&!delim(buffer[k])){
				count++;
				found=1;
			}
			k++;
		}
		for(i=0; i < clientbufferfersize; i++) buffer[i] = '\0';
	}
	close(fd);
	// Printing the final values
	if(noofbytes==0)
		printf("File not found on server. Connection closed!\n");
	else{
		printf("\nThe file transfer is successful.\n");
		printf("No of words assuming \\n as delimiter: %d\n",count);
		printf("No of bytes read: %d\n",noofbytes);
		printf("Data written to file!!\n");
	}
	close(sockfd);
}

