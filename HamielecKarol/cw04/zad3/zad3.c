#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define FORK_MODE 1
#define EXEC_MODE 0


char sig_dict[32][16] = {"xxxx", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO", "SIGPOLL"};
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

void sig_handler(int num, siginfo_t * info, void * ucontext){

    printf("\r\n(PID) %d Otrzymałem: %s errno: %d file_descriptor: %d exit_value_or_signal: %d \r\n", (int)getpid(), sig_dict[num], info->si_errno, info->si_fd, info->si_status);
}
// void sig_handler(int num){
//     printf("\r\n(PID) %d Otrzymałem: %s \r\n", (int)getpid(), sig_dict[num]);
// }

int main(int argc, char** argv){
    printf("ja to (PID)%d\r\n", (int)getpid());
    struct config cfg;

    if(argc != 2 && argc != 3){
        printf("zla liczba arugmentow");
        exit(-1);
    }
    sscanf(argv[1], "%s", cfg.sig);

    if(!strcmp(cfg.sig, "-n")){
        sscanf(argv[1], "%d", &cfg.signum);
        strcpy(cfg.sig, sig_dict[cfg.signum]);
    }else{
        cfg.signum = find_sig_num(cfg.sig);
    }

    // int sigaction(int sig_no, const struct sigaction *act, struct sigaction *old_act);
    struct sigaction sigac_params;
    sigset_t sset;
    sigemptyset(&sset);
    sigac_params.sa_mask = sset;
    sigac_params.sa_flags =  SA_SIGINFO;
    sigac_params.sa_sigaction = &sig_handler;
    sigaction(cfg.signum, &sigac_params, NULL);




    printf("raise: %s...\r\n", cfg.sig);
    raise(cfg.signum);


    while(1){
        sleep(1);
    }



}