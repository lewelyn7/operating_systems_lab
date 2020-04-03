#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

struct config{
    int buf_size;
    char pipe_name[64];
    char file_name[64];
};

int main(int argc, char ** argv){
    printf("stała PIPE_BUF: %d\n", _PC_PIPE_BUF);

    struct config cfg;

    if(argc != 4){
        perror("blad ilosci argumentow");   
        exit(-1);
    }

    sscanf(argv[1], "%s", cfg.pipe_name);
    sscanf(argv[2], "%s", cfg.file_name);
    sscanf(argv[3], "%d", &cfg.buf_size);
    cfg.buf_size += 9;

    if(mkfifo(cfg.pipe_name, S_IRUSR | S_IWUSR) == -1){
        perror("blad stworzenia potoku");
        // exit(-1);
    }

    FILE * pipe_f = fopen(cfg.pipe_name, "r");
    FILE * data_f = fopen(cfg.file_name, "w");

    if(pipe_f == NULL){
        perror("blad otwarcia pliku potoku");
        exit(-1);
    }    
    if(data_f == NULL){
        perror("blad otwarcia pliku do odczytu");
        exit(-1);
    }

    char * buffer = (char*) calloc(cfg.buf_size, sizeof(char));
    char strint[16];
    int read_bytes_num = fread(buffer, sizeof(char), cfg.buf_size, pipe_f);
    while(!feof(pipe_f)){
        buffer[read_bytes_num] = '\0';
        fputs(buffer, data_f);
        fputc('\n', data_f);
        if(read_bytes_num > 0){
            printf("przeczytalem...%s\n", buffer);
            strcpy(buffer, "");
        }
        read_bytes_num = fread(buffer, sizeof(char), cfg.buf_size, pipe_f);

    }
    fclose(pipe_f);
    fclose(data_f);

    free(buffer);
}