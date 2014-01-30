#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

int send_to_server(char *server_name, int portno, char *send_buffer, char *receive_buffer, int max_buf) {
	int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	int n;
	
	// create a socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Client cannot create socket");
		return -1;
	}
	
	if ((server = gethostbyname(server_name)) == NULL) {
		perror("Client cannot gethostbyname");
		return -1;
	}
	
	// create server addr
	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
	
	// connect to the server 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)  {
        perror("Client: ERROR connecting");
        exit(1);
    }
	
	if (write(sockfd, send_buffer, strlen(send_buffer)) < 0) {
		printf("Client: error writing to server\n");
		return -1;
	}
	
	*receive_buffer = '\0';
	n = read(sockfd, receive_buffer, max_buf-1);
	if (n >= 0) receive_buffer[n] = '\0';
	close(sockfd);
	return n;
}
