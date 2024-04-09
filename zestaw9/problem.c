#include <locale.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <stdio_ext.h>

pthread_t mikolaj;
pthread_t renifery[9];
pthread_t elfy[10];

int wrocilo_reniferow = 0;
int problemy_elfow = 0;
int oczekujace_elfy[3];
int indeksy[10];

pthread_mutex_t mutex_reniferow = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_elfow = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_spania_mikolaja = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t warunek_mikolaja = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_dzialania_mikolaja = PTHREAD_MUTEX_INITIALIZER;

void handler () {}

int losuj_liczbe(int min, int max)
{
	int r = rand() % (max - min + 1) + min;
	return r;
}

void * f_mikolaja(void * arg)
{
	int dostarczyl_razy = 0;

	while(7)
	{
		pthread_mutex_lock(&mutex_spania_mikolaja);
		while(wrocilo_reniferow < 9 && problemy_elfow < 3) pthread_cond_wait(&warunek_mikolaja, &mutex_spania_mikolaja);

		printf("Mikolaj: budze sie.\n"); fflush(stdout);
		pthread_mutex_unlock(&mutex_spania_mikolaja);
		pthread_mutex_lock(&mutex_dzialania_mikolaja);

		if(problemy_elfow == 3)
		{
			printf("Mikolaj: rozwiazuje problemy elfow %d %d %d.\n", oczekujace_elfy[0], oczekujace_elfy[1], oczekujace_elfy[2]);
			fflush(stdout);
			int czas_rozwiazania = losuj_liczbe(1, 2);
			sleep(czas_rozwiazania);
			
			pthread_mutex_lock(&mutex_elfow);
			problemy_elfow = 0;
			for(int i = 0; i < 3; i++) oczekujace_elfy[i] = -1;
			pthread_mutex_unlock(&mutex_elfow);
		}
		else
		{
			pthread_mutex_lock(&mutex_reniferow);
			wrocilo_reniferow = 0;
			pthread_mutex_unlock(&mutex_reniferow);

			printf("Mikolaj: dostarczam zabawki.\n"); fflush(stdout);
			int czas_zabawek = losuj_liczbe(2, 4);
			sleep(czas_zabawek);

			dostarczyl_razy++;
			if(dostarczyl_razy == 3)
			{
				__fpurge(stdout); printf("Mikolaj: koniec problemu.\n"); fflush(stdout);
				exit(0);
			}
		}

		printf("Mikolaj: zasypiam.\n"); fflush(stdout);
		pthread_mutex_unlock(&mutex_dzialania_mikolaja);
	}
}

void * f_renifera(void * arg)
{
	int id = *(int *)arg;
	signal(SIGCONT, handler);

	while(7)
	{
		int czas_wakacji = losuj_liczbe(5, 10);
		sleep(czas_wakacji);

		int czy_to_ja = 0;

		pthread_mutex_lock(&mutex_reniferow);
		printf("Renifer: czeka %d reniferow na Mikolaja, %d.\n", wrocilo_reniferow + 1, id); fflush(stdout);

		if(wrocilo_reniferow == 8)
		{
			pthread_mutex_lock(&mutex_dzialania_mikolaja);
			pthread_mutex_lock(&mutex_spania_mikolaja);
			wrocilo_reniferow++;
			pthread_mutex_unlock(&mutex_reniferow);
			printf("Renifer: wybudzam Mikolaja, %d.\n", id); fflush(stdout);
			pthread_cond_broadcast(&warunek_mikolaja);
			pthread_mutex_unlock(&mutex_spania_mikolaja);
			for(int i = 0; i < 9; i++) if(i != id) pthread_kill(renifery[i], SIGCONT);
			pthread_mutex_unlock(&mutex_dzialania_mikolaja);
			czy_to_ja = 7;
		}
		else 
		{
			wrocilo_reniferow++;
			pthread_mutex_unlock(&mutex_reniferow);
		}

		if(czy_to_ja == 0) pause();
		int czas_zabawek = losuj_liczbe(2, 4);
		sleep(czas_zabawek);
		printf("Renifer: wracam na wakacje, %d.\n", id); fflush(stdout);
	}
}

void * f_elfa(void * arg)
{
	int id = *(int *)arg;
	signal(SIGCONT, handler);

	while (7)
	{
		int czas_pracy = losuj_liczbe(2, 5);
		sleep(czas_pracy);

		pthread_mutex_lock(&mutex_elfow);
		if(problemy_elfow < 3)
		{
			printf("Elf: czeka %d elfow na Mikolaja, %d.\n", problemy_elfow + 1, id); fflush(stdout);
			if(problemy_elfow == 2)
			{
				oczekujace_elfy[2] = id;
				problemy_elfow++;
				pthread_mutex_unlock(&mutex_elfow);
				pthread_mutex_lock(&mutex_spania_mikolaja);
				pthread_mutex_lock(&mutex_dzialania_mikolaja);
				printf("Elf: wybudzam Mikolaja, %d.\n", id); fflush(stdout);
				pthread_cond_broadcast(&warunek_mikolaja);
				pthread_mutex_unlock(&mutex_spania_mikolaja);
				for(int i = 0; i < 2; i++) pthread_kill(elfy[oczekujace_elfy[i]], SIGCONT);
				pthread_mutex_unlock(&mutex_dzialania_mikolaja);
			}
			else 
			{
				if(problemy_elfow == 0) oczekujace_elfy[0] = id;
				else oczekujace_elfy[1] = id;
				problemy_elfow++;
				pthread_mutex_unlock(&mutex_elfow);
				pause();
			}	
			usleep(10 * 1000);
			printf("Elf: Mikolaj rozwiazuje problem, %d.\n", id); fflush(stdout);
			int czas_rozwiazania = losuj_liczbe(1, 2);
			sleep(czas_rozwiazania);
		}
		else 
		{
			printf("Elf, samodzielnie rozwiazuje swoj problem, %d.\n", id); fflush(stdout);
			pthread_mutex_unlock(&mutex_elfow);
		}
	}
}

int main()
{
	srand(time(NULL));
	for(int i = 0; i < 3; i++) oczekujace_elfy[i] = -1;
	for(int i = 0; i < 10; i++) indeksy[i] = i;
	
	pthread_create(&mikolaj, NULL, f_mikolaja, NULL);
	for(int i = 0; i < 9; i++) pthread_create(&renifery[i], NULL, f_renifera, (void *)&indeksy[i]);
	for(int i = 0; i < 10; i++) pthread_create(&elfy[i], NULL, f_elfa, (void *)&indeksy[i]);
	
	while(7) sleep(7);

	return 0;
}
