#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MFILENAME_LENGTH 64
#define MARGS_MAX 5
#define MCMD_ARGS_MAXLENGTH 64
#define MCMD_MAX 5
#define MLINE_BUFF 256


struct config_s{
    char cmd_file[MFILENAME_LENGTH];
};

struct command_s{
    char cmd[MCMD_ARGS_MAXLENGTH];
    char *args[MARGS_MAX +1];
    int argc;
};

struct line_s{
    struct command_s cmds[MCMD_MAX]; // +1 for null pointer
    int cmds_cnt;
};

struct config_s cfg;

int skip_to_next_space(char * buff){
    int i = 0;
    while(buff[i] != '\0' && buff[i] != ' '){
        i++;
    }
    return i;
}
void parse_line(char * line, struct line_s * slin){
    char * buff = line;
    int cmds_cnt = 0;
    int args_cnt = 0;
    while(*buff != '\0'){
        int space_pos = skip_to_next_space(buff);
        if(buff[space_pos - 1] == '|'){
            slin->cmds[cmds_cnt].argc = args_cnt;
            cmds_cnt++;

            args_cnt = 0;
            buff += 2;
            continue;
        }
        
        strncpy(slin->cmds[cmds_cnt].args[args_cnt], buff, space_pos); // bez -1 bo \0 trzeba wstawic recznie
        slin->cmds[cmds_cnt].args[args_cnt][space_pos] = '\0';
        if( *(buff+space_pos) == '\0') break; // koniec lini
        buff  = buff + space_pos + 1;
        args_cnt++;
    }
    slin->cmds[cmds_cnt].argc = args_cnt+1;
    slin->cmds_cnt = cmds_cnt+1;

}

void clear_slin(struct line_s *slin){
    for(int i = 0; i < MCMD_MAX; i++){
        for(int j = 0; j < MARGS_MAX; j++){
            slin->cmds[i].args[j][0] = '\0';
        }
        slin->cmds[i].args[MARGS_MAX]=  (char *) NULL;
    }
}

void close_pipes(int (*pipes)[2], int n){
    for(int i = 0; i < n; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}
int main(int argc, char **argv){

    if(argc != 2 ){
        perror("zla liczba argumentow");
    }
    sscanf(argv[1], "%s", cfg.cmd_file);
    
    char line[MLINE_BUFF];
    
    struct line_s slin;
    for(int i = 0; i < MCMD_MAX; i++){
        for(int j = 0; j < MARGS_MAX; j++){
            slin.cmds[i].args[j] = (char*) calloc(MCMD_ARGS_MAXLENGTH, sizeof(char));
        }
        slin.cmds[i].args[MARGS_MAX]=  (char *) NULL;
    }
    
    FILE * cmds_f = fopen(cfg.cmd_file, "r");
    int pipes[MCMD_MAX][2];
    while(!feof(cmds_f)){
        fgets(line, MLINE_BUFF, cmds_f);
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
        clear_slin(&slin);
        parse_line(line, &slin);


        for(int i = 0; i < slin.cmds_cnt; i++){
            pipe(pipes[i]);
        }

        for(int i = 0; i < slin.cmds_cnt; i++){
            
            pid_t pid = fork();
            if( pid == 0){
                slin.cmds[i].args[slin.cmds[i].argc] = (char*) NULL;
                if(i != 0){
                    // printf("ustawiam in");
                    dup2(pipes[i-1][0], STDIN_FILENO);
                    
                }
                // printf("ustawiam out");
                if(i != slin.cmds_cnt-1) dup2(pipes[i][1], STDOUT_FILENO);
                close_pipes(pipes, slin.cmds_cnt);
                // printf("(PID)%d child ", (int)getpid());
                
                if(execvp(slin.cmds[i].args[0], (char *const*)slin.cmds[i].args) == -1){
                    
                    perror("eeeeerrrr");

                }
                exit(0);
            }
        }

    close_pipes(pipes, slin.cmds_cnt);
    int ttt;
    while(wait(&ttt) != -1);
    }



}
int main(int argc, char **argv){

    if(argc != 2 ){
        perror("zla liczba argumentow");
    }
    sscanf(argv[1], "%s", cfg.cmd_file);
    
    char line[MLINE_BUFF];
    
    struct line_s slin;
    for(int i = 0; i < MCMD_MAX; i++){
        for(int j = 0; j < MARGS_MAX; j++){
            slin.cmds[i].args[j] = (char*) calloc(MCMD_ARGS_MAXLENGTH, sizeof(char));
        }
        slin.cmds[i].args[MARGS_MAX]=  (char *) NULL;
    }
    
    FILE * cmds_f = fopen(cfg.cmd_file, "r");
    int pipes[MCMD_MAX][2];
    while(!feof(cmds_f)){
        fgets(line, MLINE_BUFF, cmds_f);
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
        clear_slin(&slin);
        parse_line(line, &slin);


        for(int i = 0; i < slin.cmds_cnt; i++){
            pipe(pipes[i]);
        }

        for(int i = 0; i < slin.cmds_cnt; i++){
            
            pid_t pid = fork();
            if( pid == 0){
                slin.cmds[i].args[slin.cmds[i].argc] = (char*) NULL;
                if(i != 0){
                    // printf("ustawiam in");
                    dup2(pipes[i-1][0], STDIN_FILENO);
                    
                }
                // printf("ustawiam out");
                if(i != slin.cmds_cnt-1) dup2(pipes[i][1], STDOUT_FILENO);
                close_pipes(pipes, slin.cmds_cnt);
                // printf("(PID)%d child ", (int)getpid());
                
                if(execvp(slin.cmds[i].args[0], (char *const*)slin.cmds[i].args) == -1){
                    
                    perror("eeeeerrrr");

                }
                exit(0);
            }
        }

    close_pipes(pipes, slin.cmds_cnt);
    int ttt;
    while(wait(&ttt) != -1);
    }



}
// int fd[2];
// pipe(fd);
// pid_t pid = fork();
// if (pid == 0) { // dziecko
//     close(fd[1]); 
//     dup2(fd[0],STDIN_FILENO);
//     execlp("grep", "grep","Ala", NULL);
// } else { // rodzic
//     close(fd[0]);
//     // write(fd[1], ...) - przesłanie danych do grep-a
// }