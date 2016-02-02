#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#define REQUEST_BUFFER_SIZE 4096

// Specified by client as part of request
typedef enum http_verb_t {
    HTTP_GET, 
    HTTP_POST, 
    HTTP_PUT, 
    HTTP_DELETE, 
    HTTP_HEAD
} http_verb_t;

// Returned by server to client
typedef enum http_status_code_t {
    HTTP_200_OK,
    HTTP_404_NOT_FOUND, 
    HTTP_400_BAD_REQUEST,
    HTTP_403_UNAUTHORIZED,
    HTTP_500_SERVER_ERROR
} http_status_code_t;

// Parses a standard HTTP request
// Inputs: 
    // A requestBuffer filled with the HTTP request, with all CRLFs preserved
// Outputs: 
    // A resourcePath like "/path/to/file.html" that was requested
    // The type of request, given as a a http_verb_t value
    // TODO: A pointer to an array of header/setting key-value pairs that must be freed later
    // TODO: Define some sort of struct for header/setting pairs
// Returns: 
    // 0 if successful 
    // -1 if invalid request
int parse_request(char* requestBuffer, http_verb_t verb, char** resourcePath);

// Responds to a standard HTTP request
// Inputs: 
    // The HTTP verb
    // The resourcePath (note the differing type; now just need to read it)
    // The socket on which to respond
// Outputs: 
    // None
// Returns: 
    // An HTTP status code
http_status_code_t handle_request(http_verb_t verb, char* resourcePath, int respondingSocket);

#endif