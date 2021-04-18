#ifndef __SERIALDESERIAL_H__
#define __SERIALDESERIAL_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/time.h>

// the only struct that will be serialized and deserialized will be 
typedef struct{
    time_t      seconds;
    suseconds_t milliseconds;
    uint32_t mask; // refers to event->mask
    char* dir; // refers to event->name
    char* ip; // ip address
} notify_struct;

// this function takes in our notify_struct and serializes it into a buffer of bytes
uint8_t *serialise(notify_struct *sending_struct, int *buffer_len);

// this function takes in our buffer of bytes and the length of the buffer and turns it into our notify_struct
notify_struct *deserialise(uint8_t *buffer, int len);

#endif