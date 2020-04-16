#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


int main(int argc, char ** argv){
    if(argc != 2){
        printf("zla liczba argumentow");
        exit(-1);
    }


    // FILE * dataf = fopen(argv[1], "r");
    char cmd[256];
    strcpy(cmd, "sort ");
    strcat(cmd, argv[1]);

    FILE * pipef = popen(cmd, "r");

    char buffer[512];
    while(!feof(pipef)){
        fgets(buffer, 512, pipef);
        printf(buffer);

    }
}