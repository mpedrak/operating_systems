#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 20
#define MAX_TEXT_LENGHT 250
#define MSG_SIZE sizeof(struct msg)

#define INIT 1
#define STOP 2
#define LIST 3
#define TOALL 4
#define TOONE 5
#define PRINT 6
#define PRINTTOALL 7
#define PING 8

struct msg 
{
    long msgType;
    int clientID;
    int toOne;
    char text[MAX_TEXT_LENGHT];
    char date[MAX_TEXT_LENGHT];
};
