#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ftw.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
// int (*fn) (const char *fpath, const struct stat *sb,
//                    int typeflag, struct FTW *ftwbuf)
int pause_flag = 0;

int print_ls(const char *fpath, const struct stat *sb, int typeflag,  struct FTW * ftw){
    printf("%s\r\n", fpath);
    return 0;
}

void sigstop_handler(int sig){
    if(pause_flag){
        pause_flag = 0;
    }else{
        printf("my pid: %d ", (int)getpid());
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\r\n");
        pause_flag = 1;
    }
}

void sigint_handler(int sig){
    printf("Odebrano sygnał SIGINT");
    exit(0);
}
// int sigaction(int sig_no, const struct sigaction *act, struct sigaction *old_act);

int main(){
    DIR * direk = opendir("./");
    if(direk == NULL){
        printf("blad otwarcia kataogu");
        exit(-1);
    }

    struct sigaction act;
    act.sa_handler = sigstop_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0; 

    sigaction(SIGTSTP, &act, NULL );
    signal(SIGINT, sigint_handler);
    int flags = 0;
    while(1){
        if(pause_flag){
            pause();
        }
        nftw("./", &print_ls, 20, flags);
        sleep(1); // żeby nie zalało terminala
        
    }
}