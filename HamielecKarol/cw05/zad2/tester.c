#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define FIFO_NAME "tmpfifo"
#define WRITE_FILE "tmpwrite.txt"
#define READ_PREFIX "tmpread"
#define READ_BUFF "6"
struct config{
    int buf_size;
    char pipe_name[64];
    char file_name[64];
};


int main(int argc, char ** argv){
    
    struct config cfg;
    srand(time(NULL));

    // if(argc != 4){
    //     perror("blad ilosci argumentow");
    //     exit(-1);
    // }

    mkfifo(FIFO_NAME, 777);

    char buffsize_str[4];
    sprintf(buffsize_str, "%d", rand()%10+3);

    int child_pid = fork();
    if(child_pid == 0){
        execl("./konsument", "./konsument", FIFO_NAME, WRITE_FILE, buffsize_str, NULL);
        perror("blad exec konsument");
        exit(0);
    }

    char fname_buff[32];
    char intstr[3];

    for(int i = 0; i < 5; i++){
        strcpy(fname_buff, READ_PREFIX);
        sprintf(intstr, "%d", i);
        strcat(fname_buff, intstr);
        strcat(fname_buff, ".txt");

        FILE * file = fopen(fname_buff, "w");
        int file_size = rand()%100 + 10;
        for(int j = 0; j < file_size; j++){
            fputc('A'+i, file);
        }
        fclose(file);

        child_pid = fork();
        if(child_pid == 0){
            
            execl("./producent", "./producent", FIFO_NAME, fname_buff, buffsize_str, NULL);
            perror("blad exec producent");
            exit(0);
        }
    }

    int fff;
    while(wait(&fff) == -1);

}