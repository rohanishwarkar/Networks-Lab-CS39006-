// Ishwarkar Rohan Shankar
// Roll :- 16CS30012
// Networks Lab Assignment 4


// Header Files
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
#include <sys/wait.h>



#define CONTROLPORT 50000 
#define MAXLINE 1024 
#define WORDSIZE 1024


// Function to get substring from char array from start and end points
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

// Function to check if two char arrays are same
int issame(char s1[],char *s2){
	if(strcmp(s1,s2)==0)
		return 1;
	return 0;
}

// Main function
int main() 
{ 
    // Declaration of variables
    int tcpfd; 
    int DATAPORT;
    char buffer[MAXLINE]; 

    socklen_t len;
    socklen_t clilen;

    fd_set rs;      
    struct sockaddr_in cli_addr, serv_addr; 

    
    // Socket Creation
    tcpfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(tcpfd<0){
        printf("Socket creation failed!\n");
        exit(0);
    }

    int option = 1;
	setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    memset(&serv_addr,0,sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(CONTROLPORT); 
  
    // Binding
    int b = bind(tcpfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(b<0){
        printf("Bind failed!\n");
        exit(0);
    }

    listen(tcpfd, 5); 
    int errorcode;

	while (1) {
		clilen = sizeof(cli_addr);
		int newsockfd = accept(tcpfd, (struct sockaddr *) &cli_addr, &clilen) ;

		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0); 
		}
		// Receive first command which is port
		recv(newsockfd, buffer, MAXLINE, 0);
		int leng = (int)strlen(buffer);

		errorcode=503;
		if(leng>4){
			char *temp=getsub(buffer,0,3);
			if(issame("port",temp)){
				int portno = atoi(getsub(buffer,5,leng-1));
				// If port number out of bounds
				if(portno>=1024&&portno<=65535){
					DATAPORT=portno;
					// Correct port no
					errorcode=200;
				}
				else
					errorcode=550;				
			}
		}
		// Send error code
		send(newsockfd, &errorcode, sizeof(errorcode), 0);
		// If port number is correct
		if(errorcode==200){
			while(1){		
				// Receive next command as long as client is sending		
				recv(newsockfd, buffer, MAXLINE, 0);
				leng = (int)strlen(buffer);

				// Implementation of cd function
				if(issame("cd",getsub(buffer,0,1))){
					errorcode=200;
					// Change directory on server
					int ret = chdir(getsub(buffer,3,leng-1));
					if(ret!=0)
						errorcode=501;
					// Send errorcode
					send(newsockfd, &errorcode, sizeof(errorcode), 0);
				}

				// Implementation of quit function
				else if(issame("quit",getsub(buffer,0,3))){
					errorcode=421;
					// Send errorcode
					send(newsockfd, &errorcode, sizeof(errorcode), 0);
					// Close connection
					close(newsockfd);
					break;

				}

				// Implementation of get function
				else if(issame("get",getsub(buffer,0,2))){
					errorcode=250;
					int filehandle;
					// Open file
					filehandle = open(getsub(buffer,4,leng-1),O_RDONLY);
					int l;
					// Check if file exists
					if(filehandle==-1){
						errorcode=550;
					}	
					// If file exists				
					if(filehandle!=-1){
						int pid=fork();
						// Fork child process to send file
						if(pid==0){
							// Create new socket
							int tcpfd2 = socket(AF_INET, SOCK_STREAM, 0); 
						    if(tcpfd2<0){
						        printf("Socket creation failed!\n");
						        exit(0);
						    }
						    int option2 = 1;
							setsockopt(tcpfd2, SOL_SOCKET, SO_REUSEADDR, &option2, sizeof(option2));
						    memset(&cli_addr,0,sizeof(cli_addr));
						    cli_addr.sin_family = AF_INET; 
						    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
						    cli_addr.sin_port = htons(DATAPORT);  
						    // Connect to client process CD which is waiting
							if (connect(tcpfd2,(struct sockaddr*)&cli_addr,sizeof(cli_addr)) < 0) { 
						        printf("Connection Failed! \n");
						        exit(0);
						    }
						    char filebuff[10],modbuff[13],ch[1];
						    for(int i=0;i<13;i++)modbuff[i]='\0';
						    // File sending module
						    while((l=read(filehandle,filebuff,10))>0){
								filebuff[l]='\0';
								if(l==10)
									ch[0]='N';
								else
									ch[0]='L';
								int con=htonl(l);
								// Send character
								send(tcpfd2,ch,sizeof(ch),0);
								// Send bytes read
								send(tcpfd2,&con,sizeof(con),0);
								// Send filebuffer
								send(tcpfd2,filebuff,strlen(filebuff),0);
								// If file is completely read break
								if(l<10)
									break;
							}
						    exit(0);
						}
						// Wait for child process to complete
						wait(NULL);
						
					}
					// Send error code after child process is complete
					send(newsockfd, &errorcode, sizeof(errorcode), 0);
				}

				// Implementation of put function
				else if(issame("put",getsub(buffer,0,2))){
					errorcode=250;
					int pid=fork();
					// Fork a child process
					if(pid==0){
						// Socket creation
						int tcpfd2 = socket(AF_INET, SOCK_STREAM, 0); 
					    if(tcpfd2<0){
					        printf("Socket creation failed!\n");
					        exit(0);
					    }
					    int option2 = 1;
						setsockopt(tcpfd2, SOL_SOCKET, SO_REUSEADDR, &option2, sizeof(option2));
					    memset(&cli_addr,0,sizeof(cli_addr));
					    cli_addr.sin_family = AF_INET; 
					    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
					    cli_addr.sin_port = htons(DATAPORT);
					    // Connect to child process which is waiting  
						if (connect(tcpfd2,(struct sockaddr*)&cli_addr,sizeof(cli_addr)) < 0) { 
					        exit(0);
					    }
				    	int filehandle;
				    	char *line=getsub(buffer,4,leng-1);
				    	// Create or overwrite file
						filehandle = open(line, O_CREAT | O_WRONLY |O_TRUNC, 0666);
						char filebuff[5];
						int size1=0;
						// File receiving module
						while(1){
							char ch[1];
							// Receive character
							int u = recv(tcpfd2,ch,sizeof(ch),0);
							ch[u]='\0';
							int p;
							// Receive number of bytes
							recv(tcpfd2,&p,sizeof(int),0);
							p=ntohl(p);
							char filebuff[p];
							// Receive filebuffer
							p = recv(tcpfd2,filebuff,sizeof(filebuff),0);
							filebuff[p]='\0';
							// Write data to file
							write(filehandle,filebuff,strlen(filebuff));
							if(ch[0]=='L')
								break;
						}
						close(filehandle);
						close(tcpfd2);
					    exit(0);

					}
					// Wait for child process to complete
					wait(NULL);
					// Send errorcode after child process is complete
					send(newsockfd, &errorcode, sizeof(errorcode), 0);
				}	
				else{
					errorcode=502;
					send(newsockfd, &errorcode, sizeof(errorcode), 0);

				}			
			}
		}
		else
			close(newsockfd);
		
	}
	return 0;	
} 