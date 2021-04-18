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

/*
This function retrieves information from the server and prints it
*/
void user_client(char* saddr, int sport);