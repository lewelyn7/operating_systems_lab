#include "utils.h"
#include <fcntl.h>
#include <sys/wait.h>


int semID;
int shmID;

int main(int argc, char ** argv){
    printf("mother PID: %d\n", (int)getpid());

    if(argc != 4){
        perror("blad argumentow");
        exit(-1);
    }

    key_t sem_key;
    key_t shm_key;
    init(&sem_key, &shm_key);


    semID = create_sem_set(sem_key, 6, IPC_CREAT |  S_IRWXU);
    shmID = create_shm(shm_key, sizeof(struct fifo_arr), IPC_CREAT |  S_IRWXU);
    struct fifo_arr *fifoptr = include_shm(shmID, NULL, 0);
    fifoptr->size = ARR_SIZE;
    fifoptr->head = 0;
    fifoptr->tail = 0;

    sem_set_val(semID, 0, 1);
    sem_set_val(semID, 1, 1);
    sem_set_val(semID, 2, fifoptr->size);
    sem_set_val(semID, 3, fifoptr->size);
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