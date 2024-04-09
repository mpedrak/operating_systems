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
    if(argc < 2 && argc > 3)
    {
        printf("Wrong argument count.\n");
        return 1;
    }

    if(strcmp(argv[1], "ignore") == 0)
    {
        if(argc == 2) signal(SIGUSR1, SIG_IGN);

        raise(SIGUSR1);

        if(argc == 2) execl(argv[0], argv[0], argv[1], "7", NULL);
        else return 0;
    }

    else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        if(argc == 2)
        {
            sigset_t newSet, oldSet;
            sigemptyset(&newSet);
            sigemptyset(&oldSet);
            sigaddset(&newSet, SIGUSR1);
            sigprocmask(SIG_BLOCK, &newSet, &oldSet);
        }
       
        if(argc == 2 || strcmp(argv[1], "mask") == 0 ) raise(SIGUSR1);

        sigset_t testingSet;
        sigemptyset(&testingSet); 
        sigpending(&testingSet);
        if(sigismember(&testingSet, SIGUSR1) == 1)
        {
            if(argc == 2) printf("Signal is in pending set.\n");
            else printf("(As execl) Signal is in pending set.\n");
        }

        if(argc == 2) execl(argv[0], argv[0], argv[1], "7", NULL);
        else return 0;
    }

    else 
    {
        printf("Wrong argument.\n");
        return 1;
    }

    return 0;
}