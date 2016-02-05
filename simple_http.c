#include "simple_http.h"
#include <string.h>
#include <stdbool.h>

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
    for (;;) {
        // End of request
        if (curLine == NULL) {
            break;
        }
        else {
            char* curWord;
            for (;;) {
                curWord = strtok(curLine, " ");
                if (isFirstLine) {
                    if (strcmp(curWord, "GET") == 0) {
                        *verb = HTTP_GET;
                    }
                    // TODO: No other verbs are supported at present
                    else {
                        return -1;
                    }

                    // Second token should be /path/to/resource, 
                    // so we need to strip the leading slash,
                    // for which we can just increment the pointer
                    curWord = strtok(curLine, " ");
                    // Pointer-to-pointer so that it is modifiable
                    // despite being passed into a function
                    *resourcePath = curWord + 1;

                    isFirstLine = false;
                }
                // The rest is header parsing
                // Request won't have a body
                // TODO: Handle headers
                // TODO: Basic HTTP format validation 
            }

            // Placement of this matters, or else
            // we'd proceed to the next line too soon.
            curLine = strtok(requestBuffer, "\r\n");
        }
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
