#include "simple_http.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO: Test this separately in a simple_http_test.c
// See Section 2.2 of Computer Networking (Kurose and Ross, 6th Edition)
// for a summary of HTTP, its use and format, etc.
int parse_request(char* requestBuffer, http_verb_t* verb, char** resourcePath)
{
    // First line is a special case 
    bool isFirstLine = true;

    // Tokenize the request by lines, then by words within lines.
    // We fill curLinen right away to avoid prematurely tripping the NULL check
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

            // Second token should be /path/to/resource, 
            // so we need to strip the leading slash,
            // for which we can just increment the pointer.
            // strtok() expects a NULL upon subsequent calls
            // to advance forward internally to the next token.
            curWord = strtok(NULL, " ");
            // Pointer-to-pointer so that it is modifiable
            // despite being passed into a function
            *resourcePath = curWord + 1;

            isFirstLine = false;
            break;
        }
        // The rest is header parsing
        // Request won't have a body
        // TODO: Handle headers. Would need outer loop for multiple lines.
        // TODO: Basic HTTP format validation 
    }

    return 0;
}


// TODO: Test this separately
http_status_code_t handle_request(http_verb_t verb, char* resourcePath, int respondingSocket)
{
    // Could just attempt to locate resource with 
    // an fstat(), fopen() etc., and return 404 if needed. 

    // TODO: open() vs fopen()
    // open() supports giving the full path

    // TODO: Protect against null-byte poisoning

    return HTTP_200_OK;
}
