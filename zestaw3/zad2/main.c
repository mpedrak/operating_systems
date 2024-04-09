#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        printf("Wrong argument amount.\n");
        return 1;
    }

    DIR * dir = opendir(argv[1]);
    if(dir == NULL)
    {
        printf("Wrong folder name / couldn't open.\n");
        return 1;
    }
    closedir(dir);

    printf("Name - %s. ", argv[0]);
    fflush(stdout);
    execl("/bin/ls", "./ls", argv[1], NULL);

    return 0;
}