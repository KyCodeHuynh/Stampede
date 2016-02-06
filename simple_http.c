#include "simple_http.h"
#include "utility.h"

#include <fcntl.h>    // open()
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> // stat()
#include <unistd.h>   // System calls like read() and write()

// See Section 2.2 of Computer Networking (Kurose and Ross, 6th Edition)
// for a summary of HTTP, its use and format, etc.
int 
parse_request(char* requestBuffer, 
              http_verb_t* verb, 
              char** resourcePath)
{
    // First line is a special case 
    bool isFirstLine = true;

    // TODO: Protect against null-byte poisoning
    // We fill curLine right away to avoid prematurely tripping the NULL check
    char* curLine = strtok(requestBuffer, "\r\n");
    char* curWord = strtok(curLine, " ");
    for (;;) {
        // fprintf(stderr, "DEBUG: Current word: %s [Line %d]\n", curWord, __LINE__);
        if (curWord == NULL) {
            // fprintf(stderr, "DEBUG: Breaking out of parsing inner loop [Line %d]\n", __LINE__);
            break;
        }

        if (isFirstLine) {
            if (strcmp(curWord, "GET") == 0) {
                // fprintf(stderr, "DEBUG: Got a GET request [Line %d]\n", __LINE__);
                *verb = HTTP_GET;
            }
            // TODO: No other verbs are supported at present
            else {
                return -1;
            }

            // Second token should be /path/to/resource, so we need to strip
            // the leading slash, for which we can just increment the pointer.
            // strtok() expects a NULL upon subsequent calls to advance
            // forward internally to the next token.
            curWord = strtok(NULL, " ");
            *resourcePath = curWord + 1;

            isFirstLine = false;
            break;
        }
        // The rest is header parsing. Request won't have a body
        // TODO: Handle headers. Would need outer loop for multiple lines.
        // TODO: Basic HTTP format validation 
    }

    return 0;
}

// TODO: Send response based on status code, file contents, socket
int 
send_response(http_status_code_t status, 
              const char* resourceBuffer, 
              const off_t resourceLength, 
              int respondingSocket)
{
    // TODO: Use send() 
    // TODO: Append Encoding, Content-Type, Content-Length to response
    return 0;
}


// TODO: Test this separately
http_status_code_t 
handle_request(http_verb_t verb, 
               char* resourcePath, 
               int respondingSocket)
{
    // Load the file into a buffer for sending back to the client
    if (verb == HTTP_GET) {
        // Note that "f"-functions like fopen(), fstat(), etc. are standard only to POSIX, not C
        int fileDescrip = open((const char *) resourcePath, O_RDONLY);
        if (fileDescrip < 0) {
            send_response(HTTP_404_NOT_FOUND, 
                          HTTP_404_NOT_FOUND_RESPONSE, 
                          HTTP_404_NOT_FOUND_RESPONSE_LENGTH, 
                          respondingSocket);

            return HTTP_404_NOT_FOUND;
        }

        // CERT recommends against fseek() and ftell() for determining file size
        // See: https://is.gd/mwJDph-
        struct stat fileInfo;

        // State failed for some reason
        if (stat((const char *) resourcePath, &fileInfo) < 0) {
            send_response(HTTP_500_SERVER_ERROR, 
                          HTTP_500_SERVER_ERROR_RESPONSE, 
                          HTTP_500_SERVER_ERROR_RESPONSE_LENGTH, 
                          respondingSocket);

            return HTTP_500_SERVER_ERROR;
        }   
        // Not a regular file
        if (! S_ISREG(fileInfo.st_mode)) {
            send_response(HTTP_500_SERVER_ERROR, 
                          HTTP_500_SERVER_ERROR_RESPONSE, 
                          HTTP_500_SERVER_ERROR_RESPONSE_LENGTH, 
                          respondingSocket);

            return HTTP_500_SERVER_ERROR;        
        }

        // Read file into buffer
        off_t fileSize = fileInfo.st_size;



        // TODO: Use send_response()
        return HTTP_200_OK;
    }

    return HTTP_500_SERVER_ERROR;
}
