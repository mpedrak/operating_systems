#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define PATH getenv("HOME")

int M_fryzjerow, N_foteli, P_poczekalni, F_fryzur;

int segment_poczekalni;
int * poczekalnia; 
int semafory; // pierwszy to fryzjerzy, drugi fotele

void handler()
{
    shmdt(poczekalnia);
    shmctl(segment_poczekalni, IPC_RMID, NULL);
    semctl(semafory, 0, IPC_RMID, NULL);
    printf("%c", 13);
    exit(0);
}

union semun
{
    int val;
    struct semid_ds * a;
    unsigned short * b; 
    struct seminfo * c; 
};

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

    key_t klucz = ftok(PATH, getpid());
    semafory = semget(klucz, 2, IPC_CREAT | IPC_EXCL | 0770);
    if(semafory == -1) 
    {
        perror("Can't create semafores set.\n");
        exit(0);
    }

    klucz = ftok(PATH, 'A');
    segment_poczekalni = shmget(klucz, P_poczekalni * sizeof(int), IPC_CREAT | IPC_EXCL | 0770);
    poczekalnia = shmat(segment_poczekalni, NULL, 0);
    if(poczekalnia == (int *)-1)
    {
        perror("Cant't create shared memory segment.\n");
        exit(0);
    }

    for(int i = 0; i < P_poczekalni; i++) poczekalnia[i] = 0;

    union semun arg;
    arg.val = M_fryzjerow;
    semctl(semafory, 0, SETVAL, arg);
    arg.val = N_foteli;
    semctl(semafory, 1, SETVAL, arg);
    
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
            struct sembuf sops[2];
            sops[0].sem_num = 0;
            sops[1].sem_num = 1;
            sops[0].sem_op = sops[1].sem_op = -1;
            semop(semafory, sops, 2);

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

            sops[0].sem_op = sops[1].sem_op = 1;
            semop(semafory, sops, 2);
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
