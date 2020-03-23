
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
#include <unistd.h> 
#include <fcntl.h>
#include <features.h>
#define FILENAME_LEN 64

struct config{
    char list[FILENAME_LEN];
    char filea[FILENAME_LEN];
    char fileb[FILENAME_LEN];
    char filec[FILENAME_LEN];
    int childsq;
    float time_max;
    int mode;
};

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
    printf("czekam %d\r\n", (int)getpid());

    }
    printf("blokuje %d\r\n", (int)getpid());

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
    
    printf("file:\r\n%s", bigbigbuff1);
    printf("odblokuje %d\r\n", (int)getpid());
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
void child_func(FILE *filea, int arows, int acols, FILE *fileb, int bcols, int col_start, int col_end, char *filec, float time_max){
    clock_t time_start = time(NULL);
    printf("new proccess: %d; col_start: %d, col_end: %d\r\n", (int)getpid(), col_start, col_end);
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
    printf("result: \r\n");
    print_matrix(res,arows,bcols);
    paste(filec, arows,bcols, col_start, col_end, res);

    for(int i = 0; i < bcols; i++) free(res[i]);
    free(res);

    exit(1);
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

    if(argc != 5){
        printf("bledna ilosc argumentow");
        exit(0);
    }

    sscanf(argv[1], "%s", cfg.list);
    sscanf(argv[2], "%d", &cfg.childsq);
    sscanf(argv[3], "%f", &cfg.time_max);
    sscanf(argv[4], "%d", &cfg.mode);

    FILE * file = fopen(cfg.list, "r");
    if(file == NULL){
        printf("nie ma listy \r\n");
        exit(0);
    }

    FILE * tasksf = fopen("tmp_tasks.txt", "w");
    if(tasksf == NULL){
        printf("cant open tasks file");
    }
    while(fgets(cfg.filea, FILENAME_LEN, file) != NULL){
        fgets(cfg.fileb, FILENAME_LEN, file);
        fgets(cfg.filec, FILENAME_LEN, file);

        if(cfg.filea[strlen(cfg.filea)-1] == '\n') cfg.filea[strlen(cfg.filea)-1] = '\0';
        if(cfg.fileb[strlen(cfg.fileb)-1] == '\n') cfg.fileb[strlen(cfg.fileb)-1] = '\0';
        if(cfg.filec[strlen(cfg.filec)-1] == '\n') cfg.filec[strlen(cfg.filec)-1] = '\0';
        
        FILE * filea = fopen(cfg.filea, "r");
        FILE * fileb = fopen(cfg.fileb, "r");
        FILE * filec = fopen(cfg.filec, "w");
        int acoln = 0;
        int arown = 0;
        int bcoln = 0;
        int brown = 0;



        matrix_params(filea, &acoln, &arown);
        matrix_params(fileb, &bcoln, &brown);

        for(int i = 0; i < arown; i++){
            for(int j = 0; j < bcoln-1; j++){
                fputs("- ", filec);
            }
            fputs("-\n", filec);
        }


        fclose(filea);
        fclose(fileb);
        fclose(filec);
        if(acoln != brown){
            printf("macierzy nie da sie dot product \r\n");
            exit(-1);
        }
        
        int cols_per_proc = bcoln / cfg.childsq;
        int last_extension = bcoln%cfg.childsq;

        for(int i = 0; i < cfg.childsq-1; i++){
            //numer dziecka; plik a; plik b; plik c; col_start inclusive; col_end exclusive; time_max;
            fprintf(tasksf, "%d %s %s %s %d %d %f\n", i, cfg.filea, cfg.fileb, cfg.filec, (i)*cols_per_proc, (i+1)*cols_per_proc, cfg.time_max);
        }
        //last
        fprintf(tasksf, "%d %s %s %s %d %d %f\n", cfg.childsq-1, cfg.filea, cfg.fileb, cfg.filec, (cfg.childsq-1)*cols_per_proc, (cfg.childsq-1+1)*cols_per_proc+last_extension, cfg.time_max);


    }
    fclose(file);
    fclose(tasksf);

    int child_pid;

    for(int i = 0; i < cfg.childsq; i++){

        child_pid = fork();
        if(child_pid == 0){
            char to_str[8];
            sprintf(to_str, "%d", i);
            int stat;
            execl("./child", "./child", to_str, "tmp_tasks.txt", NULL);

        }

    }

    int status = 0;
    do{
        child_pid = wait(&status);
        printf("\r\nchild: %d returned %d\r\n", (int)child_pid, WEXITSTATUS(status));
    }while(child_pid != -1);
    return 0;
}