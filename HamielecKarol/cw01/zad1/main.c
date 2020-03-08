
#include "zad1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char** argv){

    int size;
    sscanf(argv[1], "%d", &size);
    printf("%d", size);
    struct main_block_arr starr;
    

    char **files_seq = (char**) calloc(size, sizeof(char*));
    for(int i = 0; i < size; i++){
        files_seq[i] = (char*) calloc(64, sizeof(char));
    }
    char fnames[128];
    int table_size;
    for(int i = 1; i < argc; i++){

        if(!strcmp(argv[i], "create_table")){
            i++;
            sscanf(argv[i], "%d", &table_size);
            starr = create_main_arr(table_size);
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
                j++;

                if(i  == argc) break;

            }
            starr = define_files_seq(files_seq, starr);
            starr = compare(starr);
            starr = fill_with_data(starr);
        }
        else if(!strcmp(argv[i], "remove_block")){
            i++;
            int idx;
            sscanf(argv[i], %d, &idx);
            remove_edit_block(starr, idx);
            
        }
        else if(!strcmp(argv[i], "remove_block")){
            i++;
            int bidx, opidx;
            sscanf(argv[i], %d, &bidx);
            i++;
            sscanf(argv[i], %d, &opidx);
            remove_edit_block(starr[bidx], opidx);
            
        }
    }

}

// int main(int argc, char** argv){
//     struct main_block_arr starr = create_main_arr(argc/2);
//     starr = define_files_seq(argv + 1, starr);
//     print_files(starr);
//     compare(starr);
//     fill_with_data(starr);
    
// }
