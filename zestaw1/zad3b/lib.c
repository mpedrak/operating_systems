#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tab * createTable(unsigned int maxSize)
{
    tab * t = malloc(sizeof(tab));
    t->numberOfBlocks = 0;
    t->maxSize = maxSize;
    t->tab = calloc(maxSize, sizeof(char *));
    return t;
}

void countWords(tab * t, char * filePath)
{
    char wc[200];
    char tmpFilePath[] = "/tmp/XXXXXX";
    mkstemp(tmpFilePath);
    snprintf(wc, 200, "wc %s > %s", filePath, tmpFilePath);
    system(wc);

    FILE * f = fopen(tmpFilePath, "r");
    int size = 0;
    while (getc(f) != EOF) size++;
    fclose(f);
    f = fopen(tmpFilePath, "r");
    char * result = calloc(size, sizeof(char));
    fgets(result, size, f);
    fclose(f);

    t->tab[t->numberOfBlocks] = result;
    t->numberOfBlocks++;

    char rm[200];
    snprintf(rm, 200, "rm %s", tmpFilePath);
    system(rm);
}

char * getBlock(tab * t, unsigned int index)
{
    return t->tab[index];
}

void deleteBlock(tab * t, unsigned int index)
{
    free(t->tab[index]);
    for (int i = index; i < t->numberOfBlocks - 1; i++)
        t->tab[i] = t->tab[i + 1];
    t->numberOfBlocks--;
}

void freeTab(tab * t)
{
    for(int i = 0; i < t->numberOfBlocks; i++)
        free(t->tab[i]);
    free(t->tab);
}