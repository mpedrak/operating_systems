#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void handleSignal() {}

int main(int argc, char ** argv)
{
    if(argc < 3) 
    {
        printf("Wrong argument amount.\n");
        return 1;
    }
    for(int i = 2; i < argc; i++)
    {
        int t = atoi(argv[i]);
        if(t < 1 || t > 5)
        {
            printf("Work mode - %d is wrong.\n", t );
            return 1;
        }
    }

    signal(SIGUSR1, handleSignal);
   
    for(int i = 2; i < argc; i++)
    {
        union sigval val;
        val.sival_int = atoi(argv[i]);
        sigqueue(atoi(argv[1]), SIGUSR1, val);

        sigset_t set;
        sigemptyset(&set);
        sigsuspend(&set);
        if(atoi(argv[i]) == 5) break;
    }
   
    return 0;
}