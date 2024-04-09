#include "definicje.h"

int mySocket;
int newClientID;
int existingClients[MAX_CLIENTS];
int activeClients[MAX_CLIENTS];
int logs;
struct sockaddr clientSockets[MAX_CLIENTS];
struct sockaddr_in clientArguments;
pthread_mutex_t ping_mutex = PTHREAD_MUTEX_INITIALIZER;

void handler()
{
    struct msg msg;
    msg.msgType = STOP;
    for(int i = 0; i < MAX_CLIENTS; i++) if(existingClients[i] != -1) 
    {
        sendto(mySocket, (char *)&msg, MSG_SIZE, 0,  &clientSockets[i], sizeof(clientSockets[i]));
        existingClients[i] = -1;
    }
    close(mySocket);
    close(logs);
    printf("%c", 13);
    exit(0);
}

void pinging()
{
    struct msg pingingMsg;
    pingingMsg.msgType = PING;
    while (7)
    {
        pthread_mutex_lock(&ping_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) if (existingClients[i] != -1)
        {
            activeClients[i] = -1;
            sendto(mySocket, (char *)&pingingMsg, MSG_SIZE, 0, &clientSockets[i], sizeof(clientSockets[i]));
        }
        pthread_mutex_unlock(&ping_mutex);
        sleep(7);
        pthread_mutex_lock(&ping_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) if (existingClients[i] != -1 && activeClients[i] == -1) existingClients[i] = -1;  
        pthread_mutex_unlock(&ping_mutex);
    }
}

int check_for_message(struct msg * currentMsg)
{
    int t;
    int size;

    while (7)
    {
        size = sizeof(clientArguments);
        t = recvfrom(mySocket, (char *)currentMsg, MSG_SIZE, MSG_DONTWAIT, (struct sockaddr *)&clientArguments, (socklen_t *)&size);
        if (t != -1) return t;
    } 
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
        if (newClientID >= MAX_CLIENTS) return;

        pthread_mutex_lock(&ping_mutex);
        existingClients[newClientID] = 1;
        activeClients[newClientID] = 1;
        memcpy(&clientSockets[newClientID], &clientArguments, sizeof(clientArguments));
        pthread_mutex_unlock(&ping_mutex);

        struct msg msg;
        msg.msgType = INIT;
        msg.clientID = newClientID;
        sendto(mySocket, (char *)&msg, MSG_SIZE, 0,  &clientSockets[newClientID], sizeof(clientSockets[newClientID]));
        newClientID++;
    }
    else if(type == LIST)
    {
        char line[MAX_TEXT_LENGHT];
        int j = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) 
            if(existingClients[i] != -1) 
            {
                line[j] = i + '0';
                j++;
            }
        line[j] = '\0';
        
        struct msg msg;
        msg.msgType = LIST;
        msg.clientID = currentMsg->clientID;
        strcpy(msg.text, line);
        sendto(mySocket, (char *)&msg, MSG_SIZE, 0,  &clientSockets[currentMsg->clientID], sizeof(clientSockets[currentMsg->clientID]));
    }
    else if(type == TOALL)
    {
        struct msg msg;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINTTOALL;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        for(int i = 0; i < MAX_CLIENTS; i++) if(existingClients[i] != -1) 
            sendto(mySocket, (char *)&msg, MSG_SIZE, 0,  &clientSockets[i], sizeof(clientSockets[i]));
    }
    else if(type == TOONE)
    {
        struct msg msg;
        if(existingClients[currentMsg->toOne] == -1) return;
        msg.clientID = currentMsg->clientID;
        msg.msgType = PRINT;
        strcpy(msg.text, currentMsg->text);
        strcpy(msg.date, asctime(timeInfo));
        sendto(mySocket, (char *)&msg, MSG_SIZE, 0,  &clientSockets[currentMsg->toOne], sizeof(clientSockets[currentMsg->toOne]));
    }
    else if(type == STOP)
    {
        pthread_mutex_lock(&ping_mutex);
        existingClients[currentMsg->clientID] = -1;
        pthread_mutex_unlock(&ping_mutex);
    } 
    else if(type == PING)
    {
        pthread_mutex_lock(&ping_mutex);
        activeClients[currentMsg->clientID] = 1;
        pthread_mutex_unlock(&ping_mutex);
    }
}

int main(int argc, char ** argv)
{
    signal(SIGINT, handler);

    newClientID = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) existingClients[i] = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) activeClients[i] = -1;

    logs = open("logs", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);   

    if (argc != 2)
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

    mySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mySocket == -1)
    {
        perror("Socket error.\n");
        exit(0);
    }
    
    struct sockaddr_in serverArguments;
    memset(&serverArguments, 0, sizeof(serverArguments));
    serverArguments.sin_family = AF_INET;
    serverArguments.sin_port = htons(port);
    serverArguments.sin_addr.s_addr = htonl(INADDR_ANY);

    int t = bind(mySocket, (struct sockaddr *)&serverArguments, sizeof(serverArguments));
    if (t == -1)
    {
        perror("Bind error.\n");
        exit(0);
    }

    fcntl(mySocket, F_SETFL, O_NONBLOCK);

    pthread_t thread_for_pinging;
    t = pthread_create(&thread_for_pinging, NULL, (void *)&pinging, NULL);
    if (t < 0)
    {
        perror("Thread create error.\n");
        exit(0);
    }
    
    struct msg currentMsg;

    while (7)
    {
        t = check_for_message(&currentMsg);
        if (t == -1) continue;
        
        changeMode(&currentMsg);
    }

    return 0;
}
