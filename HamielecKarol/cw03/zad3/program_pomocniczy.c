
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
int compare_matrices(int ** arr1, int ** arr2, int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){

            if(arr1[i][j] != arr2[i][j]){
                return -1;
            }

        }
    }
    return 0;
}
//col_start inclusive; col_end exclusive
void child_func(FILE *filea,  FILE *fileb, FILE * filec){
    int arows;
    int acols;
    int brows;
    int bcols;
    matrix_params(filea, &acols, &arows);
    matrix_params(fileb, &bcols, &brows);
    

    int **res = (int**)calloc(arows, sizeof(int*));
    for(int i = 0; i < arows; i++) res[i] = (int*) calloc(bcols, sizeof(int));


    int ** a_matrix = load_matrix(filea, arows, acols);
    int ** b_matrix = load_matrix(fileb, brows, bcols);
    int ** c_matrix = load_matrix(filec, arows, bcols);

    int sum = 0;

    for(int i = 0; i < arows; i++){

        for(int j = 0; j < bcols; j++){

            for(int k = 0; k < acols; k++){

                sum += a_matrix[i][k] * b_matrix[k][j];

            }
            res[i][j] = sum;
            sum = 0;
        }
    
    }

    printf("comparing...");
    if(compare_matrices(res, c_matrix, arows, bcols) == 0){
        printf("good\r\n");
    }else{
        printf("bad\r\n");
        printf("result: \r\n");
        print_matrix(res,arows,bcols);
        printf("loaded: \r\n");
        print_matrix(c_matrix,arows,bcols);
    }
    for(int i = 0; i < arows; i++) free(res[i]);
    free(res);

}



FILE *listaf;

void make_filename(char * buf, int *i, int lista_switch){
    char * alphabet = "abcdefghijklmnopqrstuvwxyz";
    strcpy(buf, "x");
    buf[0] = alphabet[*i];
    strcat(buf, ".txt");
    (*i)++;
    if(lista_switch == 1){
        fputs(buf, listaf);
        fputs("\n", listaf);        
    }

    return;

}
void write_matrix(FILE * matrixaf, int arows, int acols){

    for(int j = 0; j < arows; j++){

    for(int k = 0; k < acols-1; k++){

        char buff[8];
        sprintf(buff, "%d", rand()%(200)-100);
        fprintf(matrixaf, buff);
        fprintf(matrixaf, " ");

    }
    char buff[8];
    sprintf(buff, "%d", rand()%(200)-100);
    fprintf(matrixaf, buff);
    fprintf(matrixaf, "\n");
    }
}


int main(int argc, char** argv){

    if(argc != 5){
        printf("bledna ilosc argumentow");
        exit(0);
    }
    int liczba_plikow;
    int min;
    int max;

    sscanf(argv[1], "%d", &liczba_plikow);
    sscanf(argv[2], "%d", &min);
    sscanf(argv[3], "%d", &max);
    int alphabet_jumper = 0;
    srand(time(0)); 
    char filenamebuf[64];

    if(!strcmp(argv[4], "create")){
        listaf = fopen("lista.txt", "w");
        for(int i = 0; i < liczba_plikow; i++){
            int arows = rand()%(max-min)+min;
            int browsacols = rand()%(max-min)+min;
            int bcols = rand()%(max-min)+min;
            make_filename(filenamebuf, &alphabet_jumper ,1);
            FILE * matrixaf = fopen(filenamebuf,"w");
            make_filename(filenamebuf, &alphabet_jumper ,1);
            FILE * matrixbf = fopen(filenamebuf,"w");

            write_matrix(matrixaf, arows, browsacols);
            write_matrix(matrixbf, browsacols, bcols);
            fclose(matrixbf);
            fclose(matrixaf);
            make_filename(filenamebuf, &alphabet_jumper ,1);
            fclose(fopen(filenamebuf, "w"));

        }
    }

    alphabet_jumper = 0;
    if(!strcmp(argv[4], "check")){
        for(int i = 0; i < liczba_plikow; i++){
            int arows = rand()%(max-min)+min;
            int browsacols = rand()%(max-min)+min;
            int bcols = rand()%(max-min)+min;
            make_filename(filenamebuf, &alphabet_jumper ,0);
            FILE * matrixaf = fopen(filenamebuf,"r");
            make_filename(filenamebuf, &alphabet_jumper ,0);
            FILE * matrixbf = fopen(filenamebuf,"r");
            make_filename(filenamebuf, &alphabet_jumper ,0);
            FILE * matrixcf = fopen(filenamebuf,"r");

            child_func(matrixaf, matrixbf, matrixcf);

            fclose(matrixbf);
            fclose(matrixaf);
            fclose(matrixcf);


        }   
    }
    return 0;
}