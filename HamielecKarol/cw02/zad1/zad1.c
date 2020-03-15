#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FILENAME_LENGTH 64

char* getter(FILE * file,int buffer_size, int idx){
    char * buffer = (char*) calloc(buffer_size, sizeof(char));
    fseek(file, buffer_size*idx, SEEK_SET);
    fgets(buffer, buffer_size,file);
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




int main(int argc, char **argv){
    
    char fname1[FILENAME_LENGTH];
    char fname2[FILENAME_LENGTH];

    FILE * file1;
    FILE * file2;

    char * buffer;
    char byte_tmp;
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

        buffer = (char*) calloc(buffer_size, sizeof(char));

        if(!strcmp(argv[5], "lib")){
            lib_switch = 1;
        }else if(!strcmp(argv[5], "sys")){
            lib_switch = 0;
        }else{
            printf("blad parsowania");
            return 0;
        }

        file1 = fopen(fname1, "w");
        FILE * rand_handler= fopen("/dev/random", "r");

        for(int j = 0; j < record_cnt; j++){
            for(int i = 0; i < buffer_size; i++){
                fread(&byte_tmp, sizeof(char), 1, rand_handler);
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
    }else if(!strcmp(argv[1], "sort")){
        if(argc != 6){
            printf("blad parsowania argumentow");
            return 0;
        }
        strcpy(fname1, argv[2]);
        sscanf(argv[3], "%d", &record_cnt);
        sscanf(argv[4], "%d", &buffer_size);

        buffer = (char*) calloc(buffer_size, sizeof(char));

        if(!strcmp(argv[5], "lib")){
            lib_switch = 1;
        }else if(!strcmp(argv[5], "sys")){
            lib_switch = 0;
        }else{
            printf("blad parsowania");
            return 0;
        }

        file1 = fopen(fname1, "r+"); 

        quicksort(file1, 0, record_cnt-1, buffer_size);

    }

}