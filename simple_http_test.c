#include "simple_http.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Uncomment this to disable all assertions
// #define NDEBUG

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

    assert(0 == parse_request(testBuffer, &verbResult, &resourcePath));
    assert(verbResult == HTTP_GET);
    assert(strcmp(resourcePath, "file.txt") == 0);

    return 0;
}