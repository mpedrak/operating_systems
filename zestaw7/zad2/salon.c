#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int M_fryzjerow, N_foteli, P_poczekalni, F_fryzur;

int segment_poczekalni;
int * poczekalnia; 
sem_t * fryzjerzy;
sem_t * fotele;

void handler()
{
    munmap(poczekalnia, P_poczekalni * sizeof(int));
    shm_unlink("/poczekalnia77");
    sem_close(fryzjerzy);
    sem_unlink("/fryzjerzy77");
    sem_close(fotele);
    sem_unlink("/fotele77");
    printf("%c", 13);
    exit(0);
}

int main(int argc, char ** argv)
{
    signal(SIGINT, handler);
    
    if(argc != 5)
    {
        printf("Wrong arguments amount.\n");
        return 1;
    }

    M_fryzjerow = atoi(argv[1]);
    N_foteli = atoi(argv[2]);
    P_poczekalni = atoi(argv[3]);
    F_fryzur = atoi(argv[4]);

    if(M_fryzjerow <= 0 || N_foteli <= 0 || P_poczekalni <= 0 || F_fryzur <= 0)
    {
        printf("All arguments must be greater than zero.\n");
        exit(0);
    }
    
    if(M_fryzjerow < N_foteli)
    {
        printf("M must be grater or equal than N.\n");
        exit(0);
    }

    segment_poczekalni = shm_open("/poczekalnia77", O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
    ftruncate(segment_poczekalni, P_poczekalni * sizeof(int));
    poczekalnia = mmap(NULL, P_poczekalni * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, segment_poczekalni, 0);
    if(poczekalnia == (int *)-1)
    {
        perror("Cant't create shared memory segment.\n");
        exit(0);
    }

    for(int i = 0; i < P_poczekalni; i++) poczekalnia[i] = 0;

    fryzjerzy = sem_open("/fryzjerzy77", O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, M_fryzjerow);
    if(fryzjerzy == SEM_FAILED)
    {
        perror("Cant create semafor for barbers");
        exit(0);
    }

    fotele = sem_open("/fotele77", O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, N_foteli);
    if(fotele == SEM_FAILED)
    {
        perror("Cant create semafor for fotels");
        exit(0);
    }
    
    int nr_klienta = 1;
    
    while(7)
    {
        int i = 0;
        for( ; i < P_poczekalni; i++) if(poczekalnia[i] == 0) break;

        if(i == P_poczekalni) printf("No space for client %d in waiting room, so he leaves.\n", nr_klienta); 
        else poczekalnia[i] = nr_klienta;
        fflush(stdout);  
        
        if(fork() == 0)
        {
            sem_wait(fryzjerzy);
            sem_wait(fotele);

            int id_klienta = poczekalnia[0];
            if(id_klienta != 0)
            {
                for(int i = 0; i < P_poczekalni - 1; i++) poczekalnia[i] = poczekalnia[i + 1];
                poczekalnia[P_poczekalni - 1] = 0;

                srand(time(NULL));
                int fryzura = rand() % F_fryzur + 1;

                printf("Started hairstyle %d for client %d.\n", fryzura, id_klienta); fflush(stdout);
                sleep(fryzura);
                printf("Done hairstyle %d for client %d.\n", fryzura, id_klienta); fflush(stdout);
            }

            sem_post(fryzjerzy);
            sem_post(fotele);
            exit(0);
        }

        sleep(2);
        nr_klienta++;

        if(poczekalnia[0] != 0)
        {
            printf("Clients in waiting room - ");
            for (int j = 0; j < P_poczekalni && poczekalnia[j] != 0; j++) printf("%d ", poczekalnia[j]);
            printf("\n"); fflush(stdout);
        }
    }

    return 0;
}
