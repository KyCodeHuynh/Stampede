/*
 A simple client in the internet domain using TCP
 Usage: ./client hostname port (./client 192.168.0.151 10000)
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // define structures like hostent
#include <stdlib.h>
#include <strings.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    // Socket descriptor 
    int sockfd;
    int portno;
    int n;
    struct sockaddr_in serv_addr;
    // Contains tons of information, including the server's IP address 
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr, "usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    portno = atoi(argv[2]);
    // Create a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // Takes a string like "www.yahoo.com", and returns a struct hostent which
    // contains information like IP address, address type, the length of the
    // addresses, etc.
    server = gethostbyname(argv[1]); 

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    // Initialize server's address
    serv_addr.sin_family = AF_INET; 
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    //establish a connection to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    
    printf("Please enter the message: ");
    memset(buffer, 0, 256);
    // Read message
    fgets(buffer, 255, stdin);	
    
    // Write to the socket
    n = write(sockfd, buffer, strlen(buffer)); 

    if (n < 0) {
         error("ERROR writing to socket");
    }
    
    memset(buffer, 0, 256);

    // Read from the socket
    n = read(sockfd,buffer,255); 
    if (n < 0) {
         error("ERROR reading from socket");
    }

    // Print server's response
    printf("%s\n", buffer);	
    
    // Close the socket
    close(sockfd);
    
    return 0;
}
