#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_LENGTH 25

    struct main_block_arr{
        int size;
        struct edit_block_arr *arr;
    };

    struct edit_block_arr{
        char * file1;
        char * file2;        
        int size;
        char ** edit_ops;
    };

    struct main_block_arr create_main_arr(int size){
        struct  main_block_arr main_arr;
        main_arr.size = 0;
        return main_arr;
    }

    struct main_block_arr define_files_seq(char* files_seq, struct main_block_arr main_arr){
        
        while(files_seq != NULL){
            char * filea = calloc(FILE_LENGTH, sizeof(filea));
            char * fileb = calloc(FILE_LENGTH, sizeof(fileb));
            main_arr.arr = (*edit_block_arr) calloc(1, sizeof(edit_block_arr));


            char * start_pointer = files_seq;
            if(start_pointer == NULL){
                return main_arr;
            }
            char * space_pointer = strchr(start_pointer + 1, ' ');
            if(space_pointer == NULL){
                return main_arr;
            }
            char * end_pointer = strchr(space_pointer+1, ' ');
            if(end_pointer == NULL){
                end_pointer = strchr(space_pointer+1, '\0');
            }


            int i = 0;
            while(start_pointer != space_pointer){
                filea[i] = *start_pointer;
                start_pointer++;
                i++;
            }
            filea[i] = '\0';

            i = 0;
            space_pointer++;
            while(space_pointer != end_pointer){

                fileb[i] = *space_pointer;
                i++;
                space_pointer++;
            }
            printf("a: %s \r\n", filea);
            printf("b: %s \r\n", fileb);
            



            if(*end_pointer == '\0'){
                files_seq = NULL;
            }else{
                files_seq = end_pointer + 1;
            }
        }
        return main_arr;
    }

int main(void ){
    struct main_block_arr  starr= create_main_arr();
    define_files_seq("a.txt b.txt c.txt d.txt", starr);

}
