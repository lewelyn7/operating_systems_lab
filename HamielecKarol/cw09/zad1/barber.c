#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <pthread.h>
#include <semaphore.h>

int WAITINGROOMSIZE;

struct config{
    int chairs;
    int clients;
};
struct fifo_arr{
    int *val;
    int head;
    int tail;
    int size;
    int now;
};
struct fifo_arr fifo;
 struct thd_info{
     int thdID;
 };

struct config cfg;
pthread_mutex_t lock;
pthread_cond_t cond   = PTHREAD_COND_INITIALIZER;;

int get_from_fifo(struct fifo_arr *fifo){

    if(fifo->head == -1){
        printf("\033[31m kolejka pusta \n \033[37m");
        return (-1);       
    }
    int val = fifo->val[fifo->tail];
    fifo->tail = (fifo->tail+1)%fifo->size;
    if(fifo->tail == (fifo->head+1)%fifo->size){
        // printf("\033[31m kolejka opróźniona \n \033[37m");
        fifo->head = -1;
        fifo->tail = -1;
    }
    fifo->now--;
    // printf("zabrano z kolejki, teraz H:%d T:%d", fifo->head, fifo->tail);
    return val;

}
int put_to_fifo(struct fifo_arr *fifo, int value){

    if((fifo->head + 1)%fifo->size == fifo->tail){
        //zjadanie ogona
        printf("\033[31m chcesz zjesc ogon H:%d T:%d?\n \033[37m", fifo->head, fifo->tail);
        return (-1);
        
    }
    if(fifo->tail == -1){
        fifo->tail++;
    }
    fifo->head = (fifo->head + 1)%fifo->size;
    fifo->val[fifo->head] = value;
    fifo->now++;
    // printf("wlozono do kolejki, teraz H:%d T:%d ", fifo->head, fifo->tail);

    return 0;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;


    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);

    return res;
}

void* barber_func(void * args){
    while(1){
        pthread_mutex_lock(&lock);
        while(fifo.now == 0){
            printf("Golibroda: ide spac \n");
            pthread_cond_wait(&cond, &lock);
        }

        int client_id = get_from_fifo(&fifo);
        pthread_mutex_unlock(&lock);

        printf("Golibroda: czeka %d klientow, gole klienta: %d\n", fifo.now, client_id);
        msleep(rand()%3000+1000);

    }
    return 0;
}

void* client_func(void * args){
    struct thd_info *my_info = (struct thd_info* ) args;
    pthread_mutex_lock(&lock);
    while(fifo.now == WAITINGROOMSIZE){
        pthread_mutex_unlock(&lock);
        printf("zajete: ID%d\n", my_info->thdID);
        msleep(rand()%4000);
        pthread_mutex_lock(&lock);
    }
    if(fifo.now == 0){
        printf("Budze golibrode ID%d\n", my_info->thdID);
        pthread_cond_broadcast(&cond);
    }else{
        printf("poczekalnia, wolne miejsca: %d, ID%d\n", WAITINGROOMSIZE - fifo.now-1, my_info->thdID);
    }

    put_to_fifo(&fifo, my_info->thdID);
    pthread_mutex_unlock(&lock);
    return 0;
}

int main(int argc, char ** argv){
    if(argc != 3){
        perror("blad arguementow");
        exit(-1);
    }

    sscanf(argv[1], "%d", &cfg.chairs);
    sscanf(argv[2], "%d", &cfg.clients);

    srand(time(NULL));

    //initialize fifo
    WAITINGROOMSIZE = cfg.chairs;
    fifo.val = (int*)calloc(WAITINGROOMSIZE, sizeof(int));
    fifo.size = WAITINGROOMSIZE;
    fifo.head = -1;
    fifo.tail = -1;
    fifo.now = 0;

    //create mutex
    if(pthread_mutex_init (&lock, NULL) != 0){
        perror("blad tworzenia mutexa\n");
        exit(-1);
    }

    //and condition variable
    

    pthread_t *clients_ds = (pthread_t *) calloc(cfg.clients, sizeof(pthread_t));
    pthread_t barber_id;

    //create barber
    if(pthread_create(&barber_id, NULL, barber_func, (void*) NULL) != 0){
        perror("golibroda nie utworzyl sie");
    }else{
        printf("tworze golibrode: %d\n", (int) barber_id);
    }

    for(int i = 0; i < cfg.clients; i++){
        msleep(rand()%2000);
        struct thd_info *thdarg = calloc(1, sizeof(struct thd_info));
        thdarg->thdID = i;
        if(pthread_create(&clients_ds[i], NULL, client_func, (void*) thdarg) != 0){
            perror("client thread nie utworzyl sie\n");
        }else{
            //printf("tworze client thread: %d ID: %d\n", (int) clients_ds[i], i);
        }
    }

    long tt;
    pthread_join(barber_id, (void*) &tt);
    for(int i = 0; i < cfg.clients; i++){
        long tt;
        if(pthread_join(clients_ds[i], (void*) &tt) != 0){
            perror("nie mozna pobrac czasu");
        }
        printf("Thread: %d time: %ld \n", (int) clients_ds[i], tt);
    }

    free(fifo.val);
}