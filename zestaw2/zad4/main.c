#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

long long int sum = 0;

int zrob(const char * name, const struct stat * fileInfo, int flag)
{
    if(!S_ISDIR(fileInfo->st_mode)) 
    {
        printf("Size - %lld, name - %s.\n", (long long int)fileInfo->st_size, name);
        sum += (long long int)fileInfo->st_size;
    }
    return 0;
}

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

    
    sum = 0;
    ftw(argv[1], zrob, 1000 * 1000);
    printf("Cumulative size - %lld.\n", sum);

    
    return 0;
}