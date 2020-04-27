#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h> 
 #include <sys/mman.h>
 #include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/types.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>




#define ARR_SIZE 5
#define SLEEP_TIME 3

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
void exit_fun(){
    printf("\33[31m aaa zabili mnie %d \n \33[37m", (int)getpid());
}
void sig_handler(int num){
    exit(0);
}

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
};
/* sem0 - tasma miedzy worker 1 a 2
 * sem1 - tasma miedzy worker 2 a 3
 * sem2 - ilosc wolnego miejsca na tasmie 1
 * sem3 - ilosc wolnego miejsca na tasmie 2
 * sem4 - ilosc przedmiotow na tasmie 1
 * sem5 - ilosc przedmiotow na tasmie 2
 */

sem_t* semafory[10];
key_t semkeycommon;

void init(key_t * sem_key, key_t *shm_key, key_t *shm2_key){
    const char * home = getenv("HOME");
    *sem_key = ftok(home,'a');
    *shm_key = ftok(home, 'b');    
    *shm2_key = ftok(home, 'c');    
}

int create_sem_set(key_t key, int nsems, int flag){
    int reval = 0;
    char keystr[20];
    char finalstr[20];
    semkeycommon = key;
    sprintf(keystr, "%d", key);
    char semnum[2];
    sprintf(semnum, "%d", nsems);

    int flags = 0;
    if(flag == 0){
        flags = O_RDWR | O_EXCL;
    }else{
        flags = O_RDWR | O_CREAT;
    }

    for(int i = 0; i < 6; i++){
        strcpy(finalstr, "/");
        sprintf(keystr, "%d", key);
        sprintf(semnum, "%d", i);
        strcat(keystr, semnum);
        strcat(finalstr, keystr);
        semafory[i] = sem_open(finalstr, flags, 0600, 0);
        if(semafory[i] == SEM_FAILED){
            perror("blad tworzenia semafora");
            exit(-1);            
        }
    }

    return reval;
}
int get_sem_value(int semid, int semnum){
    int val;
    int err;
    err = sem_getvalue(semafory[semnum], &val);
    if(err == -1){
        perror("blad getowania");
        exit(-1);
    }
    return val;

}


int sem_operations(int semid, struct sembuf * sops, unsigned nsops){
    
    int state = 0;
    do{
    state = 0;
    for(unsigned i = 0; i < nsops; i++){
        if(sops[i].sem_op == -1){
            state = get_sem_value(0,sops[i].sem_num);
            if(state == 0) break;
        }
    }
    }while(state == 0 && errno == EAGAIN);

    for(unsigned i = 0; i < nsops; i++){
        if(sops[i].sem_op == 1){
            state = sem_post(semafory[sops[i].sem_num]);
        }else{
            state = sem_wait(semafory[sops[i].sem_num]);
        }
    }

    if(state == -1 && errno == EAGAIN){
        perror("blad operacji na semaforach ");
        exit(-1);
    }


    return 0;

}

int sem_set_val(int semid, int semnum, int val){
    int value;
    int err = sem_getvalue(semafory[semnum], &value);
    
    while(value != val){
       err = sem_post(semafory[semnum]);
       if(err == -1){
           perror("blad posta");
           exit(-1);
       }
       err = sem_getvalue(semafory[semnum], &value);
        if(err == -1){
            perror("blad getvalue");
            exit(-1);
       }
    }
    return 0;
}
int close_sem(int semid){
    for(int i = 0; i < 6; i++){
        if(sem_close(semafory[i]) == -1){
            perror("sem close err");
            exit(-1);
        }
    }
    return 0;
}
int remove_sem(int semid){
    int reval = 0;
    char keystr[20];
    char finalstr[25];
    sprintf(keystr, "%d", semkeycommon);
    char semnum[2];
    for(int i = 0; i < 6; i++){
        strcpy(finalstr, "/");
        sprintf(keystr, "%d", semkeycommon);
        sprintf(semnum, "%d", i);
        strcat(keystr, semnum);
        strcat(finalstr, keystr);
        reval = sem_unlink(finalstr);
        if(reval == -1){
            perror("blad tworzenia semafora");
            exit(-1);            
        }
    }

    return reval;
}



int put_to_fifo(struct fifo_arr *fifo, int value){

    if((fifo->head + 1)%fifo->size == fifo->tail){
        //zjadanie ogona
        // printf("\033[31m chcesz zjesc ogon H:%d T:%d?\n \033[37m", fifo->head, fifo->tail);
        return (-1);
        
    }
    if(fifo->tail == -1){
        fifo->tail++;
    }
    fifo->head = (fifo->head + 1)%fifo->size;
    fifo->val[fifo->head] = value;
    // printf("wlozono do kolejki, teraz H:%d T:%d ", fifo->head, fifo->tail);

    return 0;
}
void pid_time_print(){
    printf("PID:%d T:%lld ", (int)getpid(), current_timestamp());
    // printf(" %d ", get_sem_value(0, 0));
    // printf(" %d ", get_sem_value(0, 1));
    // printf(" %d ", get_sem_value(0, 2));
    // printf(" %d ", get_sem_value(0, 3));
    // printf(" %d ", get_sem_value(0, 4));
    // printf(" %d ", get_sem_value(0, 5));
}

int get_from_fifo(struct fifo_arr *fifo){

    if(fifo->head == -1){
        // printf("\033[31m kolejka pusta \n \033[37m");
        return (-1);       
    }
    int val = fifo->val[fifo->tail];
    fifo->tail = (fifo->tail+1)%fifo->size;
    if(fifo->tail == (fifo->head+1)%fifo->size){
        // printf("\033[31m kolejka opróźniona \n \033[37m");
        fifo->head = -1;
        fifo->tail = -1;
    }
    // printf("zabrano z kolejki, teraz H:%d T:%d", fifo->head, fifo->tail);
    return val;

}

int create_shm(key_t key, size_t size, int shmflg){
    char finalstr[25];
    char keystr[20];
    strcpy(finalstr, "/");
    sprintf(keystr, "%d", key);
    strcat(finalstr, keystr);

    int flags = 0;
    if(shmflg == 0){
        flags = O_RDWR | O_EXCL;
    }else{
        flags = O_RDWR | O_CREAT;
    }

    int reval = shm_open(finalstr, flags, 0600);
    if(reval == -1){
        perror("blad tworzenia pamieci wspolnej");
        assert(-1);
    }
    if(ftruncate(reval, (off_t) sizeof(struct fifo_arr)) == -1){
        perror("blad truncate");
        exit(-1);
    }
    return reval; 
}

void *include_shm(int shmid, const void *shmaddr, int shmflg){
    
    void * ptr = mmap(NULL, sizeof(struct fifo_arr), PROT_READ | PROT_WRITE, MAP_SHARED,shmid, 0);
    if(ptr == (void *)-1){
        perror("blad dolaczania pamieci");
    }

    return ptr;
}

int close_shm( void * ptr){
    if(munmap(ptr, sizeof(struct fifo_arr)) == -1){
        perror("blad zamykania zasobu");
        return -1;
    }
    return 0;
}

int remove_shm(int shmid){
    const char * home = getenv("HOME");

    close(shmid);

    char finalstr[25];
    char keystr[20];
    strcpy(finalstr, "/");
    sprintf(keystr, "%d", ftok(home, 'b'));
    strcat(finalstr, keystr);
    shm_unlink(finalstr);


    strcpy(finalstr, "/");
    sprintf(keystr, "%d", ftok(home, 'c'));
    strcat(finalstr, keystr);
    return 0;
}
