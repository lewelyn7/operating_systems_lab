#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include "common.h"

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[MTEXT_SIZE];    /* message data */
    
};

int myqID;
int srvqID;
struct msgbuf messbuff;

void sig_handler(int num){
    printf("Otrzymałem: %d", num);
    struct msqid_ds buff;
    messbuff.mtype = STOP;
    if(msgsnd(srvqID, &messbuff, MTEXT_SIZE, 0) == -1){
        perror("blad wyslania komunikatu stopu");
    }
    msgctl(myqID, IPC_RMID, &buff);
    exit(0);
}

int main(int argc, char ** argv){

    const char * home = getenv("HOME");
    int myidonsrv;
    struct msqid_ds buff;
    signal(SIGINT, sig_handler);
    key_t mykey = ftok(home, (int)getpid()%256);
    // key_t mykey = ftok(home, 2);
    myqID = msgget(mykey, IPC_CREAT | IPC_EXCL | S_IRWXU); 

    srvqID = msgget(ftok(home, 'A'), 0);


    if(myqID == -1){
        perror("error kolejki mojej");
        exit(-1);
    }    
    if(srvqID == -1){
        perror("error kolejki serwera");
        exit(-1);
    }  


    
    messbuff.mtype = INIT;
    sprintf(messbuff.mtext, "%d", mykey);
    printf("wysylam moj klucz %s\n", messbuff.mtext);
    if(msgsnd(srvqID, &messbuff, MTEXT_SIZE, 0) == -1){
        perror("blad wyslania komunikatu");
        exit(-1);
    }
   
    int size_received = 0;
    size_received = msgrcv(myqID, &messbuff, MTEXT_SIZE, 0, 0);
    if(size_received > 0){
        sscanf(messbuff.mtext, "%d", &myidonsrv);
        printf("odebralem id: %d\n", myidonsrv);

    }
    char scanfbuff[32];
    while(1){
        scanf("%s", scanfbuff);
        if(!strcmp("LIST", scanfbuff)){
            printf("wysylam LIST...");
            messbuff.mtype = LIST;
            if(msgsnd(srvqID, &messbuff, MTEXT_SIZE, 0) == -1){
                printf("failed\n");
                perror("error");
            }else{
                printf("sukces\n");
            }
            
        }else if(!strcmp("STOP", scanfbuff)){
            sig_handler(2);
        }else if(!strcmp("CONNECT", scanfbuff)){
            int connect_to;
            printf("connect to: ");
            scanf("%d", &connect_to);
            char for_recovery[MTEXT_SIZE];
            strcpy(for_recovery, messbuff.mtext);
            strcat(messbuff.mtext, )
            msgsnd(srvqID, messbuff, MTEXT_SIZE, 0);
        }else if(!strcmp("DISCONNECT", scanfbuff)){
            
        }
    }


    msgctl(myqID, IPC_RMID, &buff);
  
} 