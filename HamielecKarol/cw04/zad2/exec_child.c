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
    printf("Otrzymałem: %s \r\n", sig_dict[num]);
}

int main(int argc, char** argv){
    struct config cfg;

    if(argc != 2){
        printf("zla liczba arugmentow");
        exit(-1);
    }
    sscanf(argv[1], "%s", cfg.sig);


    if(!strcmp(cfg.sig, "-n")){
        sscanf(argv[3], "%d", &cfg.signum);
        strcpy(cfg.sig, sig_dict[cfg.signum]);
    }else{
        cfg.signum = find_sig_num(cfg.sig);
    }
    
    printf("jestem sobie exec i robie raise: %s...\r\n", cfg.sig);
    raise(cfg.signum);



}