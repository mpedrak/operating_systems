#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main()
{
    DIR * dir = opendir("./");
    struct dirent * current;
    struct stat fileInfo;
    long long int sum = 0;

    while ((current = readdir(dir)) != NULL)    
    {
        stat(current->d_name, &fileInfo);
        if(S_ISDIR(fileInfo.st_mode)) continue;

        printf("Size - %lld, name - %s.\n", (long long int)fileInfo.st_size, current->d_name);
        sum += (long long int)fileInfo.st_size;
    }
    
    printf("Cumulative size - %lld.\n", sum);

    closedir(dir);
    return 0;
}