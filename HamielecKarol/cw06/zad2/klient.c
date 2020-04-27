#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include "common.h"

int myidonsrv;
int myqID;
int srvqID;
struct msgbuf messbuff;
char my_key[12];
unsigned int priop = 1;

void clean_up(){

    
    
    messbuff.mtype = STOP;

    messbuff.who = myidonsrv;
    
    if(mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop) == -1){
        perror("blad wyslania komunikatu stopu");
        
    }
    mq_close(myqID);
    mq_unlink(my_key);
    
}

void sig_handler(int num){
    printf("Otrzymałem: %d", num);
    exit(0);
}

int my_mq_receive(int qID, char * messbuff2, int msg_size, unsigned int * priop){
    int size_received;
    do{

        size_received = mq_receive(qID,   messbuff2, msg_size, priop);
        if(size_received == -1){
            perror(" znowu errr: ");
        }

    }while(size_received < 0);

    return size_received;

}

void chat_mode(int chatqID, int mode){
        if(mode == 1){
            printf("czekam na msg..\n");
            my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop);
            if(messbuff.mtype == DISCONNECT){
                messbuff.who = myidonsrv;
                mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop);
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
            mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop);
            mq_send(chatqID, (char *) &messbuff, MSG_SIZE, priop);
            return;

        }
        messbuff.who = myidonsrv;
        if(mq_send(chatqID, (char *) &messbuff, MSG_SIZE, priop) == -1){
            perror("blad wysylania");
        }
        
        printf("wyslano czekam na odpowiedz\n");
        my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop);
        if(messbuff.mtype == DISCONNECT){
            messbuff.who = myidonsrv;
            mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop);
            printf("disconnected\n");
            return;
        }
        printf("odpowiedz: %s \n", messbuff.mtext);

    }
}



int main(int argc, char ** argv){
    // printf("%ld", sizeof(struct msgbuf));
    // exit(0);
    const char * home = "/XDSDFSDFS";

    struct mq_attr attry;
    attry.mq_flags = O_NONBLOCK;
    attry.mq_maxmsg = 8;
    attry.mq_msgsize = MSG_SIZE;

    signal(SIGINT, sig_handler);
    atexit(clean_up);
    //key generating
    sprintf(my_key, "/ab%d", (int)getpid());
     if(mq_unlink(my_key) == 0)
        fprintf(stdout, "Message queue %s removed from system.\n", my_key);   
    // key_t my_key = ftok(home, 2);
    // myqID = msgget(my_key, IPC_CREAT | IPC_EXCL | S_IRWXU); 
    myqID = mq_open(my_key, O_CREAT | O_RDONLY | O_EXCL, 0666, &attry);
    // mq_setattr(myqID, &attry, NULL);
    if(myqID == -1){
        perror("error kolejki mojej");
        exit(-1);
    }  
    // srvqID = msgget(ftok(home, 'A'), 0);
    srvqID = mq_open(home, O_RDWR, &attry);
    


  
    if(srvqID == -1){
        perror("error kolejki serwera");
        exit(-1);
    }  


    
    messbuff.mtype = INIT;
    // sprintf(messbuff.mtext, "%d", &my_key);
    strcpy(messbuff.key, my_key);
    printf("wysylam moj klucz %s\n", messbuff.key);
    messbuff.who = myidonsrv;
    if(mq_send(srvqID, (char *) &messbuff, MSG_SIZE, 1) == -1){
        perror("blad wyslania komunikatu");
        exit(-1);
    }
    printf("wyslano key\n");
   
    int size_received = 0;
    size_received = my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop);
    if(size_received > 0){
        sscanf(messbuff.mtext, "%d", &myidonsrv);
        printf("odebralem id: %d\n", myidonsrv);
        messbuff.who = myidonsrv;

    }
    char scanfbuff[32];
    while(1){
        printf("podaj komende: ");
        scanf("%s", scanfbuff);
        if(!strcmp("LIST", scanfbuff)){
            printf("wysylam LIST...");
            messbuff.mtype = LIST;
            messbuff.who = myidonsrv;
            if(mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop) == -1){
                printf("failed\n");
                perror("error");
            }else{
                printf("sukces\n");
            }
            continue;
        }else if(!strcmp("STOP", scanfbuff)){
            sig_handler(2);
        }else if(!strcmp("CONNECT", scanfbuff)){
            int connect_to;
            printf("connect to: ");
            scanf("%d", &connect_to);
            messbuff.connect_to = connect_to;
            messbuff.mtype = CONNECT;
            messbuff.who = myidonsrv;
            mq_send(srvqID, (char *) &messbuff, MSG_SIZE, priop);
            if(my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop) > 0 && messbuff.mtype == CONNECT){
                int chatqID = mq_open(messbuff.key, O_RDWR);
                if(chatqID == -1){
                    perror("eroor otwierania chatu");
                }
                printf("otwieram chat_mode \n ");
                chat_mode(chatqID, 0);
                

            }
            
        }
        if(my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop) > 0){
            printf("odebrano komunikat %d\n", (int)messbuff.mtype);
            if(messbuff.mtype == STOP){
                sig_handler(2);
            }else if(messbuff.mtype == CONNECT){
                printf("przechodze w tyb chat\n");
                int chatqID = mq_open(messbuff.key, O_RDWR);
                if(chatqID == -1){
                    perror("eroor otwierania chatu");
                }
                chat_mode(chatqID, 1);                   
            }
        }
        else if(!strcmp("CHAT", scanfbuff)){
            printf("otwieram chat_mode \n ");
            if(my_mq_receive(myqID, (char *) &messbuff, MSG_SIZE, &priop) == -1){
                perror("errror cahtmode");
            }
            
            if(messbuff.mtype == CONNECT){
                printf("otwieram chat\n");
                int chatqID = mq_open(messbuff.key, O_RDWR);
                if(chatqID == -1){
                    perror("eroor otwierania chatu");
                }                
                chat_mode(chatqID, 1);
            }

            
        }
    }


    mq_close(myqID);
    mq_unlink(my_key);
  
} 