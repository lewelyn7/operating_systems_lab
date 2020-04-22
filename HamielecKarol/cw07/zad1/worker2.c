#include "utils.h"

int semID;
int shmID;


int get_order(struct fifo_arr *fifo){
    
    printf("próbuje odebrac z tasmy 1\n");

    int return_value;

    struct sembuf sops[3];
    
    //zwiekszenie ilosci miejsca na tasmie 1
    sops[0].sem_num = 2;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;

    //zmniejszenie ilosci paczek na tasmie 1
    sops[1].sem_num = 4;
    sops[1].sem_op = -1;
    sops[1].sem_flg = 0;


    //blokada fifo tasmy 1
    sops[2].sem_num = 0;
    sops[2].sem_op = -1;
    sops[2].sem_flg = 0;

    sem_operations(semID, sops, 2); // blokada
    // printf("ok jest miejsce\n");
    sem_operations(semID, &sops[2], 1);
    // printf("przetwarzam...\n");
    return_value = get_from_fifo(fifo);
    printf("odebralem z tasmy 1: %d\n", return_value);

    sops[2].sem_op = 1;
    sem_operations(semID, &sops[2], 1); // odblokowanie

    return return_value;

}


void put_order(struct fifo_arr *fifo, int val){
    pid_time_print();
    printf("próbuje dodac na tasme 2\n");

    struct sembuf sops[3];

    //informacja ze zabieram miejsce
    sops[0].sem_num = 3;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    //informacja ze dodaje przedmiot na tasme
    sops[1].sem_num = 5;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    //blokada tablicy fifo
    sops[2].sem_num = 1;
    sops[2].sem_op = -1;
    sops[2].sem_flg = 0;
    sem_operations(semID, sops, 3); // informacja ze zabieram miejsce

    put_to_fifo(fifo, val);
    int do_przygotowania = get_sem_value(semID, 4);
    int do_wyslania = get_sem_value(semID, 5);
    pid_time_print();
    printf("Przygotowałem liczbę : %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d\n", val, do_przygotowania, do_wyslania);


    sops[2].sem_op = 1;
    sem_operations(semID, &sops[2], 1); // odblokowanie  tablicy



}

int main(){

    printf("worker2 PID: %d\n", (int)getpid());

    key_t sem_key;
    key_t shm_key;
    init(&sem_key, &shm_key);

    
    semID = create_sem_set(sem_key, 0, 0);
    
    shmID = create_shm(shm_key, 0, 0);
    struct fifo_arr * fifoptr = include_shm(shmID, NULL, 0);
    
    while(1){
        int val = get_order(fifoptr);

        put_order(fifoptr, val*2);
 
        sleep(rand()%SLEEP_TIME);

    }
    
    close_shm(fifoptr);
    close_sem(semID);


    
}