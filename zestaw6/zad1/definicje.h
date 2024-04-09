#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 

#define PROJECT_NAME 'Z'
#define MAX_CLIENTS 100
#define PATH getenv("HOME")
#define MAX_TEXT_LENGHT 250
#define QUEUE_PERMISSION 0666
#define MSG_SIZE sizeof(struct msg) - sizeof(long)

#define INIT 1
#define STOP 2
#define LIST 3
#define TOALL 4
#define TOONE 5
#define PRINT 6
#define PRINTTOALL 7

struct msg 
{
    long msgType;
    int clientID;
    char text[MAX_TEXT_LENGHT];
    char date[MAX_TEXT_LENGHT];
    int toOne;
};
