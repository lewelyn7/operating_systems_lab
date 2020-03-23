
#define _BSD_SOURCE 
#define _SVID_SOURCE 
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h> 
#include <fcntl.h>
#include <features.h>
#define FILENAME_LEN 64

struct config{
    int my_id;
    char tasks[FILENAME_LEN];
    char filea[FILENAME_LEN];
    char fileb[FILENAME_LEN];
    char filec[FILENAME_LEN];
    int childsq;
    double time_max;
    int mode;
    int col_start;
    int col_end;
};

double calc_elapsed(clock_t start, clock_t end){
    double elap = ((double) (end - start)) / sysconf(_SC_CLK_TCK);
    // printf("elapsed: %f\r\n", elap);
    return elap;
}


struct timestruct{
    struct tms tms_start;
    struct tms tms_end;
    clock_t clock_start;
    clock_t clock_end;
};
struct timestruct time_start(){
    struct timestruct tmstr;
    tmstr.clock_start = times(&tmstr.tms_start);
    return tmstr;
}

struct timestruct time_stop(struct timestruct tmstr){
    tmstr.clock_end = times(&tmstr.tms_end);
    return tmstr;
}
void print_matrix(int ** arr, int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%d ", arr[i][j]);
        }
        printf("\r\n");
    }
}

int find_nth_place(char * buff, int cols, int rows, int idr, int idc ){
    int cnt = 0;

    char * tmp = buff;
    if(idr == 0 && idc == 0){
        return 0;
    }
    
    for(int i = 0; buff[i] != '\0'; i++){
        if(buff[i] == ' ' || buff[i] == '\n'){
            cnt++;
        }
        if(cnt == idr*cols+idc){
            return i+1;
        }
    }

    return NULL;
}

int file_len(FILE * file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}
void paste(char * filename, int rown, int coln, int col_start, int col_end, int ** arr){

    FILE *file = fopen(filename,"r+");

    while(lockf(file->_fileno, F_TLOCK, file_len(file)) == -1){
    // printf("czekam %d\r\n", (int)getpid());

    }
    // printf("blokuje %d\r\n", (int)getpid());

    // lockf(file->_fileno, F_LOCK, file_len(file));
    
    char buff[256];
    char newbuff[256];
    char bigbigbuff1[4096];
    char bigbigbuff2[4096];

    if(file == NULL){
        printf("plik nie otworzony");
        exit(-1);
    }
    for(int i = 0; i < rown; i++){
        for(int j = col_start; j < col_end; j++){

            // //skip to pos
            // char tmp;
            // int space_counter = 0;
            // do{
            //     tmp = getc(file);
            //     if(tmp == ' '){
            //         space_counter++;
            //     }
            //     if(space_counter == col_start){
            //         break;
            //     }
            // }
            // while(tmp != NULL);

            // fprintf(file, )
            rewind(file);
            int readno = fread(bigbigbuff1,1,4096, file);
            bigbigbuff1[readno] = '\0';
            int end = find_nth_place(bigbigbuff1, coln, rown, i, j);
            bigbigbuff1[end] = '\0';
            memcpy(bigbigbuff2, bigbigbuff1, readno);
            char intstr[16];
            sprintf(intstr, "%d", arr[i][j]);
            strcat(bigbigbuff2, intstr);
            strcat(bigbigbuff2, bigbigbuff1+1+end);
            rewind(file);
            fwrite(bigbigbuff2,1, readno + strlen(intstr) -1, file);
            fflush(file);
            // int readno2 = fread(bigbigbuff2, 1, 4096, file);
            // fseek(file, -readno-readno2, SEEK_CUR);
            // char * str2 = find_nth_place(buff, j);
            // *str2 = '\0';
            // str2++;

            // char intstr[16];
            // sprintf(intstr, "%d", arr[i][j]);
            // strcpy(newbuff, buff);
            // strcat(newbuff, intstr);
            // strcat(newbuff, str2);
            // fputs(newbuff, file);
            // fwrite(bigbigbuff2, 1, readno2, file);
            // fseek(file, -readno2 - strlen(newbuff), SEEK_CUR);
            // fflush(file);

        }
    }

    rewind(file);
    bigbigbuff1[fread(bigbigbuff1, 1, 4096,file)] = '\0';
    
    //printf("file:\r\n%s", bigbigbuff1);
    // printf("odblokuje %d\r\n", (int)getpid());
    rewind(file);
    lockf(file->_fileno, F_ULOCK, file_len(file));
    fclose(file);
}

int** load_matrix(FILE *file, int rows, int cols){
    if(file == NULL){
        printf("plik nie otworzony");
        exit(-1);
    }
    fseek(file, 0, SEEK_SET);
    int **res = (int**)calloc(rows, sizeof(int*));
    for(int i = 0; i < rows; i++) res[i] = (int*) calloc(cols, sizeof(int));

    for(int i = 0; i < rows; i++){

        for(int j = 0; j < cols; j++){

            fscanf(file, "%d", &(res[i][j]));

        }
    }
    return(res);
}

//col_start inclusive; col_end exclusive
void child_func(FILE *filea, int arows, int acols, FILE *fileb, int bcols, int col_start, int col_end, char *filec){
    printf("new counting: %d; col_start: %d, col_end: %d\r\n", (int)getpid(), col_start, col_end);
    int **res = (int**)calloc(arows, sizeof(int*));
    for(int i = 0; i < arows; i++) res[i] = (int*) calloc(bcols, sizeof(int));


    int ** a_matrix = load_matrix(filea, arows, acols);
    int ** b_matrix = load_matrix(fileb, acols, bcols);

    int sum = 0;

    for(int i = 0; i < arows; i++){

        for(int j = 0; j < col_end - col_start; j++){

            for(int k = 0; k < acols; k++){

                sum += a_matrix[i][k] * b_matrix[k][j + col_start];

            }
            res[i][j+ col_start] = sum;
            sum = 0;
        }
        
    }
    // printf("result: \r\n");
    // print_matrix(res,arows,bcols);
    paste(filec, arows,bcols, col_start, col_end, res);

    for(int i = 0; i < arows; i++) free(res[i]);
    free(res);

}


void matrix_params(FILE *file, int * coln, int * rown){
    char buff[256];
    char * jmp = buff;
    *coln = 0;
    *rown = 0;
    if(file == NULL){
        printf("plik jest null :(\r\n");
        exit(0);
    }
    fgets(buff, 256, file);
    (*rown)++;

    int space_counter = 0;
    do{
        jmp = strchr(jmp + 1, ' ');
        space_counter++;
    }while(jmp != NULL);

    *coln = space_counter;

    while(fgets(buff, 256, file)) (*rown)++;
    fseek(file, 0, SEEK_SET);

}


int main(int argc, char** argv){

    struct config cfg;

    if(argc != 3){
        printf("bledna ilosc argumentow");
        exit(-1);
    }


    sscanf(argv[1], "%d", &cfg.my_id);
    sscanf(argv[2], "%s", cfg.tasks);


    FILE * tasksf = fopen(cfg.tasks, "r+");
    if(tasksf == NULL){
        printf("cant open tasks file");
    }
    char buff[512];

    int my_job_done = 0;
    int mlp_cnt = 0;
    int all_done;

    while(1){
        while(fgets(buff, 512, tasksf) != NULL){
            if(buff[0] == 'x'){
                all_done = 1;
                continue;
            }
            int tmp;
            sscanf(buff, "%d", &tmp);
            if(tmp != cfg.my_id && my_job_done == 0){
                continue;
            }
            while(flock(tasksf->_fileno,F_LOCK) == -1);
            all_done = 0;
            sscanf(buff, "%d %s %s %s %d %d %lf", &cfg.my_id, cfg.filea, cfg.fileb, cfg.filec, &cfg.col_start, &cfg.col_end, &cfg.time_max);
            
            fseek(tasksf, -strlen(buff), SEEK_CUR);
            buff[0] = 'x';
            fputs(buff, tasksf);
            fflush(tasksf);
            flock(tasksf->_fileno,F_ULOCK);
            FILE * filea = fopen(cfg.filea, "r");
            FILE * fileb = fopen(cfg.fileb, "r");

            int acoln = 0;
            int arown = 0;
            int bcoln = 0;
            int brown = 0;

            matrix_params(filea, &acoln, &arown);
            matrix_params(fileb, &bcoln, &brown);

            if(acoln != brown){
                printf("macierzy nie da sie dot product \r\n");
                exit(-1);
            }

            struct timestruct tmstr;
            tmstr = time_start();

            child_func(filea, arown, acoln, fileb, bcoln, cfg.col_start, cfg.col_end, cfg.filec);   
            mlp_cnt++;        
            // printf("PID %d %d\r\n", (int)getpid(), mlp_cnt);
            tmstr = time_stop(tmstr);
            if(calc_elapsed(tmstr.clock_start, tmstr.clock_end) > cfg.time_max){
                printf("time elapsed: %d %d\r\n", (int)getpid(), mlp_cnt);
                exit(mlp_cnt);
            }
            while(flock(tasksf->_fileno,F_LOCK) == -1);
        }
        rewind(tasksf);
        my_job_done = 1;
        if(all_done){
            exit(mlp_cnt);
        }
    }
    fclose(tasksf);
    exit(mlp_cnt);
    // child_func(filea, arown, acoln, fileb, bcoln, 1, 2, filec, 10);
    // for(int i = 0; i < cfg.childsq; i++){

    //     child_pid = fork();
    //     if(child_pid == 0){
    //         child_func(filea, arown, acoln, fileb, bcoln, (i)*cols_per_proc, (i+1)*cols_per_proc, "c.txt", 10);

    //     }

    // }

    return 0;
}