#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        printf("Wrong argument amount.\n");
        return 1;
    }

    for (int i = 0; i < atoi(argv[1]); i++)
    {
        if(fork() == 0)
        {
            printf("PPID - %d, PID - %d.\n", (int)getppid(), (int)getpid()); // potomek
            return 0;
        }  
        wait(NULL);
    }
    printf("argv[1] - %d.\n", atoi(argv[1]));

    return 0;
}