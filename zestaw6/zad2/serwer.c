#include "definicje.h"

int myQueueID;
int newClientID;
int clientQueues[MAX_CLIENTS];
int logs;

void handler()
{
    struct msg msg;
    msg.msgType = STOP;
    for(int i = 0; i < MAX_CLIENTS; i++) if(clientQueues[i] != 0) mq_send(clientQueues[i], (char *)&msg, MSG_SIZE, STOP);
    mq_unlink(PROJECT_NAME);
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
        int clientQueueID = mq_open(currentMsg->text, O_WRONLY);
        if(clientQueueID == -1)
        {
            perror("Can't open client queue.\n");
            return;
        }
        clientQueues[newClientID] = clientQueueID;
        struct msg msg;
        msg.msgType = INIT;
        msg.clientID = newClientID;
        mq_send(clientQueueID, (char *)&msg, MSG_SIZE, INIT);
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
        mq_send(clientQueues[currentMsg->clientID], (char *)&msg, MSG_SIZE, LIST);
    }
    else if(type == TOALL)
    {
        struct msg msg;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINTTOALL;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        for(int i = 0; i < MAX_CLIENTS; i++) if(clientQueues[i] != 0) mq_send(clientQueues[i], (char *)&msg, MSG_SIZE, TOALL);
    }
    else if(type == TOONE)
    {
        struct msg msg;
        if(clientQueues[currentMsg->toOne] == 0) return;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINT;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        mq_send(clientQueues[currentMsg->toOne], (char *)&msg, MSG_SIZE, TOONE);
    }
    else if(type == STOP)
    {
        mq_close(clientQueues[currentMsg->clientID]);
        clientQueues[currentMsg->clientID] = 0;
    } 
}

int main()
{
    signal(SIGINT, handler);
    newClientID = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) clientQueues[i] = 0;
    logs = open("logs", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);   

    struct mq_attr attrubites;
    attrubites.mq_curmsgs = 0;
    attrubites.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    attrubites.mq_maxmsg = MAX_MESSAGES;
    attrubites.mq_msgsize = MSG_SIZE;
    
    myQueueID = mq_open(PROJECT_NAME, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, QUEUE_PERMISSION, &attrubites);
    if (myQueueID == -1)
    {
        perror("Cant't create server queue.\n");
        exit(0);
    }

    struct msg currentMsg;
    unsigned int p;

    while (7)
    {
        int t = mq_receive(myQueueID, (char *)&currentMsg, MSG_SIZE, &p);
        if(t == -1) continue;
        changeMode(&currentMsg);
    }

    return 0;
}
