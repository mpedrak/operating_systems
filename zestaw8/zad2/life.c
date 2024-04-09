#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const int width = 30;
const int height = 30;

char *foreground;
char *background;
pthread_t * watki;
struct argumenty * args;
int ile_watkow;

void naKoncu () 
{
	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	free(watki);
	for(int i = 0; i < ile_watkow; i++) 
	{
		free(args[i].i);
		free(args[i].j);
	}
	free(args);
}

int main(int argc, char ** argv)
{
	atexit(naKoncu);
	
	if(argc != 2)
	{
		printf("Wrong argumnet count.\n");
		return 1;
	}
	
	ile_watkow = atoi(argv[1]);
	if(ile_watkow <= 0)
	{
		printf("Thread count must be grater than 0.\n");
		return 1;
	}

	if(ile_watkow > width * height) ile_watkow = width * height;
	int ile_dla_kazdego = width * height / ile_watkow;

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	foreground = create_grid();
	background = create_grid();
	char *tmp;

	init_grid(foreground);

	watki = malloc(ile_watkow * sizeof(pthread_t));
	bool pierwszy_raz = true;
	args = malloc(ile_watkow * sizeof(struct argumenty));

	int j = 0;
	for(int i = 0; i < ile_watkow - 1; i++) 
	{
		args[i].dst = background;
		args[i].src = foreground;
		args[i].i = malloc(ile_dla_kazdego * sizeof(int));
		args[i].j = malloc(ile_dla_kazdego * sizeof(int));
		for(int q = 0; q < ile_dla_kazdego; q++)
		{
			args[i].i[q] = j / width;
			args[i].j[q] = j % height;
			j++;
		}
		args[i].ile = ile_dla_kazdego;
	}

	int ile_pozostalo = width * height - ile_dla_kazdego * (ile_watkow - 1);
	args[ile_watkow - 1].dst = background;
	args[ile_watkow - 1].src = foreground;
	args[ile_watkow - 1].i = malloc(ile_pozostalo * sizeof(int));
	args[ile_watkow - 1].j = malloc(ile_pozostalo * sizeof(int));
	for(int q = 0; q < ile_pozostalo; q++)
	{
		args[ile_watkow - 1].i[q] = j / width;
		args[ile_watkow - 1].j[q] = j % height;
		j++;
	}
	args[ile_watkow - 1].ile = ile_pozostalo;
		
	while (true)
	{
		draw_grid(foreground);
		
		// Step simulation
		if(pierwszy_raz) 
		{
			for(int i = 0; i < ile_watkow; i++) pthread_create(&watki[i], NULL, update_grid, &args[i]);
			pierwszy_raz = false;
		}
		else for(int i = 0; i < ile_watkow; i++) pthread_kill(watki[i], SIGCONT);
		
		tmp = foreground;
		foreground = background;
		background = tmp;
		for(int i = 0; i < ile_watkow; i++)
		{
			args[i].dst = foreground;
			args[i].src = background;
		} 
		usleep(500 * 1000);
	}

	

	return 0;
}
