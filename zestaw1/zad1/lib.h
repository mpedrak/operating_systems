typedef struct tab
{
    unsigned int maxSize;
    unsigned int numberOfBlocks;
    char ** tab;
} tab;

tab * createTable(unsigned int maxSize);
void countWords(tab * t, char * filePath);
char * getBlock(tab * t, unsigned int index);
void deleteBlock(tab * t, unsigned int index);
void freeTab(tab * t);
