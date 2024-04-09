#include "definicje.h"

int myID;
int clientQueueID;
int serverQueueID;

void atExit()
{
    msgctl(clientQueueID, IPC_RMID, NULL);
}

void handler() 
{ 
    struct msg msg;
    msg.msgType = STOP;
    msg.clientID = myID;
    msgsnd(serverQueueID, &msg, MSG_SIZE, 0);
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

int main()
{
    atexit(atExit);
    signal(SIGINT, handler);
    
    key_t clientKey = ftok(PATH, getpid());
    if(clientKey == -1)
    {
        perror("Cant't create client key.\n");
        exit(0);
    }
 
    key_t serverKey = ftok(PATH, PROJECT_NAME);
    if(serverKey == -1)
    {
        perror("Cant't create server key.\n");
        exit(0);
    }

    clientQueueID = msgget(clientKey, IPC_CREAT | QUEUE_PERMISSION);
    if(clientQueueID == -1)
    {
        perror("Cant't create client queue.\n");
        exit(0);
    }

    serverQueueID = msgget(serverKey, IPC_CREAT);
    if(serverQueueID == -1)
    {
        perror("Cant't open server queue.\n");
        exit(0);
    }

    struct msg msg;
    msg.msgType = INIT;
    msg.clientID = -7;
    sprintf(msg.text, "%d", clientKey);
    msgsnd(serverQueueID, &msg, MSG_SIZE, IPC_NOWAIT);

    struct msg currentMsg;

    while (7)
    {
        msgrcv(clientQueueID, &currentMsg, MSG_SIZE, 0, MSG_NOERROR);
        if(currentMsg.msgType == INIT)
        {
            myID = currentMsg.clientID;
            break;
        }
    }

    printf("My id %d.\n", myID);

    while (7)
    {
        int p = msgrcv(clientQueueID, &currentMsg, MSG_SIZE, 0, IPC_NOWAIT);
        if(p != -1)
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
                msgsnd(serverQueueID, &msg, MSG_SIZE, 0);
            }
            else if(strcmp(first, "2ALL") == 0)
            {
                char * second = strtok(NULL, "\n");
                msg.msgType = TOALL;
                msg.clientID = myID;
                strcpy(msg.text, second);
                msgsnd(serverQueueID, &msg, MSG_SIZE, 0);
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
                msgsnd(serverQueueID, &msg, MSG_SIZE, 0);
            }
            else if(strcmp(first, "STOP") == 0)
            {
                msg.msgType = STOP;
                msg.clientID = myID;
                msgsnd(serverQueueID, &msg, MSG_SIZE, 0);
                exit(0);
            }
            else printf("Wrong command.\n");
        }  
    }

    return 0;
}
