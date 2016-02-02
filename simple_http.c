#include "simple_http.h"
#include <string.h>

int parse_request(char* requestBuffer, http_verb_t verb, char** resourcePath)
{
    // Tokenize the request by lines, then by words within lines.
    // Need to populate curLine right away, or else nullity check will trigger prematurely
    char* curLine = strtok(requestBuffer, "\r\n");
    for (;;) {
        if (curLine == NULL) {
            break;
        }
        else {
            char* curWord;
            for (;;) {
                curWord = strtok(curLine, " ");
                    // TODO: Parse filename
                    // TODO: Special isFirstLine case? 
                    // The rest would be header parsing

                    // fopen() supports giving the full path
                    // "GET /path/to/resource.html HTTP/1.1" 
                    // => "GET", "/path/to/resource.html", "HTTP/1.1"

                    // First line is special. It's space-delimited, 
                    // but remaining header lines are colon-delimited, 
                    // with \r\n (CRLF) separating headers. 
                    // 
                    // "Header-Name: Value"

                    // Request won't have a body
                    // TODO: Basic HTTP format validation 
            }

                // Placement of this matters, or else
                // we'd proceed to the next line too soon.
            curLine = strtok(requestBuffer, "\r\n");
        }

    } 
}


http_status_code_t handle_request(http_verb_t verb, char* resourcePath, int respondingSocket)
{
    // Could just attempt to locate resource with 
    // an fstat(), fopen() etc., and return 404 if needed. 
    // But use send() instead of write() or fwrite()
}
