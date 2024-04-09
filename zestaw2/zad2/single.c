#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h> 


int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        printf("Wrong amount of argumnets.\n");
        return 1;
    }

    struct timespec start, end;
    clock_gettime(0, &start);
    
    char c[1];
    FILE * we, * wy;
    we = fopen(argv[1], "r");
    wy = fopen(argv[2], "w"); 
    int i = -1;
    while(fseek(we, i, SEEK_END) == 0)
    {
        fread(c, sizeof(char), 1, we);
        i--;
        fwrite(c, sizeof(char), 1, wy);
    }

    clock_gettime(0, &end );
    printf("Real time of single use - %lfms.\n", (end.tv_nsec - start.tv_nsec) / (1000 * 1000.0));
            
    return 0;
}