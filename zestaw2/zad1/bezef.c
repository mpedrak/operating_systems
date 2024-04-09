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

    char c;
    int we, wy;
    we = open(argv[3], O_RDONLY);
    wy = open(argv[4], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);   
    while(read(we, &c, 1) == 1)
    {
        if(c == argv[1][0]) write(wy, argv[2], 1);
        else write(wy, &c, 1);
    }

    clock_gettime(0, &end );
    printf("Real time of using functions without 'f' - %lfms.\n", (end.tv_nsec - start.tv_nsec) / (1000 * 1000.0));
            
    return 0;
}