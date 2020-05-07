#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <pthread.h>
#include <time.h>
#include <math.h>


#define SIGN 1
#define BLOCK 2
#define INTERLEAVED 3

#define BUFFSIZE 26

struct config{
    int threads_num;
    int div_method;
    char in_file[64];
    char out_file[64];
};

struct arka{
    int ** v;
    int h;
    int w;
};

 struct thd_info{
     int thdID;
 };


struct config cfg;
struct arka arr;
int px_quantity[256];

pthread_mutex_t lock;

int read_one(FILE * file){
    if(file == NULL){
        perror("nie przeczytam one bo file is null");
        exit(-1);
    }
    char buff[8];
    strcpy(buff, "");
    char last = 's';
    int i = 0;
    while(!feof(file) && last != ' ' && last != '\n'){

        last = fgetc(file);
        buff[i] = last;
        i++;
    }
    buff[i] = '\0';
    int result;
    sscanf(buff, "%d", &result);

    return result;

}

long time_difference(struct timespec startTime, struct timespec endTime) {
    long time = endTime.tv_sec - startTime.tv_sec;
    time = time * 1000000 + (endTime.tv_nsec - startTime.tv_nsec) / 1000;

    return time;
}

void * calc_sign(void * args){
    struct thd_info *my_info = (struct thd_info* ) args;

    struct timespec startt;
    struct timespec endtt;

    clock_gettime(CLOCK_REALTIME, &startt);

    int range = 255 / cfg.threads_num;
    int start = my_info->thdID * range;
    int end = start + range - 1;
    if(cfg.threads_num == my_info->thdID +1){
        end = 255;
    }

    for(int i = start; i <= end; i++){
        for(int j = 0; j < arr.h; j++){
            for(int k = 0; k < arr.w; k++){
                if(arr.v[j][k] == i){
                    px_quantity[i]++;
                }
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &endtt);
    long elapsed = time_difference(startt, endtt);
    return (void*) elapsed;
}


void * calc_block(void * args){
    struct thd_info *my_info = (struct thd_info* ) args;


    struct timespec startt;
    struct timespec endtt;

    clock_gettime(CLOCK_REALTIME, &startt);

    int start = (my_info->thdID) * ceil(arr.w/cfg.threads_num);
    int end = (my_info->thdID+1) * ceil(arr.w/cfg.threads_num);

    // printf("start: %d end: %d \n", start, end);
    for(int i = start; i < end; i++){
        for(int j = 0; j < arr.h; j++){
            pthread_mutex_lock(&lock); 
            px_quantity[arr.v[j][i]]++;
            pthread_mutex_unlock(&lock); 

        }
    }

    clock_gettime(CLOCK_REALTIME, &endtt);
    long elapsed = time_difference(startt, endtt);
    return (void*) elapsed;

}

void * calc_interleaved(void * args){
    struct thd_info *my_info = (struct thd_info* ) args;


    struct timespec startt;
    struct timespec endtt;

    clock_gettime(CLOCK_REALTIME, &startt);

    int start = (my_info->thdID);
    int step = cfg.threads_num;

    for(int i = start; i < arr.w; i+=step){

        for(int j = 0; j < arr.h; j++){
            pthread_mutex_lock(&lock); 
            px_quantity[arr.v[j][i]]++;
            pthread_mutex_unlock(&lock); 
        }

    }

    clock_gettime(CLOCK_REALTIME, &endtt);
    long elapsed = time_difference(startt, endtt);
    return (void*) elapsed;

}

void sigseg_handler(int n){
    printf("\nSegfault\n");
    exit(-1);
}

void sigint_handler(int n){
    printf("\nSIGINT\n");
}

void write_output(FILE *file){
    if(file == NULL){
        perror("out file is null"); 
    }

    fputs("P2\n", file);
    fputs("256 64\n", file);
    fputs("255\n", file);


    int max = 0;
    for(int i = 0; i < 256; i++)
        if(px_quantity[i] > max)
            max = px_quantity[i];

    int heights[256];
    for(int i = 0; i < 256; i++){
        double height = (double) px_quantity[i] * 64.0 / (double) max;
        heights[i] = (int) height;
    }

    char buff[4096]; //*
    for(int i = 63; i >= 0; i--){
        strcpy(buff, "");
        for(int j = 0; j < 256; j++){
            if(heights[j] >= i){
                strcat(buff, "255 ");
            }else{
                strcat(buff, "0 ");
            }
        }
        fputs(buff, file);
        fputs("\n", file);
    }
    // for(int i = 0; i < 256; i++){
    //     fprintf(file, "%d %d\n", i, px_quantity[i]);
    // }
}

int main(int argc, char ** argv){
    if(argc != 5){
        perror("blad argumentow");
        exit(-1);
    }

    signal(SIGSEGV, sigseg_handler);
    signal(SIGINT, sigint_handler);

    for(int i = 0; i < 256; i++) px_quantity[i] = 0;

    sscanf(argv[1], "%d", &cfg.threads_num);
    if(!strcmp(argv[2], "block")){
        cfg.div_method = BLOCK;
    }else if(!strcmp(argv[2], "sign")){
        cfg.div_method = SIGN;
    }else if(!strcmp(argv[2], "interleaved")){
        cfg.div_method = INTERLEAVED;
    }else{
        perror("blad argumentu 2");
        exit(-1);
    }

    strcpy(cfg.in_file, argv[3]);
    strcpy(cfg.out_file, argv[4]);

    FILE * inf = fopen(cfg.in_file, "r");
    if(inf == NULL){
        perror("blad otwarcia infile");
        exit(-1);
    }

    char buff[BUFFSIZE];
    fgets(buff, BUFFSIZE, inf);

    fgets(buff, BUFFSIZE, inf);
    sscanf(buff, "%d %d", &arr.w, &arr.h);


    fgets(buff, BUFFSIZE, inf);

    arr.v = (int**) calloc(arr.h, sizeof(int*));
    for(int i = 0; i < arr.h; i++) arr.v[i] = (int*) calloc(arr.w, sizeof(int));

    for(int i = 0; i < arr.w*arr.h; i++){
        if(feof(inf)){
            perror("za malo liczb");
            exit(-1);
        }
        int val = read_one(inf);
        arr.v[i/arr.w][i%arr.w] = val;
    }


    //THREADS
    pthread_t *thds = (pthread_t *) calloc(cfg.threads_num, sizeof(pthread_t));


    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        perror("\n mutex init has failed\n"); 
        exit(-1); 
    } 

    for(int i = 0; i < cfg.threads_num; i++){
        struct thd_info *thdarg = calloc(1, sizeof(struct thd_info));
        thdarg->thdID = i;
        void* (*foo) (void *) = &calc_sign;
        if(cfg.div_method == SIGN) 
            foo = &calc_sign;
        else if(cfg.div_method == BLOCK)
            foo = &calc_block;
        else
            foo = &calc_interleaved;

        if(pthread_create(&thds[i], NULL, foo, (void*) thdarg) != 0){
            perror("thread nie utworzyl sie");
        }else{
            printf("tworze thread: %d\n", (int) thds[i]);
        }
    }

    for(int i = 0; i < cfg.threads_num; i++){
        long tt;
        if(pthread_join(thds[i], (void*) &tt) != 0){
            perror("nie mozna pobrac czasu");
        }
        printf("Thread: %d time: %ld \n", (int) thds[i], tt);
    }

    pthread_mutex_destroy(&lock); 
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    printf("czas calkowity: %ld\n", time_difference(start, end));

    FILE * outf = fopen(cfg.out_file, "w");
    write_output(outf);

}