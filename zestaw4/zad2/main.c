#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void handleSigInfo(int sig, siginfo_t * info, void * ucontext)
{
    printf("PID - %d.\n", (int)info->si_pid);
    printf("Signal - %d.\n", sig);
    printf("UID - %d.\n", (int)info->si_uid);
    printf("User time consumed - %d.\n", (int)info->si_utime);
    printf("System time consumed - %d.\n", (int)info->si_stime); 
}

void handleResetHand(int sig)
{
    printf("Signal %d handled (next will not and program will be closed).\n", sig);
}

void handleOnStack(int sig)
{
    printf("Signal %d handled (using alterante signal stack).\n", sig);
}

int main()
{
    printf("Flag - SA_SIGINFO - gives information.\n");
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handleSigInfo;
    sigaction(SIGUSR1, &act, NULL);
    raise(SIGUSR1);

    printf("\nFlag - SA_ONSTACK - call the signal handler on an alternate stack.\n");
    struct sigaction act3;
    sigemptyset(&act3.sa_mask);
    act3.sa_flags = SA_ONSTACK;
    act3.sa_handler = handleOnStack;
    sigaction(SIGUSR1, &act3, NULL);
    raise(SIGUSR1);

    printf("\nFlag - SA_RESETHAND - delete handle pointer after one use.\n");
    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_RESETHAND;
    act2.sa_handler = handleResetHand;
    sigaction(SIGUSR1, &act2, NULL);
    raise(SIGUSR1);
    raise(SIGUSR1);

    return 0;
}
