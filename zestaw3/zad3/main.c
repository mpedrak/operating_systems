#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

void doDir(char * path, char * argv2)
{   
    struct dirent * current;
    struct stat fileInfo;
    DIR * dir = opendir(path);
    if(dir == NULL) 
    {
        perror("Can't open directory.\n");
        return;
    }
    
    while ((current = readdir(dir)) != NULL)    
    {
        char * name = malloc(strlen(current->d_name) + 1);
        strcpy(name, current->d_name);
        
        char * pathWithName = malloc(strlen(path) + strlen(name) + 1);
        strcpy(pathWithName, path);
       
        strcat(pathWithName, name);
        stat(pathWithName, &fileInfo);

        if(S_ISDIR(fileInfo.st_mode)) // katalog
        {
            if(strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
            {
                int i = fork();
                if(i == 0) 
                {
                    pathWithName = realloc(pathWithName ,strlen(pathWithName) + 2);
                    strcat(pathWithName, "/");

                    doDir(pathWithName, argv2);

                    free(pathWithName);
                    free(name);
                    closedir(dir);
                    return;
                }
                else if(i > 0) wait(NULL);
                else 
                {
                    perror("Can't create process.\n");
                }
            }
        }
        else // nie katalog
        {
            FILE * we;
            we = fopen(pathWithName, "r");
            if (we != NULL)
            {
                char c[255];
                fread(c, sizeof(char), strlen(argv2), we);
                if (strcmp(c, argv2) == 0) printf("Path - %s, PID - %d.\n", pathWithName, getpid()); 
            }
            else
            {
                perror("Can't open file.\n");
            }
        }

        free(pathWithName);
        free(name);
    }

    closedir(dir);
}

int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        printf("Wrong argument amount.\n");
        return 1;
    }

    DIR * dir = opendir(argv[1]);
    if(dir == NULL)
    {
        perror("Wrong folder name / couldn't open.\n");
        return 1;
    }
    closedir(dir);

    if(strlen(argv[2]) > 255)
    {
        printf("Too big argv[2].\n");
        return 1;
    }

    char * argv1;
    if(argv[1][strlen(argv[1]) - 1] != '/')
    {
        argv1 = malloc(strlen(argv[1]) + 2);
        strcpy(argv1, argv[1]);
        strcat(argv1, "/");
    }
    else
    {
        argv1 = malloc(strlen(argv[1]) + 1);
        strcpy(argv1, argv[1]);
    }
    
    doDir(argv1, argv[2]);

    free(argv1);

    return 0;
}