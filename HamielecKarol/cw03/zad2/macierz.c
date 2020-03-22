
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

char * find_nth_place(char * buff, int n ){
    int cnt = 0;

    char * tmp = buff;
    if(n == 0){
        return tmp;
    }
    do{
        tmp = strchr(tmp+1, ' ');
        cnt++;

    }while(tmp != NULL && cnt != n);

    tmp++;
    return tmp;
}

int file_len(FILE * file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}
void paste(char * filename, int rown, int col_start, int col_end, int ** arr){

    FILE *file = fopen(filename,"r+");
    while(lockf(file->_fileno, F_TLOCK, file_len(file)) == -1){
        printf("czekam ");
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
            int readno = strlen(fgets(buff, 256, file));
            int readno2 = fread(bigbigbuff2, 1, 4096, file);
            fseek(file, -readno-readno2, SEEK_CUR);
            char * str2 = find_nth_place(buff, j);
            *str2 = '\0';
            str2++;

            char intstr[16];
            sprintf(intstr, "%d", arr[i][j]);
            strcpy(newbuff, buff);
            strcat(newbuff, intstr);
            strcat(newbuff, str2);
            fputs(newbuff, file);
            fwrite(bigbigbuff2, 1, readno2, file);
            fseek(file, -readno2 - strlen(newbuff), SEEK_CUR);
            fflush(file);
        }
        fgets(buff, 256, file);
    }

    rewind(file);
    fread(buff, 1, 4096,file);
    printf("file:\r\n%s", buff);
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
    paste(filec, arows, col_start, col_end, res);

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
    fgets(cfg.filea, FILENAME_LEN, file);
    fgets(cfg.fileb, FILENAME_LEN, file);
    fgets(cfg.filec, FILENAME_LEN, file);
    fclose(file);
    
    FILE * filea = fopen("a.txt", "r");
    FILE * fileb = fopen("b.txt", "r");

    int acoln = 0;
    int arown = 0;
    int bcoln = 0;
    int brown = 0;
    
    // lockf(filec->_fileno, F_LOCK, file_len(file));

    matrix_params(filea, &acoln, &arown);
    matrix_params(fileb, &bcoln, &brown);
    if(acoln != brown && arown != bcoln){
        printf("macierzy nie da sie dot product \r\n");
        exit(-1);
    }
    
    int cols_per_proc = bcoln / cfg.childsq;

    int child_pid;
    // child_func(filea, arown, acoln, fileb, bcoln, 1, 2, filec, 10);
    for(int i = 0; i < cfg.childsq; i++){

        child_pid = fork();
        if(child_pid == 0){
            child_func(filea, arown, acoln, fileb, bcoln, (i)*cols_per_proc, (i+1)*cols_per_proc, "c.txt", 10);

        }

    }

    int status = 0;
    do{
        child_pid = wait(&status);
        printf("\r\nchild: %d returned %d\r\n", (int)child_pid, WEXITSTATUS(status));
    }while(child_pid != -1);
    return 0;
}