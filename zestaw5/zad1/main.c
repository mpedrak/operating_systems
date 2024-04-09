#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int comparator(const void * av, const void * bv)
{
    char * a = (char *)av;
    char * b = (char *)bv;
    if((a[0] == ' ' || a[0] == '>') && (b[0] != ' ' && b[0] != '>')) return -1;
    if((a[0] != ' ' && a[0] != '>') && (b[0] == ' ' || b[0] == '>')) return 1;
    int q = 6;
    while (a[q] != ' ')
    {
        if(a[q] != b[q]) return a[q] - b[q];
        q++;
    }
    if (b[q] != ' ') return 1;
    return 0;
}

int main(int argc, char ** argv)
{
    if(argc == 4)
    {
        char komenda[250];
        sprintf(komenda, "exec bash -c 'mail -s \"%s\" %s <<< \"%s\"'", argv[2], argv[1], argv[3]);
        FILE * file = popen(komenda, "w");
        pclose(file);
    }   
    else if(argc == 2)
    {
        if(strcmp(argv[1], "data") == 0)
        {
            FILE * file = popen("exec bash -c 'mail <<< \"echo q\"'", "r");
            char wynik [250][250];
            int i = 0;
            while (fgets(wynik[i], 250, file) != NULL) i++;
            pclose(file);
            for(int j = 2; j < i - 2; j++) for(int q = 1; q < strlen(wynik[j]); q++) printf("%c", wynik[j][q]);
        }
        else if(strcmp(argv[1], "nadawca") == 0)
        {
            FILE * file = popen("exec bash -c 'mail <<< \"echo q\"'", "r");
            char wynik [250][250];
            int i = 0;
            while (fgets(wynik[i], 250, file) != NULL) i++;
            qsort(wynik, i, 250 * sizeof(char), comparator);
            for(int j = 0; j < i - 4; j++) for(int q = 1; q < strlen(wynik[j]); q++) printf("%c", wynik[j][q]);
        }
        else
        {
            printf("Wrong argument.\n");
            return 1;
        }
    }
    else
    {
        printf("Wrong argument count.\n");
        return 1;
    } 

    return 0;
}
