#include "simple_http.h"
#include "utility.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Uncomment this to disable all assertions
// #define NDEBUG

// Header will include them as an external constant. 
// See: https://stackoverflow.com/questions/5499504/shared-c-constants-in-a-header
// See: https://stackoverflow.com/questions/2328671/constant-variables-not-working-in-header
const char HTTP_200_OK_RESPONSE[] 
= "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\n";
const off_t HTTP_200_OK_RESPONSE_LENGTH = 46;

// '\n' are for if anyone wants to view the HTML source 
const char HTTP_404_NOT_FOUND_RESPONSE[]
= "HTTP/1.1 404 Not Found\r\nConnection: Close\r\nServer: Stampede/0.1\r\n\r\n<html>\n<head><title>Page Not Found</title></head>\n<body>This is not the page you are looking for.</body>\n</html>";
const off_t HTTP_404_NOT_FOUND_RESPONSE_LENGTH = 191;

const char HTTP_500_SERVER_ERROR_RESPONSE[]
= "HTTP/1.1 500 Server Error\r\nConnection: Close\r\nServer: Stampede/0.1\r\n\r\n<html>\n<head><title>Server Error</title></head>\n<body>We're sorry, something has gone wrong.</body>\n</html>";
const off_t HTTP_500_SERVER_ERROR_RESPONSE_LENGTH = 189;

// 'static' as this should be visible to 
// this file only. Global constant for if we add more tests.
static const char stdRequest[] = "GET /file.txt HTTP/1.1\r\n";
static const char stdRequestLength = 27;

int main() 
{
    char testBuffer[REQUEST_BUFFER_SIZE];
    memset(testBuffer, 0, REQUEST_BUFFER_SIZE);
    memcpy(testBuffer, stdRequest, stdRequestLength);

    http_verb_t verbResult;
    char* resourcePath = (char*)malloc(sizeof(char) * MAX_FILE_PATH_LENGTH);
    content_type_t typeResult;

    assert(0 == parse_request(testBuffer, &verbResult, &resourcePath, &typeResult));

    assert(verbResult == HTTP_GET);
    assert(strcmp(resourcePath, "file.txt") == 0);
    assert(typeResult == CONTENT_TEXT);

    // TODO: Fix malloc() error
    // free(resourcePath);

    return 0;
}