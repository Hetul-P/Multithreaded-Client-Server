#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "serialdeserial.h"

#define BUFFER_SIZE 1024

/*
This struct is created for the arguments of our handleObserver threads function
*/
typedef struct{
    int client_socket; // the socket
    int index; // index of our all_data[] array
} obs_arguments;

/*
This struct is created for the arguments of our handleUser threads function
*/
typedef struct{
    int client_socket; // the socket
    float interval; // the interval needed to wait before printing
} use_arguments;

/*
This function is for our observer threads
*/
void* handleObserver(void* arguments);

/*
This function is for our user threads
*/
void* handleUser(void* arguments);

/*
This function is for our 1 heart beat thread
*/
void* heartBeat(void* interval);

/*
This function creates a server, it takes in the port that it listens on and the interval
that the user function needs to wait before broadcasting
*/
void server(int sport, float interval, char* logfile);

/*
This function will print to a logfile if given one
*/
void writeToLogFile(char* writeline);