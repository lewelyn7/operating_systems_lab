#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

int msleep(long msec)
{
    struct timespec ts;
    int res;


    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);

    return res;
}



struct config{
    int buf_size;
    char pipe_name[64];
    char file_name[64];
};

int main(int argc, char ** argv){
    printf("stała PIPE_BUF: %d\n", _PC_PIPE_BUF);
    
    struct config cfg;
    srand(time(NULL));

    if(argc != 4){
        perror("blad ilosci argumentow producent");
        exit(-1);
    }

    sscanf(argv[1], "%s", cfg.pipe_name);
    sscanf(argv[2], "%s", cfg.file_name);
    sscanf(argv[3], "%d", &cfg.buf_size);
    // cfg.buf_size += 9;


    FILE * pipe_f = fopen(cfg.pipe_name, "w");
    FILE * data_f = fopen(cfg.file_name, "r");
    if(pipe_f == NULL){
        perror("blad otwarcia pliku potoku");
        exit(-1);
    }    
    if(data_f == NULL){
        perror("blad otwarcia pliku do zapisu");
        exit(-1);
    }

    char * buffer = (char*) calloc(cfg.buf_size, sizeof(char));
    char * line_to_write = (char*) calloc(cfg.buf_size + 32, sizeof(char));
    char strint[16];
    
    int read_bytes_num = fread(buffer, sizeof(char), cfg.buf_size, data_f);
    while(!feof(data_f)){
        buffer[read_bytes_num] = '\0';
        strcpy(line_to_write, "");
        strcat(line_to_write, "#");
        sprintf(strint, "%d", (int)getpid());
        strcat(line_to_write, strint);
        strcat(line_to_write, "#(");
        strcat(line_to_write, buffer);
        strcat(line_to_write, ")");
        // strcat(line_to_write, "\n");
        printf("zapisuje...%s\n", line_to_write);
        fputs(line_to_write, pipe_f);
        fflush(pipe_f);
        msleep(rand()%2000);
        
        read_bytes_num = fread(buffer, sizeof(char), cfg.buf_size, data_f);

    }


    free(buffer);
    free(line_to_write);
    fclose(pipe_f);
    fclose(data_f);
}