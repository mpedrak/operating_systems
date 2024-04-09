#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

int zmian = 0;
int czyTrybCztery = 0;
int poprzedniTryb = -1;

void zrobTrybPracy(int tryb)
{
    czyTrybCztery = 0;
    if(tryb == 1)
    {
        for (int i = 1; i <= 100; i++)
            printf("%d ", i);
        printf("\n");
    }
    else if(tryb == 2)
    {   
        time_t t;
        struct tm * timeInfo;
        time(&t);
        timeInfo = localtime(&t);
        printf("Podaje aktualny czas - jest godzina - %s", asctime(timeInfo));
    }
    else if(tryb == 3)
    {
        printf("Liczba otrzymany zadan zmian - %d.\n", zmian);
    }
    else if(tryb == 4)
    {
        czyTrybCztery = 7;
    }
    else if(tryb == 5)
    {
        printf("To koniec catchera.\n");
    }
}

void handleSigInfo(int sig, siginfo_t * info, void * ucontext)
{
    if(poprzedniTryb != info->si_value.sival_int) zmian++;
    poprzedniTryb = info->si_value.sival_int;
    kill(info->si_pid, SIGUSR1);
    zrobTrybPracy(info->si_value.sival_int);
    if(info->si_value.sival_int == 5) exit(0);
}

int main(int argc, char ** argv)
{
    printf("Catcher PID - %d.\n", (int)getpid());

    zmian = 0;
    czyTrybCztery = 0;
    poprzedniTryb = -1;

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handleSigInfo;
    sigaction(SIGUSR1, &act, NULL);

    time_t t;
    struct tm * timeInfo;
    while(7)
    {
        if(czyTrybCztery)
        {
            time(&t);
            timeInfo = localtime(&t);
            printf("Podaje aktualny czas - jest godzina - %s", asctime(timeInfo));
            sleep(1);
        }
    }
    
    return 0;
}