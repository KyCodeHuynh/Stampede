#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#define REQUEST_BUFFER_SIZE 4096
#define MAX_FILE_PATH_LENGTH 255

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

// Defined in server.c and declared here
// for use across multiple files.
extern const char HTTP_200_OK_RESPONSE[]; 
extern const char HTTP_404_NOT_FOUND_RESPONSE[];


// Parses a standard HTTP request
// Inputs: 
    // A requestBuffer filled with the HTTP request, with all CRLFs preserved
// Outputs: 
    // A pointer to a resourcePath like "/path/to/file.html" that will be replaced with the one used
    // A pointer to the type of request, a http_verb_t value, to be replaced with the one used
    // TODO: A pointer to an array of header/setting key-value pairs that must be freed later
    // TODO: Define some sort of struct for header/setting pairs
// Returns: 
    // 0 if successful 
    // -1 if invalid request
int parse_request(char* requestBuffer, http_verb_t* verb, char** resourcePath);

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