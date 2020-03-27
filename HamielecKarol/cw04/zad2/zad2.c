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
#include <sys/wait.h>

#define FORK_MODE 1
#define EXEC_MODE 0


char sig_dict[32][16] = {"xxxx", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGIOT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO", "SIGPOLL"};
struct config{
    char opt[16];
    char sig[16];
    int signum;
    int mode;
};
int find_sig_num(char * sig){
    for(int i = 1; i < 31; i++){
        if(!strcmp(sig, sig_dict[i])){
            return i;
        }
    }
    return 0;
}

void sig_handler(int num){
    printf("(PID) %d Otrzymałem: %s \r\n", (int)getpid(), sig_dict[num]);
}

int main(int argc, char** argv){
    struct config cfg;

    if(argc != 4 && argc != 5){
        printf("zla liczba arugmentow");
        exit(-1);
    }
    sscanf(argv[1], "%s", cfg.opt);
    sscanf(argv[2], "%s", cfg.sig);
    if(!strcmp(argv[argc-1], "fork")){
        cfg.mode = FORK_MODE;
    }else if(!strcmp(argv[argc-1], "exec")){
        cfg.mode = EXEC_MODE;
    }else{
        printf(" error forkowo-execowy");
        exit(-1);
    }

    if(!strcmp(cfg.sig, "-n")){
        sscanf(argv[3], "%d", &cfg.signum);
        strcpy(cfg.sig, sig_dict[cfg.signum]);
    }else{
        cfg.signum = find_sig_num(cfg.sig);
    }

    if(!strcmp(cfg.opt, "ignore")){
        printf("ustawiam ignorowanie %s\r\n", cfg.sig);
        signal(cfg.signum, SIG_IGN);
    }else if(!strcmp(cfg.opt, "handler")){
        printf("ustawiam handler %s\r\n", cfg.sig);
        signal(cfg.signum, sig_handler);   
    }else if(!strcmp(cfg.opt, "mask")){
        printf("ustawiam maske  na %s\r\n", cfg.sig);
        sigset_t new_set;
        sigemptyset(&new_set);
        sigaddset(&new_set, cfg.signum);
        sigprocmask(SIG_SETMASK, &new_set, NULL);
    }else if(!strcmp(cfg.opt, "pending")){
    
    }

    printf("raise: %s...\r\n", cfg.sig);
    raise(cfg.signum);

    if(cfg.mode == FORK_MODE){
        int childpid = fork();
        if(childpid == 0){
            printf("raise jako potomek (PID)%d %s...\r\n", (int)getpid(), cfg.sig);
            raise(cfg.signum);
            exit(0);
        }
        childpid = wait(NULL);
    }else{
        printf("bedzie raise jako exec %s...\r\n", cfg.sig);
        execl("./exec_child", "exec_child", cfg.sig, NULL);
    }



}