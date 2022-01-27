#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"

int validateIP(char *IPAddr){
    char temp[60];
    strcpy(temp,IPAddr);
    int count = 0;
    char *token = strtok(temp,".");
    while(token != NULL){
        if(atoi(token) < 0 || atoi(token) > 255) return FALSE;
        token = strtok(NULL,".");
        count++;
    }
    if (count!=4) return FALSE;
    return TRUE;
}