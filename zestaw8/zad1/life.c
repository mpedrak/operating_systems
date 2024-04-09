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


void naKoncu () 
{
	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);
}

int main()
{
	atexit(naKoncu);
	
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	foreground = create_grid();
	background = create_grid();
	char *tmp;

	init_grid(foreground);

	pthread_t watki[height][width];
	bool pierwszy_raz = true;
	struct argumenty args[height][width];
	for(int i = 0; i < height; i++) 
		for(int j = 0; j < width; j++)
		{
			args[i][j].dst = background;
			args[i][j].src = foreground;
			args[i][j].i = i;
			args[i][j].j = j;
		}

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		if(pierwszy_raz) 
		{
			for(int i = 0; i < height; i++) 
				for(int j = 0; j < width; j++)
					pthread_create(&watki[i][j], NULL, update_grid, &args[i][j]);
			pierwszy_raz = false;
		}
		else
		{
			for(int i = 0; i < height; i++) 
				for(int j = 0; j < width; j++)
					pthread_kill(watki[i][j], SIGCONT);
		}
		
		tmp = foreground;
		foreground = background;
		background = tmp;
		for(int i = 0; i < height; i++) 
			for(int j = 0; j < width; j++)
			{
				args[i][j].dst = foreground;
				args[i][j].src = background;
			} 
	}

	
	return 0;
}
