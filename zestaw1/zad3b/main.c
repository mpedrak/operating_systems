#include "lib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <inttypes.h>

double time(clock_t s, clock_t e)
{
    return (double)(e - s) / sysconf(_SC_CLK_TCK);
}

int main()
{
    printf("Commands - init x, count f, show i, delete i, destroy, stop.\n");

    tab * t = NULL;

    while (7)
    {
        char line[200];
        fgets(line, 200, stdin);
        char * command = strtok(line, " ");
        char * more = strtok(NULL, " ");
        int value; 
        if (more == NULL) command[strlen(command) - 1] = '\0';
        if (more != NULL) more[strlen(more) - 1] = '\0';
        clock_t start, end;
        struct tms start2, end2;
        start = times(&start2);

        if(strcmp(command, "destroy") == 0)
        {
            if (t != NULL) freeTab(t);
            else printf("No table to destroy.\n");
            t = NULL;
        }
        else if(strcmp(command, "init") == 0)
        {
            if (t != NULL) 
            {
                printf("Table is already initlilized.\n");
                continue;
            }
            if (more != NULL)
            {
                value = atoi(more);
                if (value > 0) t = createTable(value);
                else printf("Wrong size.\n");
            } 
            else 
                printf("No size.\n");
        }
        else if(strcmp(command, "count") == 0)
        {
            if(t != NULL && t->numberOfBlocks < t->maxSize) countWords(t, more);
            else printf("No space or table not intilized.\n");
        }
        else if(strcmp(command, "show") == 0)
        {
            if (t == NULL)
            {
                printf("No table.\n");
                continue;
            }
            if (more != NULL) value = atoi(more);
            else 
            {
                printf("No index.\n");
                continue;
            }
            if (value < 0 || value >= t->numberOfBlocks) 
            {
                printf("Wrong index.\n");
                continue;
            }
            printf("%s\n", getBlock(t, value));
        }
        else if(strcmp(command, "delete") == 0)
        {
            if (t == NULL)
            {
                printf("No table.\n");
                continue;
            }
            if (more != NULL) value = atoi(more);
            else 
            {
                printf("No index.\n");
                continue;
            }
            if (value < 0 || value > t->numberOfBlocks) 
            {
                printf("Wrong index.\n");
                continue;
            }
            deleteBlock(t, value);
        }
        else if(strcmp(command, "stop") == 0)
        {
            if (t != NULL) freeTab(t);
            return 0;
        }
        else
        {
            printf("Wrong command\n");
        }
        end = times(&end2);
        printf("Real time - %f, user time - %f, system time - %f.\n", time(start, end),
            time(start2.tms_cutime, end2.tms_cutime), 
            time(start2.tms_cstime, end2.tms_cstime));
        
    }

    return 0;
}