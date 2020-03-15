#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>


#define DEBUG 0

#define FILENAME_LENGTH 64

FILE * reportfile;

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
double calc_elapsed(clock_t start, clock_t end){
    return ((double) (end - start)) / sysconf(_SC_CLK_TCK);
}


void write_report(FILE * fptr, char* operation, struct timestruct tmstr){
    if(fptr == NULL){
        printf("cannot open report file\r\n");
    }
    fprintf(fptr,"\r\noperation: %s\r\n", operation);
    fprintf(fptr,"real time: %f \r\n", calc_elapsed(tmstr.clock_start, tmstr.clock_end));
    fprintf(fptr,"user time: %f \r\n", calc_elapsed(tmstr.tms_start.tms_utime, tmstr.tms_end.tms_utime));
    fprintf(fptr,"system time: %f \r\n\r\n", calc_elapsed(tmstr.tms_start.tms_stime, tmstr.tms_end.tms_stime));

    if(DEBUG == 1){
        printf("\r\noperation: %s\r\n", operation);
        printf("real time: %F\r\n", calc_elapsed(tmstr.clock_start, tmstr.clock_end));
        printf("user time: %F\r\n", calc_elapsed(tmstr.tms_start.tms_utime, tmstr.tms_end.tms_utime));
        printf("system time: %F\r\n\r\n", calc_elapsed(tmstr.tms_start.tms_stime, tmstr.tms_end.tms_stime));
    }
}


char* getter(FILE * file,int buffer_size, int idx){
    char * buffer = (char*) calloc(buffer_size, sizeof(char));
    fseek(file, buffer_size*idx, SEEK_SET);
    fgets(buffer, buffer_size,file);
    return buffer;
}

char* getter_sys(int file,int buffer_size, int idx){
    char * buffer = (char*) calloc(buffer_size, sizeof(char));
    lseek(file, buffer_size*idx, SEEK_SET);
    read(file, buffer, buffer_size);
    return buffer;
}

void swap_strings(FILE * file, int id1, int id2, int buffer_size){
    char * tmp1 = (char*) calloc(buffer_size, sizeof(char));
    char * tmp2 = (char*) calloc(buffer_size, sizeof(char));
    
    fseek(file, buffer_size*id1 ,SEEK_SET);
    fread(tmp1,sizeof(char), buffer_size,file);

    fseek(file, buffer_size*id2, SEEK_SET);
    fread(tmp2,sizeof(char), buffer_size, file);

    fseek(file, -buffer_size, SEEK_CUR);
    fwrite(tmp1, sizeof(char), buffer_size, file);

    fseek(file, buffer_size*id1, SEEK_SET);
    fwrite(tmp2, sizeof(char), buffer_size, file);

    fflush(file);

    free(tmp1);
    free(tmp2);
}

void swap_strings_sys(int file, int id1, int id2, int buffer_size){
    char * tmp1 = (char*) calloc(buffer_size, sizeof(char));
    char * tmp2 = (char*) calloc(buffer_size, sizeof(char));
    
    lseek(file, buffer_size*id1 ,SEEK_SET);
    read(file, tmp1, buffer_size);

    lseek(file, buffer_size*id2, SEEK_SET);
    read(file, tmp2, buffer_size);

    lseek(file, -buffer_size, SEEK_CUR);
    write(file,tmp1, buffer_size);

    lseek(file, buffer_size*id1, SEEK_SET);
    write(file, tmp2, buffer_size);


    free(tmp1);
    free(tmp2);
}

int compare_strings(char* str1, char* str2){
    int res = strcmp(str1, str2);
    free(str1);
    free(str2);
    return res;
}


void quicksort(FILE * file, int first, int last, int buffer_size)
{
    int i, j, pivot;
    if(first < last)
    {
        pivot = first;
        i = first;
        j = last;
        while (i < j)
        {
            while (compare_strings(getter(file,buffer_size, i),getter(file,buffer_size, pivot)) <= 0 && i < last)
                i++;
            while (compare_strings(getter(file,buffer_size,j), getter(file,buffer_size,pivot)) > 0)
                j--;
            if (i < j)
            {
                swap_strings(file, i, j, buffer_size);
            }
        }
        swap_strings(file, pivot, j, buffer_size);
        quicksort(file, first, j - 1, buffer_size);
        quicksort(file, j + 1, last, buffer_size);
    }
}

void quicksort_sys(int file, int first, int last, int buffer_size)
{
    int i, j, pivot;
    if(first < last)
    {
        pivot = first;
        i = first;
        j = last;
        while (i < j)
        {
            while (compare_strings(getter_sys(file,buffer_size, i),getter_sys(file,buffer_size, pivot)) <= 0 && i < last)
                i++;
            while (compare_strings(getter_sys(file,buffer_size,j), getter_sys(file,buffer_size,pivot)) > 0)
                j--;
            if (i < j)
            {
                swap_strings_sys(file, i, j, buffer_size);
            }
        }
        swap_strings_sys(file, pivot, j, buffer_size);
        quicksort_sys(file, first, j - 1, buffer_size);
        quicksort_sys(file, j + 1, last, buffer_size);
    }
}

void generate_lib(char * fname1, int buffer_size, int record_cnt){
    
        struct timestruct tmstr;
        tmstr = time_start();

 
        FILE * file1;
        char * buffer = (char*) calloc(buffer_size, sizeof(char));
        char byte_tmp;

        file1 = fopen(fname1, "w");
        FILE * rand_handler= fopen("/dev/random", "r");

        for(int j = 0; j < record_cnt; j++){
            for(int i = 0; i < buffer_size; i++){
                fread(&byte_tmp, sizeof(char), 1, rand_handler);
                if(byte_tmp == -128) byte_tmp++;
                if(byte_tmp < 0) byte_tmp *= (-1);
                byte_tmp = (byte_tmp%26) + 65;
                buffer[i] = byte_tmp;
            }
            // buffer[buffer_size-3] = '\r';
            buffer[buffer_size-2] = '\r';
            buffer[buffer_size-1] = '\n';

            fwrite(buffer, sizeof(char), buffer_size, file1);
        }

        free(buffer);
        fclose(file1);

        tmstr = time_stop(tmstr);
        char message[128];
        sprintf(message, "generate lib bufsize: %d record_cnt: %d", buffer_size, record_cnt);
        write_report(reportfile, message, tmstr);
}

void generate_sys(char * fname1, int buffer_size, int record_cnt){

        struct timestruct tmstr;
        tmstr = time_start();
    
        int file1;
        char * buffer = (char*) calloc(buffer_size, sizeof(char));
        char byte_tmp;



        file1 = open(fname1, O_WRONLY|O_CREAT);
        int rand_handler= open("/dev/random", O_RDONLY);

        for(int j = 0; j < record_cnt; j++){
            for(int i = 0; i < buffer_size; i++){
                read(rand_handler, &byte_tmp, 1);
                if(byte_tmp == -128) byte_tmp++;
                if(byte_tmp < 0) byte_tmp *= (-1);
                byte_tmp = (byte_tmp%26) + 65;
                buffer[i] = byte_tmp;
            }
            // buffer[buffer_size-3] = '\r';
            buffer[buffer_size-2] = '\r';
            buffer[buffer_size-1] = '\n';

            write(file1, buffer, buffer_size);
        }

        free(buffer);
        close(file1);

        tmstr = time_stop(tmstr);
        char message[128];
        sprintf(message, "generate sys bufsize: %d record_cnt: %d", buffer_size, record_cnt);
        write_report(reportfile, message, tmstr);
}


void sort_lib(char * fname1, int buffer_size, int record_cnt){

        struct timestruct tmstr;
        tmstr = time_start();

        FILE * file1;
        file1 = fopen(fname1, "r+"); 
        quicksort(file1, 0, record_cnt-1, buffer_size);

        tmstr = time_stop(tmstr);
        char message[128];
        sprintf(message, "sort lib bufsize: %d record_cnt: %d", buffer_size, record_cnt);
        write_report(reportfile, message, tmstr);
}

void sort_sys(char* fname1, int buffer_size, int record_cnt){
    struct timestruct tmstr;
    tmstr = time_start();

    int file1;
    file1 = open(fname1, O_RDWR);
    quicksort_sys(file1, 0, record_cnt-1, buffer_size);
    

    tmstr = time_stop(tmstr);
    char message[128];
    sprintf(message, "sort sys bufsize: %d record_cnt: %d", buffer_size, record_cnt);
    write_report(reportfile, message, tmstr);
}

void copy_lib(char* fname1, char * fname2, int buffer_size, int record_cnt){

        struct timestruct tmstr;
        tmstr = time_start();

        FILE * file1;
        FILE * file2;
        char * buffer = (char*) calloc(buffer_size, sizeof(char));

        file1 = fopen(fname1, "r");
        file2 = fopen(fname2, "w"); 

        for(int i = 0; i < record_cnt; i++){
            fread(buffer, sizeof(char), buffer_size, file1);
            fwrite(buffer, sizeof(char), buffer_size, file2);
        }

        fclose(file1);
        fclose(file2);

        tmstr = time_stop(tmstr);
        char message[128];
        sprintf(message, "copy lib bufsize: %d record_cnt: %d", buffer_size, record_cnt);
        write_report(reportfile, message, tmstr);
}

void copy_sys(char* fname1, char * fname2, int buffer_size, int record_cnt){

        struct timestruct tmstr;
        tmstr = time_start();

        int file1;
        int file2;
        char * buffer = (char*) calloc(buffer_size, sizeof(char));

        file1 = open(fname1, O_RDONLY);
        file2 = open(fname2, O_WRONLY); 

        for(int i = 0; i < record_cnt; i++){
            read(file1, buffer,  buffer_size);
            write(file2, buffer, buffer_size);
        }

        close(file1);
        close(file2);

        tmstr = time_stop(tmstr);
        char message[128];
        sprintf(message, "copy sys bufsize: %d record_cnt: %d", buffer_size, record_cnt);
        write_report(reportfile, message, tmstr);
}

int main(int argc, char **argv){
    
    char fname1[FILENAME_LENGTH];
    char fname2[FILENAME_LENGTH];

    reportfile = fopen("report.txt", "a");

    int buffer_size;
    int record_cnt;
    int lib_switch = 0;

    if (!strcmp(argv[1], "generate")){
        if(argc != 6){
            printf("blad parsowania argumentow");
            return 0;
        }
        
        strcpy(fname1, argv[2]);
        sscanf(argv[3], "%d", &record_cnt);
        sscanf(argv[4], "%d", &buffer_size);


        if(!strcmp(argv[5], "lib")){
            lib_switch = 1;
        }else if(!strcmp(argv[5], "sys")){
            lib_switch = 0;
        }else{
            printf("blad parsowania");
            return 0;
        }
        if(lib_switch){
            generate_lib(fname1, buffer_size, record_cnt);
        }else{
            generate_sys(fname1, buffer_size, record_cnt);
        }

    }else if(!strcmp(argv[1], "sort")){
        if(argc != 6){
            printf("blad parsowania argumentow");
            return 0;
        }
        strcpy(fname1, argv[2]);
        sscanf(argv[3], "%d", &record_cnt);
        sscanf(argv[4], "%d", &buffer_size);

        if(!strcmp(argv[5], "lib")){
            lib_switch = 1;
        }else if(!strcmp(argv[5], "sys")){
            lib_switch = 0;
        }else{
            printf("blad parsowania");
            return 0;
        }

        sort_lib(fname1, buffer_size, record_cnt);

    }else if(!strcmp(argv[1], "copy")){
        if(argc != 7){
            printf("blad parsowania argumentow");
            return 0;
        }
        strcpy(fname1, argv[2]);
        strcpy(fname2, argv[3]);
        sscanf(argv[4], "%d", &record_cnt);
        sscanf(argv[5], "%d", &buffer_size);


        if(!strcmp(argv[6], "lib")){
            lib_switch = 1;
        }else if(!strcmp(argv[6], "sys")){
            lib_switch = 0;
        }else{
            printf("blad parsowania lib");
            return 0;
        }
        if(lib_switch){
            copy_lib(fname1, fname2, buffer_size, record_cnt);
        }else{
            copy_sys(fname1, fname2, buffer_size, record_cnt);
        }
    }

}