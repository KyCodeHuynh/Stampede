#include "simple_http.h"
#include "utility.h"

#include <fcntl.h>    // open()
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h> // stat()
#include <unistd.h>   // System calls like read() and write()

// See Section 2.2 of Computer Networking (Kurose and Ross, 6th Edition)
// for a summary of HTTP, its use and format, etc.
int 
parse_request(char* requestBuffer, 
              http_verb_t* verb, 
              char** resourcePath, 
              content_type_t* type)
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
            // fprintf(stderr, "DEBUG: Path to resource: %s\n", *resourcePath);

            // Get the file extension: ".jpeg" -> "jpeg"
            char* fileExt = strrchr((const char *) *resourcePath, '.');
            // No dot was found, so assume of text type
            if (fileExt == NULL) {
                *type = CONTENT_TEXT;
            }

            // fprintf(stderr, "DEBUG: File extension: %s [Line: %d]\n", fileExt, __LINE__);
            // fprintf(stderr, "DEBUG: Got past the strrchr() [Line: %d]\n", __LINE__);
            fileExt += 1;
            // fprintf(stderr, "DEBUG: File extension without dot: %s [Line %d]\n", fileExt, __LINE__);
            // str_tolower(fileExt);

            if (strcmp(fileExt, "gif") == 0) {
                *type = CONTENT_GIF;
            }
            else if (strcmp(fileExt, "html") == 0) {
                *type = CONTENT_HTML;
            }
            else if (strcmp(fileExt, "jpeg") == 0
                  || strcmp(fileExt, "jpg") == 0) {
                *type = CONTENT_JPEG;
            }
            else if (strcmp(fileExt, "txt") == 0) {
                *type = CONTENT_TEXT;
            }

            // fprintf(stderr, "DEBUG: Got past the fileExt type-matching [Line: %d]\n", __LINE__);

            isFirstLine = false;
            break;
        }
        // The rest is header parsing. Request won't have a body
        // TODO: Basic HTTP format validation.
    }

    return 0;
}


// TODO: Test this separately
int
handle_request(http_verb_t verb, 
               char* resourcePath, 
               content_type_t type,
               int respondingSocket)
{
    // Load the file into a buffer for sending back to the client
    if (verb == HTTP_GET) {
        // Special case: we don't have anything at root '/'
        if (strcmp(resourcePath, "") == 0) {
            send_response(HTTP_404_NOT_FOUND, 
                          HTTP_404_NOT_FOUND_RESPONSE, 
                          HTTP_404_NOT_FOUND_RESPONSE_LENGTH, 
                          type,
                          respondingSocket);

            return 0;
        }

        // Note that "f"-functions like fopen(), fstat(), etc. are standard only to POSIX, not C
        int fileDescrip = open((const char *) resourcePath, O_RDONLY);
        if (fileDescrip < 0) {
            send_response(HTTP_404_NOT_FOUND, 
                          HTTP_404_NOT_FOUND_RESPONSE, 
                          HTTP_404_NOT_FOUND_RESPONSE_LENGTH, 
                          type,
                          respondingSocket);

            return 0;
        }

        // CERT recommends against fseek() and ftell() for determining file size
        // See: https://is.gd/mwJDph-
        struct stat fileInfo;

        // State failed for some reason
        if (stat((const char *) resourcePath, &fileInfo) < 0) {
            send_response(HTTP_500_SERVER_ERROR, 
                          HTTP_500_SERVER_ERROR_RESPONSE, 
                          HTTP_500_SERVER_ERROR_RESPONSE_LENGTH, 
                          type,
                          respondingSocket);

            return 0;
        }   
        // Not a regular file
        if (! S_ISREG(fileInfo.st_mode)) {
            send_response(HTTP_500_SERVER_ERROR, 
                          HTTP_500_SERVER_ERROR_RESPONSE, 
                          HTTP_500_SERVER_ERROR_RESPONSE_LENGTH, 
                          type,
                          respondingSocket);

            return HTTP_500_SERVER_ERROR;        
        }

        // Read file into buffer
        off_t fileSize = fileInfo.st_size;
        char* resourceBuffer = (char *)malloc(sizeof(char) * fileSize);
        if (read(fileDescrip, resourceBuffer, fileSize) < 0) {
            return -1;
        }

        // Send back the actual response 
        int result = send_response(HTTP_200_OK, resourceBuffer, fileSize, type, respondingSocket);
        if (result < 0) {
            return -1;
        }

        free(resourceBuffer);

        return 0;
    }

    return -1;
}


int 
send_response(http_status_code_t status, 
              const char* resourceBuffer, 
              const off_t resourceLength, 
              content_type_t type,
              int respondingSocket)
{
    switch (status) {
        // I prefer the visual blocking of braces-everywhere
        case HTTP_200_OK: {
            send(respondingSocket, "HTTP/1.1 200 OK\r\nConnection: close\r\n", 40, 0);
            // send(respondingSocket, "Server: Stampede/0.1\r\n", 24, 0);

            char numericalLength[100];
            sprintf(numericalLength, "Content-Length: %d\r\n", (int)resourceLength);
            int numBytes = strlen(numericalLength);
            send(respondingSocket, numericalLength, numBytes, 0);

            switch (type) {
                case CONTENT_GIF: {
                    send(respondingSocket, "Content-Type: image/gif\r\n", 23, 0);
                    break;
                }
                case CONTENT_HTML: {
                    send(respondingSocket, "Content-Type: text/html\r\n", 27, 0);
                    break;
                }
                case CONTENT_JPEG: {
                    send(respondingSocket, "Content-Type: text/jpeg\r\n", 27, 0);
                    break;
                }
                case CONTENT_TEXT: {
                    send(respondingSocket, "Content-Type: text/plain\r\n", 28, 0);
                    break;
                }
                default: {
                    send(respondingSocket, "Content-Type: text/plain\r\n", 28, 0);
                }
            }
            send(respondingSocket, "\r\n", 2, 0);
            send(respondingSocket, resourceBuffer, resourceLength, 0);

            return 0;
        }

        case HTTP_400_BAD_REQUEST: {
            // TODO: Implement handling
            return 0;
        }

        case HTTP_403_UNAUTHORIZED: {
            // TODO: Implement handling
            return 0;
        }

        case HTTP_404_NOT_FOUND: {
            // int send(int sockfd, const void *msg, int len, int flags);
            send(respondingSocket, resourceBuffer, resourceLength, 0);
            return 0;
        }

        case HTTP_500_SERVER_ERROR: {
            send(respondingSocket, resourceBuffer, resourceLength, 0);
            return 0;
        }
    }

    return -1;
}
