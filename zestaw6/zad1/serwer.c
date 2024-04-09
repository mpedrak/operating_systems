#include "definicje.h"

int myQueueID;
int newClientID;
int clientQueues[MAX_CLIENTS];
int logs;

void handler()
{
    struct msg msg;
    msg.msgType = STOP;
    for(int i = 0; i < MAX_CLIENTS; i++) if(clientQueues[i] != 0) msgsnd(clientQueues[i], &msg, MSG_SIZE, 0);
    msgctl(myQueueID, IPC_RMID, NULL);
    close(logs);
    exit(0);
}

void changeMode(struct msg * currentMsg)
{
    long type = currentMsg->msgType;
    
    time_t t;
    struct tm * timeInfo;
    time(&t);
    timeInfo = localtime(&t);
    char line[MAX_TEXT_LENGHT];
    int n = snprintf(line, MAX_TEXT_LENGHT, "Client id %d, Command %ld, Date %s", currentMsg->clientID, type, asctime(timeInfo));
    write(logs, line, n);

    if (type == INIT)
    {
        key_t key = atoi(currentMsg->text);
        int clientQueueID = msgget(key, IPC_CREAT);
        if (clientQueueID == -1)
        {
            perror("Cant't open client queue.\n");
            return;
        }
        clientQueues[newClientID] = clientQueueID;
        struct msg msg;
        msg.msgType = INIT;
        msg.clientID = newClientID;
        msgsnd(clientQueueID, &msg, MSG_SIZE, 0);
        newClientID++;
    }
    else if(type == LIST)
    {
        char line[MAX_TEXT_LENGHT];
        int j = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) 
            if(clientQueues[i] != 0) 
            {
                line[j] = i + '0';
                j++;
            }
        line[j] = '\0';
        struct msg msg;
        msg.msgType = LIST;
        msg.clientID = currentMsg->clientID;
        strcpy(msg.text, line);
        msgsnd(clientQueues[currentMsg->clientID], &msg, MSG_SIZE, 0);
    }
    else if(type == TOALL)
    {
        struct msg msg;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINTTOALL;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        for(int i = 0; i < MAX_CLIENTS; i++) if(clientQueues[i] != 0) msgsnd(clientQueues[i], &msg, MSG_SIZE, 0);
    }
    else if(type == TOONE)
    {
        struct msg msg;
        if(clientQueues[currentMsg->toOne] == 0) return;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINT;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        msgsnd(clientQueues[currentMsg->toOne], &msg, MSG_SIZE, 0);
    }
    else if(type == STOP)
    {
        clientQueues[currentMsg->clientID] = 0;
    }
}

int main()
{
    signal(SIGINT, handler);
    newClientID = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) clientQueues[i] = 0;
    logs = open("logs", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); 

    key_t key = ftok(PATH, PROJECT_NAME);
    if(key == -1)
    {
        perror("Cant't create server key.\n");
        exit(0);
    }

    myQueueID = msgget(key, IPC_CREAT | QUEUE_PERMISSION);
    if (myQueueID == -1)
    {
        perror("Cant't create server queue.\n");
        exit(0);
    }

    struct msg currentMsg;

    while (7)
    {
        msgrcv(myQueueID, &currentMsg, MSG_SIZE, -50, MSG_NOERROR);
        changeMode(&currentMsg);
    }
     
    return 0;
}
