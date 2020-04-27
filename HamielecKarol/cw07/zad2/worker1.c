#include "utils.h"

int semID;
int shmID;

//TODO PRIINT TIMESTAMP I WYCZYSCIC PRINTY

int get_order(){
    int randnum = rand();
    if(randnum < 0) randnum *= -1;
    return randnum%255;
}

void put_order(struct fifo_arr *fifo, int val){
    // pid_time_print();
    // printf("(PID)%d próbuje dodac na tasme 1\n", (int)getpid());
    struct sembuf sops[3];

    //informacja ze zabieram miejsce
    sops[0].sem_num = 2;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    //informacja ze dodaje przedmiot na tasme
    sops[1].sem_num = 4;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    //blokada tablicy fifo
    sops[2].sem_num = 0;
    sops[2].sem_op = -1;
    sops[2].sem_flg = 0;

    sem_operations(semID, sops, 3); // blokada
    // printf("ok jest miejsce\n");
    // sem_operations(semID, &sops[2], 1);

    put_to_fifo(fifo, val);
    int do_przygotowania = fifo->size - get_sem_value(semID, 2);
    int do_wyslania = fifo->size - get_sem_value(semID, 3);
    
    pid_time_print();
    printf("Dodałem liczbę : %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d\n", val, do_przygotowania, do_wyslania);


    sops[2].sem_op = 1;
    sem_operations(semID, &(sops[2]), 1); // odblokowanie  tablicy



}

int main(){
    atexit(exit_fun);
    signal(SIGINT, sig_handler);
    printf("worker1 PID: %d\n", (int)getpid());
    srand(time(NULL));
    key_t sem_key;
    key_t shm_key;
    key_t shm2_key;
    init(&sem_key, &shm_key, &shm2_key);

    semID = create_sem_set(sem_key, 0, 0);
    printf("semafor2: %d\n", get_sem_value(0,2));

    shmID = create_shm(shm_key, 0, 0);
    struct fifo_arr * fifoptr = include_shm(shmID, NULL, 0);
    
    while(1){
        int val = get_order();

        put_order(fifoptr, val);
 
        sleep(rand()%SLEEP_TIME);
    }

    close_shm(fifoptr);
    close_sem(semID);


    
}