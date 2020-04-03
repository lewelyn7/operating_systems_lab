// #define podpunkt_B
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/times.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define M_KILL 0
#define M_SIGQUEUE 1
#define M_SIGRT 2


#define M_TRANSMITTER 1
#define M_RECEIVER 0


char sig_dict[32][16] = {"xxxx", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO", "SIGPOLL"};
struct config{
    int quantity;
    int pid;
    int mode;
    int role;
};
struct config cfg;


int find_sig_num(char * sig){
    for(int i = 1; i < 31; i++){
        if(!strcmp(sig, sig_dict[i])){
            return i;
        }
    }
    return 0;
}
int usr1_cnt = 0;
int sig_received;

void sig_handler(int num, siginfo_t * info, void * ucontext){
    // if(cfg.role == M_RECEIVER) printf("\r\n(PID) %d Otrzymałem: %s od %d\r\n", (int)getpid(), sig_dict[num], info->si_pid);
    sig_received = num;
    if(num == SIGUSR1 || num == SIGRTMIN+1){
        usr1_cnt++;
    }
    
    #ifdef podpunkt_B
        if(cfg.role == M_RECEIVER) kill(cfg.pid, SIGUSR1);
    #endif

}
void send_sig(struct config cfg, int signo){
    if(cfg.mode == M_KILL){
        kill(cfg.pid, signo);
    }else if(cfg.mode == M_SIGQUEUE){
        
        union sigval sval;
        sval.sival_ptr = NULL;
        sigqueue(cfg.pid, signo, sval);
    }else if(cfg.mode == M_SIGRT){
        if(signo == SIGUSR1){
            kill(cfg.pid, SIGRTMIN+1);
        }else{
            kill(cfg.pid, SIGRTMIN+2);
        }
    }

}


int main(int argc, char** argv){
    cfg.role = M_TRANSMITTER;
    printf("ja to (PID)%d\r\n", (int)getpid());

    if(argc != 4){
        printf("zla liczba arugmentow");
        exit(-1);
    }
    sscanf(argv[1], "%d", &cfg.pid);
    sscanf(argv[2], "%d", &cfg.quantity);
    char buff[16];
    sscanf(argv[3], "%s", buff);
    if(!strcmp(buff, "kill")){
        cfg.mode = M_KILL;
    }else if(!strcmp(buff, "sigqueue")){
        cfg.mode = M_SIGQUEUE;
    }else if(!strcmp(buff, "sigrt")){
        cfg.mode = M_SIGRT;
    }else{
        printf("blad argumentow");
        exit(-1);
    }

    struct sigaction sigac_params;
    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGUSR1);
    sigaddset(&sset, SIGUSR2);
    sigaddset(&sset, SIGRTMIN+1);
    sigaddset(&sset, SIGRTMIN+2);
    sigac_params.sa_mask = sset;
    sigac_params.sa_flags = SA_SIGINFO;
    sigac_params.sa_sigaction = &sig_handler;
    sigaction(SIGUSR1, &sigac_params, NULL);
    sigaction(SIGUSR2, &sigac_params, NULL);
    sigaction(SIGRTMIN+1, &sigac_params, NULL);
    sigaction(SIGRTMIN+2, &sigac_params, NULL);


    for(int i = 0; i < cfg.quantity; i++){
        send_sig(cfg, SIGUSR1);
        #ifdef podpunkt_B
            pause();
        #endif
    }
    send_sig(cfg, SIGUSR2);
    #ifdef podpunkt_B
        pause();
    #endif
    cfg.role = M_RECEIVER;
    usr1_cnt = 0;
    printf("wyslalem %d syngalow \n", cfg.quantity);


    while(1){
        pause();
        if(sig_received == SIGUSR2  || sig_received == SIGRTMIN+2){
            printf("odebralem %d syngalow, a powinienem %d", usr1_cnt, cfg.quantity);
            exit(0);
        }
    }    

}