#include "serialdeserial.h"

uint8_t *serialise(notify_struct *sending_struct, int *buffer_len){
    int str1_len = strlen(sending_struct->dir);
    int str2_len = strlen(sending_struct->ip);

    *buffer_len = str1_len + 1 + str2_len + 1 + sizeof(uint32_t) + sizeof(suseconds_t) + sizeof(time_t);
    
    uint8_t *buffer = calloc(sizeof(uint8_t), *buffer_len);
    if (buffer == NULL) {
        return NULL;
    }

    // sets the int and float paramters
    *(time_t *)(buffer) = sending_struct->seconds;
    *(suseconds_t *)(buffer + sizeof(time_t)) = sending_struct->milliseconds;
    *(uint32_t *)(buffer + sizeof(time_t) + sizeof(suseconds_t)) = sending_struct->mask;

    memcpy(buffer + sizeof(time_t) + sizeof(suseconds_t) + sizeof(uint32_t) , sending_struct->dir, str1_len);
    memcpy(buffer + sizeof(time_t) + sizeof(suseconds_t) + sizeof(uint32_t) + str1_len + 1, sending_struct->ip, str2_len);
    

    return buffer;
}

notify_struct *deserialise(uint8_t *buffer, int len) {
    
    notify_struct *receive_struct = malloc(sizeof(notify_struct));

    receive_struct->seconds = *(time_t *)(buffer);
    receive_struct->milliseconds = *(suseconds_t *)(buffer + sizeof(time_t));
    receive_struct->mask = *(uint32_t *)(buffer + sizeof(time_t) + sizeof(suseconds_t));

    int str1_len = strnlen(buffer + sizeof(uint32_t) + sizeof(suseconds_t) + sizeof(time_t),
                     len - sizeof(uint32_t) + sizeof(suseconds_t) + sizeof(time_t) - 1);
                     
    //int str_len = len - sizeof(time_t) + sizeof(suseconds_t) + sizeof(uint32_t);
    
    receive_struct->dir = calloc(str1_len, sizeof(char));
    strcpy(receive_struct->dir,(char *)(buffer + sizeof(time_t) + sizeof(suseconds_t) + sizeof(uint32_t)));

    int str2_len = len - (sizeof(uint32_t) + sizeof(suseconds_t) + sizeof(time_t) + str1_len + 1);

    receive_struct->ip = calloc(str2_len, sizeof(char));
    strcpy(receive_struct->ip,(char *)(buffer + sizeof(time_t) + sizeof(suseconds_t) + sizeof(uint32_t) + str1_len + 1));
    

    
    return receive_struct;
}
