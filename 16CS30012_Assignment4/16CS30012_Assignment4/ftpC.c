// Ishwarkar Rohan Shankar
// Roll :- 16CS30012
// Networks Lab Assignment 4
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


#define CONTROLPORT 50000 
#define MAXLINE 1024 
#define WORDSIZE 1024
#define BUFSIZE 80

// Function to get substring from array
char * getsub(char arr[],int start,int end){
	char *s = (char *)malloc(sizeof(char)*(end-start+1));
	int i=0;
	while(start<=end){
		s[i]=arr[start];
		start++;
		i++;
	}
	return s;
}

// Fucntion to check if two arrays are same
int issame(char s1[],char *s2){
	if(strcmp(s1,s2)==0)
		return 1;
	return 0;
}

// Main function
int main() 
{ 
	// Pipe to transfer data between processes
	int pipe1[2];
	if(pipe(pipe1)==-1){
        printf("Pipe Creation Failed!\n");
        return 1;
    }

    // Declaration of variables
	int sockfd1,sockfd2; 
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr;   
    int n, len;

    memset(&servaddr, 0, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(CONTROLPORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY;
    int errorcode;

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

	// Accept first command
	printf("\n");
	printf(">");
	scanf(" %[^\n]s",buffer);
	// Send command to server
	send(sockfd1, buffer, strlen(buffer) + 1, 0);
	// Receive errorcode
	recv(sockfd1,&errorcode,sizeof(int), 0);
	if(errorcode==503){
		// If another command is typed as first command
		printf("Error Code: %d\n",errorcode);
		printf("Port Command must be the first command!!\n");
		exit(0);
	}
	else if(errorcode==550){
		// If port number is out of bounds
		printf("Error Code: %d\n",errorcode);
		printf("Please enter a valid port no between 1024 and 65535.\n");
		exit(0);
	}
	else if(errorcode==200){ 
		// If successfull
		printf("Reply Code: %d\n",errorcode);
		printf("Port sent successfully to the server!!\n");   
	}


	int leng = (int)strlen(buffer);
	// Get portno
	int DATAPORT = atoi(getsub(buffer,5,leng-1));

	// Get commands
	while(1){

		printf(">");
		scanf(" %[^\n]s",buffer);
		leng = (int)strlen(buffer);
		
		// Implementation of cd function
		if(leng>=2&&issame("cd",getsub(buffer,0,1))){
			// Send command to server
			send(sockfd1, buffer, strlen(buffer) + 1, 0);
			// Receive error code
			recv(sockfd1,&errorcode,sizeof(int), 0);
			if(errorcode==501){
				printf("Error Code: %d\n",errorcode);
				printf("Directory not found on server!\n");
			}
			else{
				printf("Reply Code: %d\n",errorcode);
				printf("Directory successfully changed on server!\n");
			}
		}

		// Implementation fo quit fucntion
		else if(leng>=4&&issame("quit",getsub(buffer,0,3))){
			// Send command to server
			send(sockfd1, buffer, strlen(buffer) + 1, 0);
			// Receive error code
			recv(sockfd1,&errorcode,sizeof(int), 0);
			if(errorcode==421){
				printf("Reply Code: %d\n",errorcode);
				printf("Disconnected from server!\n");
				exit(0);
			}
		}

		// Implementation of get function
		else if(leng>=3&&issame("get",getsub(buffer,0,2))){
			char *line,ch[1];
			// Get file name
			line =getsub(buffer,4,leng-1);     
			// Write filename to pipe                    
        	write(pipe1[1], line, BUFSIZE); 
        	// Create  child process
        	int pid=fork();
        	if(pid==0){
        		// Socket creation
	        	if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
			        printf("Failed to create socket!\n"); 
			        exit(0); 
    			}
    			int option = 1;
				setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    			struct sockaddr_in cli_addr;
				cli_addr.sin_family = AF_INET; 
			    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
			    cli_addr.sin_port = htons(DATAPORT); 	  
			    // BInd socket
			    int b = bind(sockfd2, (struct sockaddr*)&cli_addr, sizeof(cli_addr)); 
			    if(b<0){
			        printf("Bind failed!\n");
			        exit(0);
			    }	
				close(pipe1[1]);					
		    	read(pipe1[0], line, BUFSIZE);		    	
		    	listen(sockfd2, 5);	
		    	socklen_t serv_len;		
		    	serv_len = sizeof(servaddr);
		    	// Accept connections from server
				sockfd2 = accept(sockfd2, (struct sockaddr *) &servaddr, &serv_len) ;
		    	int filehandle,p,u;
		    	// Create or overwrite file
				filehandle = open(line, O_CREAT | O_WRONLY |O_TRUNC, 0666);				
				// File receiving module
				while(1){
					// Receive character
					u = recv(sockfd2,ch,sizeof(ch),0);
					ch[u]='\0';
					// Receive number of bytes
					recv(sockfd2,&p,sizeof(int),0);
					p=ntohl(p);
					char filebuff[p];
					// Receive buffer
					p = recv(sockfd2,filebuff,sizeof(filebuff),0);
					filebuff[p]='\0';
					write(filehandle,filebuff,strlen(filebuff));
					// If last block break
					if(ch[0]=='L')
						break;
				}
				close(filehandle);
				exit(0);    			
			}   
			// Send command
			send(sockfd1, buffer, strlen(buffer) + 1, 0);
			// Receive errorcode
			recv(sockfd1,&errorcode,sizeof(int), 0);
			if(errorcode==250){
				printf("Reply Code: %d\n",errorcode);
				printf("File Transfer successfull!\n");
			}
			else{
				kill(pid, SIGKILL);
				printf("Error Code: %d\n",errorcode);
				printf("There was some problem transfering file(May be file doesn't exist)!\n");
			}
		}

		// Implementation of put function
		else if(leng>=3&&issame("put",getsub(buffer,0,2))){
			char *line;
			// Get file name
			line =getsub(buffer,4,leng-1);   
			// Write filename to pipe                      
        	write(pipe1[1], line, BUFSIZE);
        	int filehandle;
        	// Open file to read
			filehandle = open(line,O_RDONLY);
        	// If file exists on client
        	if(filehandle!=-1){
        		// Create child process
        		int pid=fork();
        		if(pid==0){
        			// Create socket
		        	if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
				        printf("Failed to create socket!\n"); 
				        exit(0); 
	    			}
	    			int option = 1;
					setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	    			struct sockaddr_in cli_addr;
					cli_addr.sin_family = AF_INET; 
				    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
				    cli_addr.sin_port = htons(DATAPORT); 	  
				    // Bind socket
				    int b = bind(sockfd2, (struct sockaddr*)&cli_addr, sizeof(cli_addr)); 
				    if(b<0){
				        printf("Bind failed!\n");
				        exit(0);
				    }	
					close(pipe1[1]);	
					// Read filename				
			    	read(pipe1[0], line, BUFSIZE);		    	
			    	listen(sockfd2, 5);	
			    	socklen_t serv_len;		
			    	serv_len = sizeof(servaddr);
			    	// Accept call from server
					sockfd2 = accept(sockfd2, (struct sockaddr *) &servaddr, &serv_len) ;
					char filebuff[10];
					int l;
					char ch[1];
					// File sending module
				    while((l=read(filehandle,filebuff,10))>0){
						filebuff[l]='\0';
						if(l==10)
							ch[0]='N';
						else
							ch[0]='L';
						int con=htonl(l);
						// Send character
						send(sockfd2,ch,sizeof(ch),0);
						// Send number of bytes
						send(sockfd2,&con,sizeof(con),0);
						// Send buffer
						send(sockfd2,filebuff,strlen(filebuff),0);
						if(l<10)
							break;
					}
					close(sockfd2);
			    	exit(0);		    	
    			}
    			// Send command
    			send(sockfd1, buffer, strlen(buffer) + 1, 0);
    			// Receive errorcode
				recv(sockfd1,&errorcode,sizeof(int), 0);
				if(errorcode==250){
					printf("Reply Code: %d\n",errorcode);
					printf("File Transfer successfull!\n");
				}
				else{
					kill(pid, SIGKILL);
					printf("Error Code: %d\n",errorcode);
					printf("There was some problem transfering file!\n");
				}
        	}
        	else
        		printf("File not found on client!\n");
        	
        	
						
		}		
		else{
			send(sockfd1, buffer, strlen(buffer) + 1, 0);
			recv(sockfd1,&errorcode,sizeof(int), 0);
			printf("Error Code: %d\n",errorcode);
			printf("Sorry, Command not supported!\n");
		}
	}
    
}