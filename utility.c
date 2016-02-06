#include "utility.h" 

#include <stdlib.h> 
#include <stdio.h>

void error(char* message)
{
    // We use 1 as our exit convention as this 
    // is a standalone web server that may be scripted. 
    // Bash convention treats 1 as a general error. 
    // We would use -1 were this a library, as POSIX
    // convention uses < 0 for errors.
    perror(message); 
    exit(EXIT_FAILURE);
}