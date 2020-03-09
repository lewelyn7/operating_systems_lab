#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zad1.h>

#define FILE_LENGTH 25
#define COMMAND_SIZE 256



    struct main_block_arr* create_main_arr(int size){
        struct  main_block_arr *main_arr = calloc(1, sizeof(struct main_block_arr));
        main_arr->size = size;
        main_arr->idx = -1;
        main_arr->arr = (struct edit_block_arr*) calloc(size, sizeof(struct edit_block_arr));
        for(int i = 0; i < size; i++){
            main_arr->arr[i].size = 0;
            main_arr->arr[i].file1 = NULL;
            main_arr->arr[i].file2 = NULL;
            main_arr->arr[i].edit_ops = NULL;
        }
        return main_arr;
    }

    void define_files_seq(char** files_seq, struct main_block_arr *main_arr, int files_number){
        for(int j = 0; j < files_number/2; j++){
            int i = 0;
            for(;i < main_arr->size; i++){
                if(main_arr->arr[i].file1 == NULL){
                    i++;
                    break;
                }

            }
            i--; 
            if(main_arr->arr[i].file1 != NULL){
                printf("cant define file seq table is too small");
                return;
            }
            main_arr->arr[i].size = 0;
            main_arr->arr[i].file1 = (char*) calloc(strlen(files_seq[j*2]), sizeof(char));
            main_arr->arr[i].file2 = (char*) calloc(strlen(files_seq[j*2+1]), sizeof(char));
            strcpy(main_arr->arr[i].file1, files_seq[j*2]);
            strcpy(main_arr->arr[i].file2, files_seq[j*2+1]);
            
        }

    }

    void compare(struct main_block_arr *starr){
        char * command = calloc(COMMAND_SIZE, sizeof(char));
        char str_number[5];
        for(int i = 0; i <= starr->size; i++){
            if(starr->arr[i].file1 == NULL){
                continue;
            }
            for(int i = 0; i < COMMAND_SIZE; i++) command[i] = 0;
            strcpy(command, "diff ");
            strcat(command, starr->arr[i].file1);
            strcat(command, " ");
            strcat(command, starr->arr[i].file2);
            strcat(command, " > ");
            strcat(command, "./tmp/diff_op");
            sprintf(str_number, "%d", i);        
            strcat(command, str_number);        
            strcat(command, ".tmp");        
            system(command);
        }
    }
    void print_files(struct main_block_arr *main){
        for(int i = 0; i <= main->idx; i++){
            printf("file: %s : %s \r\n", main->arr[i].file1, main->arr[i].file2);
        }
    }
    int count_edit_blocks(FILE * f){

        int op_counter = 1;
        char ch1, ch2;
        fseek (f, 0, SEEK_SET);
        ch1 = fgetc(f);
        while( !feof(f)) {
            ch2 = fgetc(f);
            if(ch1 == '\n' && ch2 > 47 && ch2 < 58){
                op_counter++;
            }
            ch1 = ch2;
        }
        fseek (f, 0, SEEK_SET);
        return op_counter;
    }
    int get_edit_operations(struct main_block_arr *starr, int id){
        return starr->arr[id].size;
    }
    int fill_with_data(struct main_block_arr *starr, int pair_id){
    
            char *fname = (char*) calloc(FILE_LENGTH, sizeof(char));
            char str_number[5];        
            strcat(fname, "./tmp/diff_op");
            sprintf(str_number, "%d", pair_id);        
            strcat(fname, str_number);        
            strcat(fname, ".tmp"); 

            FILE *f = fopen(fname, "r");
            if (f == NULL)
            {
                perror("Cannot open file");
                exit(1);
            }
            fseek (f, 0, SEEK_END);
            int  length = ftell (f);
            fseek (f, 0, SEEK_SET);
            int op_counter = count_edit_blocks(f);
            starr->arr[pair_id].size = op_counter;
            starr->arr[pair_id].edit_ops = (char**) calloc(op_counter, sizeof(char*));

            for(int j = 0; j < op_counter; j++){
                starr->arr[pair_id].edit_ops[j] = (char*) calloc(length, sizeof(char));
                char ch1, ch2;
                ch1 = fgetc(f);
                int end_of_block = 0;
                int ii = 0;
                while(!end_of_block && !feof(f)) {
                    ch2 = fgetc(f);
                    if(ch1 == '\n' && ch2 > 47 && ch2 < 58){

                        end_of_block = 1;
                    }
                    starr->arr[pair_id].edit_ops[j][ii] = ch1;
                    ii++;
                    ch1 = ch2;
                }
                fseek(f, -1, SEEK_CUR);
            }

            fclose(f);
            return pair_id;
    
    }
    void remove_edit_block(struct main_block_arr *starr, int id){
        free(starr->arr[id].edit_ops);
        starr->arr[id].edit_ops = NULL;
        starr->arr[id].file1 = NULL;
        starr->arr[id].file2 = NULL;


    }
    void remove_edit_ops(struct main_block_arr *starr, int bid, int id){
        free(starr->arr[bid].edit_ops[id]);
    }

