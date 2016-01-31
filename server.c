/* A simple web server using TCP for sockets.
   The port number is passed as an argument. 
   This version runs forever, forking off a separate 
   process for each connection.
   */
#include <unistd.h>      // Standard system calls
#include <sys/types.h>   // Definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // Definitions of structures needed for sockets, e.g. sockaddr
#include <sys/wait.h>    // waitpid() system call
#include <netinet/in.h>  // Constants and structures needed for addresses, e.g. sockaddr_in
#include <signal.h>      // Signal name macros, and the kill() prototype
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

void error(char *msg)
{
    // We use 1 as our exit convention as this 
    // is a standalone web server that may be scripted. 
    // Bash convention treats 1 as a general error. 
    // We would use -1 were this a library, as POSIX
    // convention uses < 0 for errors.
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        // Follow Bash conventions for insufficient arguments
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
    if (sockfd < 0) {
        error("ERROR: opening socket");
    }

    // Always initialize to 0 bytes to avoid junk data
    struct sockaddr_in serverAddress;
    memset((char *) &serverAddress, 0, sizeof(serverAddress));	

    // Sanity checks on port number
    int portNum = atoi(argv[1]);

    if (portNum < 0 || portNum < UINT16_MAX ) {
        error("ERROR: invalid port number (allowable range is 0-65535)");
    }

    // Fill in address info
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNum);

    if (bind(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR: on binding");
    }
    // 5 simultaneous connections at most 
    listen(sockfd, 5);

    // Accept connections
    struct sockaddr_in clientAddress;
    socklen_t clientSocketLength;
    int newsockfd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientSocketLength);

    if (newsockfd < 0) {
        error("ERROR: on accept");
    }


    // To read the client's request: 
    //
    // Will need to tokenize, validate, and parse

    // To construct a response: 
    //
    // Could just read first line of request: 
    //   GET /image.png HTTP/1.1
    // And return the correct resource. 
    //
    // Need to fork() in order for suspension while waiting
    // int pid;
    //
    // Could just attempt to locate resource with 
    // an fstat() etc., and return 404 if needed. 
    // But use send() instead of write() or fwrite()
    //
    // NOTE: Handling of nullbytes in files? write() 
    // is by default delimited by nullbytes. 
    char buffer[256];
    memset(buffer, 0, 256); 

    int n = read(newsockfd, buffer, 255);
    if (n < 0) {
        error("ERROR: reading from socket");
    }

    printf("Here is the message: %s\n",buffer);

    // reply to client
    // Use send() instead
    n = write(newsockfd, "I got your message", 18);
    if (n < 0) {
        error("ERROR: writing to socket");
    }


    close(newsockfd); //close connection 
    close(sockfd);

    return 0; 
}

