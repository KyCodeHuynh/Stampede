#ifndef UTILITY_H
#define UTILITY_H 

// Wrapper for perror()
void error(char* message);

// Handles reaping zombie children, 
// which send a SIGCHLD to their parent upon exiting
// See: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void signal_handler_sigchld(int signal);

// TODO: Create a proper str_tolower() method
// Converts a null-terminated string to all lowercase. 
// Example: "HeLLo WoRLD" -> "hello world"
// void str_tolower(char* string);

#endif