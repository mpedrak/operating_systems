#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

double f(double x)
{
    return 4.0 / (x * x + 1);
}

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
    int ** fd;
    fd = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) fd[i] = malloc(2 * sizeof(int));

    for(int i = 0; i < n; i++)
    {
        pipe(fd[i]);
        if(fork() == 0)
        {
            close(fd[i][0]);

            double calka = 0;
            double obecneX = ileDlaProcesu * i;
           
            while(obecneX + delta <= ileDlaProcesu * (i + 1))
            {
                calka += f(obecneX + delta / 2) * delta;
                obecneX += delta;
            }
            double deltaTmp = ileDlaProcesu * (i + 1) - obecneX;
            calka += f(obecneX + deltaTmp / 2) * deltaTmp;
    
            char buffer[100];
            sprintf(buffer, "%f", calka);
            write(fd[i][1], &buffer, 100);
            close(fd[i][1]);
            return 0;
        }
        else close(fd[i][1]);
    }

    for(int i = 0; i < n; i++) wait(NULL);

    double sumaCalkowita = 0;

    for(int i = 0; i < n; i++)
    {
        char buffer[100];
        read(fd[i][0], &buffer, 100);
        double sumaCzesciowa = atof(buffer);
        sumaCalkowita += sumaCzesciowa;
        close(fd[i][0]);
    }
    
    gettimeofday(&end, NULL);
    double czasu = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

    printf("Sum of integral is - %f. Number of proces - %d.\n", sumaCalkowita, n);
    printf("Time - %.2f ms. Accuracy - %.10f.\n\n", czasu, delta);

    for (int i = 0; i < n; i++) free(fd[i]);
    free(fd);
    
    return 0;
}
