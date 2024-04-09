#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>
#include <stdlib.h>


int main(int argc, char ** argv)
{
    if(argc != 5)
    {
        printf("Wrong amount of argumnets.\n");
        return 1;
    }

    struct timespec start, end;
    clock_gettime(0, &start);

    char c[1];
    FILE * we, * wy;
    we = fopen(argv[3], "r");
    wy = fopen(argv[4], "w");   
    while(fread(c, sizeof(char), 1, we) > 0)
    {
        if(c[0] == argv[1][0]) fwrite(argv[2], sizeof(char), 1, wy);
        else fwrite(c, sizeof(char), 1, wy);
    }

    clock_gettime(0, &end );
    printf("Real time of using fuctions with 'f' - %lfms.\n", (end.tv_nsec - start.tv_nsec) / (1000 * 1000.0));
         
    return 0;
}