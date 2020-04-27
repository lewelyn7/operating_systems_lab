#include "utils.h"
#include <fcntl.h>
#include <sys/wait.h>


int semID;
int shmID;
int shmID2;


void exit_func(void){
    printf("\33[31m aaa zabili mnie %d \n \33[37m", (int)getpid());

    if(remove_sem(semID) == -1 ){
        perror("\033[31m blad zamykania semaforow\n");
    }
    if(remove_shm(shmID) == -1){
        perror("\033[31m blad zamykania pamieci wspoldzielonej\n");
    }
    if(remove_shm(shmID2) == -1){
        perror("\033[31m blad zamykania pamieci wspoldzielonej\n");
    }
}

int main(int argc, char ** argv){
    printf("mother PID: %d\n", (int)getpid());
    signal(SIGINT, sig_handler);
    atexit(exit_func);

    if(argc != 4){
        perror("blad argumentow");
        exit(-1);
    }

    key_t sem_key;
    key_t shm_key;
    key_t shm2_key;
    init(&sem_key, &shm_key, &shm2_key);


    semID = create_sem_set(sem_key, 6, IPC_CREAT |  S_IRWXU);
    shmID = create_shm(shm_key, sizeof(struct fifo_arr), IPC_CREAT |  S_IRWXU);
    shmID2 = create_shm(shm2_key, sizeof(struct fifo_arr), IPC_CREAT |  S_IRWXU);
    struct fifo_arr *fifoptr = include_shm(shmID, NULL, 0);
    struct fifo_arr *fifoptr2 = include_shm(shmID2, NULL, 0);

    fifoptr->size = ARR_SIZE;
    fifoptr->head = -1;
    fifoptr->tail = -1;
    
    fifoptr2->size = ARR_SIZE;
    fifoptr2->head = -1;
    fifoptr2->tail = -1;


    sem_set_val(semID, 0, 1);
    sem_set_val(semID, 1, 1);
    sem_set_val(semID, 2, fifoptr->size);
    sem_set_val(semID, 3, fifoptr2->size);
    sem_set_val(semID, 4, 0);
    sem_set_val(semID, 5, 0);
    pause();
    int work1num;
    int work2num;
    int work3num;

    sscanf(argv[1], "%d",&work1num);
    sscanf(argv[2], "%d", &work2num);
    sscanf(argv[3], "%d", &work3num);


    for(int i = 0; i < work1num; i++){
        printf("tworze worker1 nr %d\n", i);
        int child = fork();
        if(child == 0){
            execl("./worker1.out","./worker1.out", NULL);
            exit(-1);
        }

    }

    for(int i = 0; i < work2num; i++){
        printf("tworze worker2 nr %d\n", i);
        int child = fork();
        if(child == 0){
            execl("./worker2.out","./worker2.out", NULL);
            exit(-1);
        }

    }

    for(int i = 0; i < work3num; i++){
        printf("tworze worker3 nr %d\n", i);
        int child = fork();
        if(child == 0){
            execl("./worker3.out","./worker3.out", NULL);
            exit(-1);
            
        }

    }
    int exit_stat;
    do{
        exit_stat = wait(NULL);
        printf("exit status: %d\n", exit_stat);

    }while(exit_stat != -1);

    remove_sem(semID);
    remove_shm(shmID);


}