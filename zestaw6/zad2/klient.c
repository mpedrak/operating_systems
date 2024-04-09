#include "definicje.h"

int myID;
int clientQueueID;
int serverQueueID;
char clientKey[MAX_TEXT_LENGHT];

void atExit()
{
    mq_unlink(clientKey);
}

void handler() 
{ 
    struct msg msg;
    msg.msgType = STOP;
    msg.clientID = myID;
    mq_send(serverQueueID, (char *)&msg, MSG_SIZE, STOP);
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

    sprintf(clientKey, "/%d", getpid());

    struct mq_attr attrubites;
    attrubites.mq_curmsgs = 0;
    attrubites.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    attrubites.mq_maxmsg = MAX_MESSAGES;
    attrubites.mq_msgsize = MSG_SIZE;
    
    clientQueueID = mq_open(clientKey, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, QUEUE_PERMISSION, &attrubites);
    if(clientQueueID == -1)
    {
        perror("Cant't create client queue.\n");
        exit(0);
    }

    serverQueueID = mq_open(PROJECT_NAME, O_WRONLY);
    if(serverQueueID == -1)
    {
        perror("Cant't open server queue.\n");
        exit(0);
    }

    struct msg msg;
    msg.msgType = INIT;
    msg.clientID = -7;
    strcpy(msg.text, clientKey);
    mq_send(serverQueueID, (char *)&msg, MSG_SIZE, INIT);

    struct msg currentMsg;
    unsigned int p;

    while (7)
    {
        int t = mq_receive(clientQueueID, (char *)&currentMsg, MSG_SIZE, &p);
        if(t != -1 && currentMsg.msgType == INIT)
        {
            myID = currentMsg.clientID;
            break;
        }
    }
    
    printf("My id %d.\n", myID);


    while (7)
    {
        int t = mq_receive(clientQueueID, (char *)&currentMsg, MSG_SIZE, &p);
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
                mq_close(serverQueueID);
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
                mq_send(serverQueueID, (char *)&msg, MSG_SIZE, LIST);
            }
            else if(strcmp(first, "2ALL") == 0)
            {
                char * second = strtok(NULL, "\n");
                msg.msgType = TOALL;
                msg.clientID = myID;
                strcpy(msg.text, second);
                mq_send(serverQueueID, (char *)&msg, MSG_SIZE, TOALL);
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
                mq_send(serverQueueID, (char *)&msg, MSG_SIZE, TOONE);
            }
            else if(strcmp(first, "STOP") == 0)
            {
                msg.msgType = STOP;
                msg.clientID = myID;
                mq_send(serverQueueID, (char *)&msg, MSG_SIZE, STOP);
                exit(0);
            }
            else printf("Wrong command.\n"); 
        }  
    }

    return 0;
}
