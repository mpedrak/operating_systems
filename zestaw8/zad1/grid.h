#pragma once
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

struct argumenty
{
    char * src;
    char * dst;
    int i; 
    int j;
};

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void * update_grid(void * varg);
