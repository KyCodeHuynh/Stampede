/* A simple web server using TCP for sockets.
   The port number is passed as an argument. 
   This version runs forever, forking off a separate 
   process for each connection.
   */
#include "simple_http.h"
#include "utility.h"

#include <netinet/in.h>  // Constants and structures needed for addresses, e.g. sockaddr_in
#include <signal.h>      // Signal name macros, and the kill() prototype
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>      // malloc(), calloc(), EXIT_FAILURE, etc.
#include <strings.h>
#include <sys/socket.h>  // Definitions of structures needed for sockets, e.g. sockaddr
#include <sys/types.h>   // Definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/wait.h>    // waitpid() system call
#include <unistd.h>      // Standard system calls


// Header will include them as an external constant. 
// See: https://stackoverflow.com/questions/5499504/shared-c-constants-in-a-header
// See: https://stackoverflow.com/questions/2328671/constant-variables-not-working-in-header
// const char HTTP_200_OK_RESPONSE[] 
// = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\n";
// const off_t HTTP_200_OK_RESPONSE_LENGTH = 46;

// '\n' are for if anyone wants to view the HTML source 
const char HTTP_404_NOT_FOUND_RESPONSE[]
= "HTTP/1.1 404 Not Found\r\nConnection: Close\r\nServer: Stampede/0.1\r\n\r\n<html>\n<head><title>Page Not Found</title></head>\n<body>This is not the page you are looking for.</body></html>";
const off_t HTTP_404_NOT_FOUND_RESPONSE_LENGTH = 191;

const char HTTP_500_SERVER_ERROR_RESPONSE[]
= "HTTP/1.1 500 Server Error\r\nConnection: Close\r\nServer: Stampede/0.1\r\n\r\n<html>\n<head><title>Server Error</title></head>\n<body>We're sorry, something has gone wrong.</body></html>";
const off_t HTTP_500_SERVER_ERROR_RESPONSE_LENGTH = 189;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        // Follow Bash conventions for insufficient arguments
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    // Create socket
    int listeningSocketFD = socket(AF_INET, SOCK_STREAM, 0);	
    if (listeningSocketFD < 0) {
        error("ERROR: opening socket");
    }

    // Always initialize to 0 bytes to avoid junk data
    struct sockaddr_in serverAddress;
    memset((char *) &serverAddress, 0, sizeof(serverAddress));	

    // Sanity checks on port number
    uint16_t portNum = atoi(argv[1]);
    fprintf(stderr, "Port number: %d [Line: %d]\n", portNum, __LINE__);

    if (portNum < 0 || UINT16_MAX < portNum) {
        error("ERROR: invalid port number (allowable range is 0-65535)");
    }

    // Fill in address info
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNum);

    if (bind(listeningSocketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR: on binding");
    }
    // 5 simultaneous connections at most 
    listen(listeningSocketFD, 5);

    // Accept connections
    struct sockaddr_in clientAddress;
    socklen_t clientSocketLength;
    int respondingSocketFD = accept(listeningSocketFD, (struct sockaddr *) &clientAddress, &clientSocketLength);
    if (respondingSocketFD < 0) {
        error("ERROR: on accept");
    }

    // Load the request into a buffer
    // Expand buffer size as needed. 
    char requestBuffer[REQUEST_BUFFER_SIZE];
    memset(requestBuffer, 0, REQUEST_BUFFER_SIZE); 
    http_verb_t verb;
    char* resourcePath = NULL;
    content_type_t type;

    // recv() returns the number of bytes actually read in
    // -1 on error, and 0 upon closed connection (which we
    // assume is not due to error [for now])
    int receiveResult = recv(respondingSocketFD, (char *)requestBuffer, REQUEST_BUFFER_SIZE, 0);
    if (receiveResult < 0) {
        error("ERROR: receiving request from client failed");
    }
    else if (receiveResult == 0) {
        // Other side closed the connection
        close(respondingSocketFD);
    }
    // We're gonna process the request! 
    else {
        // Null-terminate the request so tokenizing doesn't return junk
        requestBuffer[receiveResult + 1] = '\0';
        printf("Client request:\n %s\n", requestBuffer);

        // Respond to client with requested resource
        parse_request(requestBuffer, &verb, &resourcePath, &type);
        // fprintf(stderr, "DEBUG: Got past parse_request()\n");
        handle_request(verb, resourcePath, type, respondingSocketFD);
        // fprintf(stderr, "DEBUG: Got past handle_request()\n");
    }

    // int n = write(respondingSocketFD, "I got your message", 18);
    // if (n < 0) {
    //     error("ERROR: writing to socket");
    // }

    close(listeningSocketFD);

    return EXIT_SUCCESS; 
}

