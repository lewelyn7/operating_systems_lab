 #define DLL

#ifdef DLL
#include <dlfcn.h>
#endif

#include "zad1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#define DEBUG 0
int match_file_pattern(char* str){
    int i = 0;
    int colon_flag = 0;
    while(str[i] != '\0'){
        if(str[i] == ':'){
            colon_flag = 1;
        }
        i++;
    }
    if( i >= 2 && colon_flag == 1){
        return 1;
    }else{
        return 0;
    }
}
double calc_elapsed(clock_t start, clock_t end){
    return ((double) (end - start)) / sysconf(_SC_CLK_TCK);
}
void write_report(FILE * fptr, char* operation, clock_t start, clock_t end, struct tms tms_start, struct tms tms_end){
    if(fptr == NULL){
        printf("cannot open report file\r\n");
    }
    fprintf(fptr,"\r\noperation: %s\r\n", operation);
    fprintf(fptr,"real time: %f \r\n", calc_elapsed(start, end));
    fprintf(fptr,"user time: %f \r\n", calc_elapsed(tms_start.tms_utime, tms_end.tms_utime));
    fprintf(fptr,"system time: %f \r\n\r\n", calc_elapsed(tms_start.tms_stime, tms_end.tms_stime));

    if(DEBUG == 1){
        printf("\r\noperation: %s\r\n", operation);
        printf("real time: %F\r\n", calc_elapsed(start, end));
        printf("user time: %F\r\n", calc_elapsed(tms_start.tms_utime, tms_end.tms_utime));
        printf("system time: %F\r\n\r\n", calc_elapsed(tms_start.tms_stime, tms_end.tms_stime));
    }
}
int main(int argc, char** argv){
    #ifdef DLL 
    void *handle = dlopen("./libzad1.so", RTLD_LAZY); 
    struct main_block_arr* (*create_main_arr)(int size) = dlsym(handle, "create_main_arr");
    void (*define_files_seq)(char** files_seq, struct main_block_arr* main_arr, int files_number) = dlsym(handle, "define_files_seq");
    void (*compare)(struct main_block_arr* starr) = dlsym(handle, "compare");
    int (*fill_with_data)(struct main_block_arr* starr, int pair_id) = dlsym(handle, "fill_with_data");
    void (*remove_edit_block)(struct   main_block_arr *starr, int id) = dlsym(handle, "remove_edit_block");
    void (*remove_edit_ops)(struct main_block_arr *starr, int bid, int id) = dlsym(handle, "remove_edit_ops");
   #endif
    struct tms global_tms_start;
    struct tms global_tms_end;
    clock_t global_start = times(&global_tms_start);

    struct main_block_arr * starr;
    struct tms tms_start;
    struct tms tms_end;
    clock_t clock_start;
    clock_t clock_end;
    char operation[64];
    fclose(fopen("raport2.txt", "w"));
    FILE * freport;
    freport = fopen("raport2.txt", "a");

    char ** files_seq;
    char fnames[128];
    int table_size;
    for(int i = 1; i < argc; i++){
        strcpy(operation, argv[i]);
        if(!strcmp(argv[i], "create_table")){
            clock_start = times(&tms_start);
            i++;
            sscanf(argv[i], "%d", &table_size);
            starr = create_main_arr(table_size);
            files_seq = (char**) calloc(table_size*2, sizeof(char*));
            for(int i = 0; i < table_size*2; i++){
                files_seq[i] = (char*) calloc(64, sizeof(char));
            }
            clock_end = times(&tms_end);
        }
        else if(!strcmp(argv[i], "compare_pairs")){
            i++;
            int j = 0;
            while(match_file_pattern(argv[i])){
                strcpy(fnames, argv[i]);
                char * colon_pos = strchr(fnames, ':');
                *colon_pos = '\0';
                strcpy(files_seq[j], fnames);
                strcpy(files_seq[j+1], colon_pos+1);
                i++;
                j += 2;

                if(i  == argc) break;
            }
            i--;
            clock_start = times(&tms_start);            
            define_files_seq(files_seq, starr,j);
            compare(starr);
            clock_end = times(&tms_end);
            write_report(freport, "compare_pairs", clock_start, clock_end, tms_start, tms_end);
            
            for(int jj = 0; jj < starr->size; jj++){
                if(starr->arr[jj].file1 == NULL){
                    continue;
                }
                clock_start = times(&tms_start);            
                fill_with_data(starr, jj );
                sprintf(operation, "save block  %s:%s", starr->arr[jj].file1, starr->arr[jj].file2);
                clock_end = times(&tms_end);
                write_report(freport, operation, clock_start, clock_end, tms_start, tms_end);
            }
            strcpy(operation, "");
            for(int ii = 0; ii < j; ii++){
                strcpy(files_seq[ii], "");            
            }
        }
        else if(!strcmp(argv[i], "remove_block")){
            clock_start = times(&tms_start);
            i++;
            int idx;
            sscanf(argv[i], "%d", &idx);
            remove_edit_block(starr, idx);
            clock_end = times(&tms_end);
           
        }
        else if(!strcmp(argv[i], "remove_operation")){
            clock_start = times(&tms_start);
            i++;
            int bidx, opidx;
            sscanf(argv[i], "%d", &bidx);
            i++;
            sscanf(argv[i], "%d", &opidx);
            remove_edit_ops(starr, bidx, opidx);
            clock_end = times(&tms_end);            
        }
        if(strcmp(operation, ""))
            write_report(freport, operation, clock_start, clock_end, tms_start, tms_end);

    }
    clock_t global_clock_end = times(&global_tms_end);
    write_report(freport, "all operations", global_start, global_clock_end, global_tms_start, global_tms_end);
    fclose(freport);
    return 0;

#ifdef DLL
    dlclose(handle);
#endif
}

// int main(int argc, char** argv){
//     struct main_block_arr starr = create_main_arr(argc/2);
//     starr = define_files_seq(argv + 1, starr);
//     print_files(starr);
//     compare(starr);
//     fill_with_data(starr);
    
// }
