#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/time.h>
#include "serialdeserial.h"

#define MONITOR_EVENT_MAX  2048
#define NAME_LEN  32 //file name length
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event)) //size of 1 event
#define OBS_BUFFER_SIZE MONITOR_EVENT_MAX * (MONITOR_EVENT_SIZE + NAME_LEN) // buffer length

typedef struct  {
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
}timeval;

/*
This function runs all of the inotify processes and sends information to the server
*/
void observer_client(char* saddr, int sport, char* fileordir);