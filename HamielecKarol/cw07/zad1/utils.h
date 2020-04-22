#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define ARR_SIZE 5
#define SLEEP_TIME 3

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

struct fifo_arr{
    int val[ARR_SIZE];
    int head;
    int tail;
    int size;
    char empty;
};
/* sem0 - tasma miedzy worker 1 a 2
 * sem1 - tasma miedzy worker 2 a 3
 * sem2 - ilosc wolnego miejsca na tasmie 1
 * sem3 - ilosc wolnego miejsca na tasmie 2
 * sem4 - ilosc przedmiotow na tasmie 1
 * sem5 - ilosc przedmiotow na tasmie 2
 */



void init(key_t * sem_key, key_t *shm_key){
    const char * home = getenv("HOME");
    *sem_key = ftok(home,'a');
    *shm_key = ftok(home, 'b');    
}

int create_sem_set(key_t key, int nsems, int flag){
    int reval = semget(key, nsems, flag);
    if(reval == -1){
        perror("blad tworzenia semafora");
        assert(reval);exit(-1);
    }
    return reval;
}


int sem_operations(int semid, struct sembuf * sops, unsigned nsops){
    int reval = semop(semid, sops, nsops);
    if(reval == -1){
        perror("blad operacji na semaforach");
        assert(reval);exit(-1);
    }
    return reval;

}

int sem_set_val(int semid, int semnum, int val){
    union semun arg;
    arg.val = val;
    int reval = semctl(semid, semnum, SETVAL, arg);
    if(reval == -1){
        perror("blad ustawiania semafora");
        assert(reval);
        exit(-1);
    }
    return reval;
}
int close_sem(int semid){
    union semun arg;
    int reval = semctl(semid, 0, IPC_RMID, arg);
    if(reval == -1){
        perror("blad ustawiania semafora");
        assert(reval);exit(-1);
    }
    return reval;
}
int remove_sem(int semid){
    union semun arg;
    int reval = semctl(semid, 0, IPC_RMID, arg);
    if(reval == -1){
        perror("blad ustawiania semafora");
        assert(reval);exit(-1);
    }
    return reval;
}

int get_sem_value(int semid, int semnum){
    union semun arg;
    int reval = semctl(semid, semnum, GETVAL, arg);
    return reval;

}

int put_to_fifo(struct fifo_arr *fifo, int value){

    if((fifo->head + 1)%fifo->size == fifo->tail){
        //zjadanie ogona
        printf("chcesz zjesc ogon?");
        return (-1);
        
    }
    fifo->head = (fifo->head + 1)%fifo->size;
    if(fifo->empty == 1){
        fifo->empty = 0;
        fifo->head--;
    }
    fifo->val[fifo->head] = value;
    return 0;
}
void pid_time_print(){
    printf("PID:%d T:%d ", (int)getpid(), 0);
}

int get_from_fifo(struct fifo_arr *fifo){

    if(fifo->tail == fifo->head){
        if(fifo->empty == 1){
            printf("pusto przeciez");
            return -1;            
        }else{
            fifo->empty = 1;
            return fifo->val[fifo->tail-1];            
        }

    }
    
    fifo->tail++;
    return fifo->val[fifo->tail-1];            


}

int create_shm(key_t key, size_t size, int shmflg){
    int reval = shmget(key, size, shmflg);
    if(reval == -1){
        perror("blad tworzenia pamieci wspolnej");
        assert(-1);
    }   
    return reval; 
}

void *include_shm(int shmid, const void *shmaddr, int shmflg){
    
    void * ptr = shmat(shmid, shmaddr, shmflg);
    if(ptr == NULL){
        perror("blad dolaczania pamieci");
    }

    return ptr;
}

int close_shm( void * ptr){
    if(shmdt(ptr) == -1){
        perror("blad zamykania zasobu");
        return -1;
    }
    return 0;
}

int remove_shm(int shmid){
    struct shmid_ds buf;
    shmctl(shmid, IPC_RMID, &buf);
    return 0;
}
