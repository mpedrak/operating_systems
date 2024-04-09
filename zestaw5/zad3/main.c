#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> 
#include <time.h>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        printf("Wrong argument amount.\n");
        return 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    double delta = atof(argv[1]);
    int n = atoi(argv[2]);
    double ileDlaProcesu = 1.0 / n;

    mkfifo("fifo", 0700);
    FILE * file = fopen("fifo", "ab+");
    
    for(int i = 0; i < n; i++)
    {
        if (fork() == 0)
        {
            char argv1[100];
            char argv2[100];
            char argv3[100];
            sprintf(argv1, "%.20f", delta);
            sprintf(argv2, "%.20f", ileDlaProcesu * i);
            sprintf(argv3, "%.20f", ileDlaProcesu * (i + 1));
            execl("./licz", "./licz", argv1, argv2, argv3, NULL);
            return 0;
        }
    }
    
    for(int i = 0; i < n; i++) wait(NULL);

    double sumaCalkowita = 0;

    for(int i = 0; i < n; i++)
    {
        double sumaCzesciowa;
        fread(&sumaCzesciowa, sizeof(double), 1, file);
        sumaCalkowita += sumaCzesciowa;
    }
    
    gettimeofday(&end, NULL);
    double czasu = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

    printf("Sum of integral is - %f. Number of proces - %d.\n", sumaCalkowita, n);
    printf("Time - %.2f ms. Accuracy - %.10f.\n\n", czasu, delta);

    fclose(file); 
    char usunFifo[100] = "rm fifo";
    system(usunFifo);
    
    return 0;
}
