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
#include <dirent.h>

struct config{
    int mtime;
    int atime;
    int maxdepth;
    char start_point[512];
};

void find(struct config cfg){
    DIR* direk = opendir(cfg.start_point);
    if(direk == NULL){
        printf("blad otwarcia kataogu");
        exit(-1);
    }

    struct dirent* curr_rec;
    struct stat buf_stat;
    char pathfull[256];
    
    struct tm *ts;
    char buff[80];

    curr_rec = readdir(direk);
    while(curr_rec != NULL){
        strcpy(pathfull, cfg.start_point);
        strcat(pathfull, curr_rec->d_name);
        if(stat(pathfull, &buf_stat) != 0){
            printf("blad czytania pliku");
            exit(-1);
        }
        printf("%s %d", pathfull, (int)buf_stat.st_nlink);
        if(S_ISDIR(buf_stat.st_mode)){
            printf(" dir ");
        }else if(S_ISCHR(buf_stat.st_mode)){
            printf(" char dev ");
        }else if(S_ISBLK(buf_stat.st_mode)){
            printf(" block dev ");
        }else if(S_ISREG(buf_stat.st_mode)){
            printf(" file ");           
        }else if(S_ISFIFO(buf_stat.st_mode)){
            printf(" fifo ");           
        }else if(S_ISLNK(buf_stat.st_mode)){
            printf(" slink ");           
        }else if(S_ISSOCK(buf_stat.st_mode)){
            printf(" sock ");           
        }
        printf(" %d ", (int)buf_stat.st_size);

        ts = localtime(&buf_stat.st_atime);
        strftime(buff, sizeof(buff), " %a %Y-%m-%d %H:%M:%S %Z ", ts);
        printf(" %s ", buff);

        ts = localtime(&buf_stat.st_mtime);
        strftime(buff, sizeof(buff), " %a %Y-%m-%d %H:%M:%S %Z ", ts);
        printf(" %s ", buff);

        printf("\r\n");
        curr_rec = readdir(direk);
    }
}

int main(int argc, char** argv){

    struct config cfg;
    for(int i = 1; i < argc; i++){
        
        if(!strcmp(argv[i], "-mtime")){
            if(i == argc-1){
                printf("blad argumentow");
                exit(-1);
            }
            i++;
            sscanf(argv[i], "%d", &cfg.mtime);

        }else if(!strcmp(argv[i], "-atime")){
            if(i == argc-1){
                printf("blad argumentow");
                exit(-1);
            }
            i++;
            sscanf(argv[i], "%d", &cfg.atime);
            
        }else if(!strcmp(argv[i], "-maxdepth")){
            if(i == argc-1){
                printf("blad argumentow");
                exit(-1);
            }
            i++;
            sscanf(argv[i], "%d", &cfg.maxdepth);
            
        }else{

            //find last "/"
            // char * jump_pointer = argv[0];
            // char * slash_pointer;
            // while(jump_pointer != NULL){
            //     slash_pointer = jump_pointer;
            //     jump_pointer = strchr(jump_pointer+1, '/');
            // }
            // slash_pointer++;
            // *slash_pointer = '\0';

            // strcpy(cfg.start_point, argv[0]);
            if(argv[i][0] == '/'){
                strcpy(cfg.start_point, argv[i]);
            }else{
                getcwd(cfg.start_point, 512);
                strcat(cfg.start_point, "/");
                strcat(cfg.start_point, argv[i]);
            }

            strcat(cfg.start_point, "/");
            chdir(cfg.start_point);
            getcwd(cfg.start_point, 512);
            strcat(cfg.start_point, "/");
           
        
        }
    }
    find(cfg);
}