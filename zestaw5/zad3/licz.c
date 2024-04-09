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

double f(double x)
{
    return 4.0 / (x * x + 1);
}

int main(int argc, char ** argv) 
{
    double calka = 0;
    double delta = atof(argv[1]);
    double obecneX = atof(argv[2]);
    double koniecX = atof(argv[3]);     
           
    while(obecneX + delta <= koniecX)
    {
        calka += f(obecneX + delta / 2) * delta;
        obecneX += delta;
    }
    double deltaTmp = koniecX - obecneX;
    calka += f(obecneX + deltaTmp / 2) * deltaTmp;
    
    FILE * file = fopen("fifo", "ab+");
    fwrite(&calka, sizeof(double), 1, file);
    fclose(file);
   
    return 0;
}
