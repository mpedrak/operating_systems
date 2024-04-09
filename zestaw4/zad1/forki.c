#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void func()
{
    printf("Signal handled.\n");
}

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        printf("Wrong argument count.\n");
        return 1;
    }

    if(strcmp(argv[1], "ignore") == 0 || strcmp(argv[1], "handler") == 0 )
    {
        if(strcmp(argv[1], "ignore") == 0 ) signal(SIGUSR1, SIG_IGN);
        else if (strcmp(argv[1], "handler") == 0) signal(SIGUSR1, func);

        raise(SIGUSR1);

        if(fork() == 0)
        {
            if(strcmp(argv[1], "handler") == 0 ) printf("(As child process) ");
            raise(SIGUSR1);
            return 0;
        }
        else wait(NULL);
    }

    else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t newSet, oldSet;
        sigemptyset(&newSet);
        sigemptyset(&oldSet);
        sigaddset(&newSet, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newSet, &oldSet);

        raise(SIGUSR1);

        sigset_t testingSet;
        sigemptyset(&testingSet);
        sigpending(&testingSet);
        if(sigismember(&testingSet, SIGUSR1) == 1)
        {
            printf("Signal is in pending set.\n");
        }

        if(fork() == 0)
        {
            if(strcmp(argv[1], "mask") == 0) raise(SIGUSR1);
            sigset_t testingSet2;
            sigemptyset(&testingSet2);
            sigpending(&testingSet2);
            if(sigismember(&testingSet2, SIGUSR1) == 1)
            {
                printf("(As child process) Signal is in pending set.\n");
            }
            return 0;
        }
        else wait(NULL);
    }

    else 
    {
        printf("Wrong argument.\n");
        return 1;
    }

    return 0;
}