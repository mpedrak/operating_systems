#include "definicje.h"

int myID;
int mySocket;
struct sockaddr_in clientArguments;

void closeSocket()
{
    close(mySocket);
    printf("%c", 13);
}

void handler() 
{ 
    struct msg msg;
    msg.msgType = STOP;
    msg.clientID = myID;
    sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
    exit(0); 
}

int canMakeNewRequest()
{
    struct timeval time;
    fd_set set;
    time.tv_sec = 0;
    time.tv_usec = 0;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    select(STDIN_FILENO + 1, &set, NULL, NULL, &time);
    return(FD_ISSET(0, &set));
}

int main(int argc, char ** argv)
{
    signal(SIGINT, handler);
    atexit(closeSocket);

    if (argc != 4)
    {
        printf("Wrong argument count.\n");
        exit(0);
    }

    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
    {
        printf("Wrong port number.\n");
        exit(0);
    }

    char * ip = argv[2];
    if (strlen(ip) < 7)
    {
        printf("Wrong ip.\n");
        exit(0);
    }

    char * name = argv[3];

    mySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mySocket == -1)
    {
        perror("Socket error.\n");
        exit(0);
    }

    memset(&clientArguments, 0, sizeof(clientArguments));
    clientArguments.sin_family = AF_INET;
    clientArguments.sin_port = htons(port);
    clientArguments.sin_addr.s_addr = inet_addr(ip);

    int t = connect(mySocket, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
    if (t == -1)
    {
        perror("Connect error.\n");
        exit(0);
    }

    struct msg msg;
    msg.msgType = INIT;
    sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));

    struct msg currentMsg;

    while (7)
    {
        int t = recvfrom(mySocket, (char *)&currentMsg, MSG_SIZE, 0, NULL, NULL);
        if(t != -1 && currentMsg.msgType == INIT)
        {
            myID = currentMsg.clientID;
            break;
        }
    }
    
    printf("My id %d, my name %s.\n", myID, name);

    while (7)
    {
        t = recvfrom(mySocket, (char *)&currentMsg, MSG_SIZE, MSG_DONTWAIT, NULL, NULL);
        if(t != -1)
        {
            if(currentMsg.msgType == LIST)
            {
                printf("Active clients id - ");
                printf("%d", currentMsg.text[0] - '0');
                for (int i = 1; currentMsg.text[i] != '\0'; i++) printf(", %d", currentMsg.text[i] - '0');
                printf("\n");
            } 
            else if(currentMsg.msgType == PRINT) 
            {
                if(currentMsg.clientID != myID) printf("%s from client %d, %s", currentMsg.text, currentMsg.clientID, currentMsg.date);
            }  
            else if(currentMsg.msgType == PRINTTOALL)
            {
                if(currentMsg.clientID != myID) printf("%s from client %d, %s", currentMsg.text, currentMsg.clientID, currentMsg.date);
            } 
            else if(currentMsg.msgType == STOP)
            {
                printf("Server stopped.\n");
                exit(0);
            }
            else if (currentMsg.msgType == PING)
            {
                struct msg msg;
                msg.clientID = myID;
                msg.msgType = PING;
                sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
            }
        }
        if(canMakeNewRequest())
        {
            char line[MAX_TEXT_LENGHT];
            fgets(line, MAX_TEXT_LENGHT, stdin);
            int position = 0;
            for( ; position < MAX_TEXT_LENGHT && line[position] != ' '; position++) ;
            if(position != MAX_TEXT_LENGHT) line[position] = '\n';
            char * first = strtok(line, "\n");
            struct msg msg;
        
            if(strcmp(first, "LIST") == 0)
            {   
                msg.msgType = LIST;
                msg.clientID = myID;
                sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
            }
            else if(strcmp(first, "2ALL") == 0)
            {
                char * second = strtok(NULL, "\n");
                msg.msgType = TOALL;
                msg.clientID = myID;
                strcpy(msg.text, second);
                sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
            }
            else if(strcmp(first, "2ONE") == 0)
            {
                for( ; position < MAX_TEXT_LENGHT && line[position] != ' '; position++) ;
                if(position != MAX_TEXT_LENGHT) line[position] = '\n';
                char * second = strtok(NULL, "\n");
                char * third = strtok(NULL, "\n");
                
                msg.msgType = TOONE;
                msg.clientID = myID;
                msg.toOne = atoi(second);
                strcpy(msg.text, third);
                sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
            }
            else if(strcmp(first, "STOP") == 0)
            {
                msg.msgType = STOP;
                msg.clientID = myID;
                sendto(mySocket, (char *)&msg, MSG_SIZE, 0, (struct sockaddr *)&clientArguments, sizeof(clientArguments));
                exit(0);
            }
            else printf("Wrong command.\n"); 
        }  
    }

    return 0;
}
