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

int myidonsrv;
int myqID;
int srvqID;
struct msgbuf messbuff;

void sig_handler(int num){
    printf("Otrzymałem: %d", num);
    struct msqid_ds buff;
    
    messbuff.mtype = STOP;
    messbuff.who = myidonsrv;
    if(msgsnd(srvqID, &messbuff, MSG_SIZE, 0) == -1){
        perror("blad wyslania komunikatu stopu");
    }
    msgctl(myqID, IPC_RMID, &buff);
    exit(0);
}


void chat_mode(int chatqID, int mode){
        if(mode == 1){
            printf("czekam na msg..\n");
            msgrcv(myqID, &messbuff, MSG_SIZE, 0, 0);
            if(messbuff.mtype == DISCONNECT){
                messbuff.who = myidonsrv;
                msgsnd(srvqID, &messbuff, MSG_SIZE, 0);
                printf("disconnected\n");
                return;
            }
            printf("odpowiedz: %s \n", messbuff.mtext);            
        }
        while(1){
        printf("podaj message: ");
        messbuff.mtype = CHAT;
        scanf("%s", messbuff.mtext);
        if(!strcmp(messbuff.mtext, "DISCONNECT")){
            printf("disconnected\n");
            messbuff.mtype = DISCONNECT;
            messbuff.who = myidonsrv;
            msgsnd(srvqID, &messbuff, MSG_SIZE, 0);
            msgsnd(chatqID, &messbuff, MSG_SIZE, 0);
            return;

        }
        messbuff.who = myidonsrv;
        if(msgsnd(chatqID, &messbuff, MSG_SIZE, 0) == -1){
            perror("blad wysylania");
        }
        
        printf("wyslano czekam na odpowiedz\n");
        msgrcv(myqID, &messbuff, MSG_SIZE, 0, 0);
        if(messbuff.mtype == DISCONNECT){
            messbuff.who = myidonsrv;
            msgsnd(srvqID, &messbuff, MSG_SIZE, 0);
            printf("disconnected\n");
            return;
        }
        printf("odpowiedz: %s \n", messbuff.mtext);

    }
}

int main(int argc, char ** argv){

    const char * home = getenv("HOME");
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
    messbuff.who = mykey;
    printf("wysylam moj klucz %s\n", messbuff.mtext);
    messbuff.who = myidonsrv;
    if(msgsnd(srvqID, &messbuff, MSG_SIZE, 0) == -1){
        perror("blad wyslania komunikatu");
        exit(-1);
    }
   
    int size_received = 0;
    size_received = msgrcv(myqID, &messbuff, MSG_SIZE, 0, 0);
    if(size_received > 0){
        sscanf(messbuff.mtext, "%d", &myidonsrv);
        printf("odebralem id: %d\n", myidonsrv);
        messbuff.who = myidonsrv;

    }
    char scanfbuff[32];
    while(1){
        scanf("%s", scanfbuff);
        if(!strcmp("LIST", scanfbuff)){
            printf("wysylam LIST...");
            messbuff.mtype = LIST;
            messbuff.who = myidonsrv;
            if(msgsnd(srvqID, &messbuff, MSG_SIZE, 0) == -1){
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
            messbuff.connect_to = connect_to;
            messbuff.mtype = CONNECT;
            messbuff.who = myidonsrv;
            msgsnd(srvqID, &messbuff, MSG_SIZE, 0);
            if(msgrcv(myqID, &messbuff, MSG_SIZE, 0, 0) > 0 && messbuff.mtype == CONNECT){
                int chatqID = msgget(messbuff.key, 0);
                if(chatqID == -1){
                    perror("eroor otwierania chatu");
                }
                printf("otwieram chat_mode \n ");
                chat_mode(chatqID, 0);
                

            }
            
        }else if(!strcmp("CHAT", scanfbuff)){
            printf("otwieram chat_mode \n ");
            if(msgrcv(myqID, &messbuff, MSG_SIZE, 0, 0) == -1){
                perror("errror cahtmode");
            }
            
            if(messbuff.mtype == CONNECT){
                printf("otwieram chat\n");
                int chatqID = msgget(messbuff.key, 0);
                if(chatqID == -1){
                    perror("eroor otwierania chatu");
                }                
                chat_mode(chatqID, 1);
            }

            
        }else{


        }
    }


    msgctl(myqID, IPC_RMID, &buff);
  
} 