#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include "common.h"


#define MAX_NUMBER_OF_CLIENTS 10

struct clients clids;
int qID;
struct msqid_ds buff;
struct msgbuf messbuff;
unsigned int priop = 1;
const char * home = "/XDSDFSDFS";

struct client_params{
    int qID;
    int available;
    int deleted;
    char key[64];
};
struct clients{
    struct client_params id[MAX_NUMBER_OF_CLIENTS];
    int next_id;
};

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

void clean_up(){

    messbuff.mtype = STOP;
    for(int i = 0; i < clids.next_id; i++){
        if(clids.id[i].deleted == 0){
            if(mq_send(clids.id[i].qID, (char *) &messbuff, MSG_SIZE, 8) == -1){
                perror("blad wysylania komunikatu STOP ");
                clids.id[i].deleted = 1;
            }else{
                printf("wysylam STOP do %d\n", messbuff.who);
            }
        }
    }
    for(int i = 0; i < clids.next_id; i++){
        if(clids.id[i].deleted == 1) 
            continue;

        if(my_mq_receive(qID, (char *) &messbuff, MSG_SIZE, NULL) == -1){
            perror("blad odbierania komunikatu STOP ");
        }else{
            printf("odebrano STOP od %d\n", messbuff.who);
        }
    }
    printf("zamykam swoja kolejke\n");
    mq_close(qID);
    mq_unlink(home);

}

void sig_handler(int num){
    printf("Otrzymałem: %d\n", num);
    exit(0);
}


int main(int argc, char ** argv){



    struct mq_attr attry;
    attry.mq_flags = 0;
    attry.mq_maxmsg = 8;
    attry.mq_msgsize = MSG_SIZE;
    attry.mq_curmsgs = 8;
    if(mq_unlink(home) == 0)
        fprintf(stdout, "Message queue %s removed from system.\n", home);
    clids.next_id = 0;
    signal(SIGINT, sig_handler);
    atexit(clean_up);

    // qID = q(ftok(home, 'A'), IPC_CREAT | S_IRWXU);
    qID = mq_open(home,O_CREAT | O_RDONLY, 0620, &attry);
    //msgctl(qID, IPC_RMID, &buff);

    if(qID == -1){
        perror("kolejka o podanym kluczu juz istnieje");
        exit(-1);
    }    
    if(errno == EEXIST){
        perror("kolejka juz istnieje");
        exit(-1);
    }  

    int size_received = -1;
    
    while(1){

        printf("czekam...\n");

        size_received = my_mq_receive(qID, (char *)  &messbuff, MSG_SIZE, NULL);

        if(size_received > 0){
            printf("odebrano\n");
            if(messbuff.mtype == INIT){
                if(clids.next_id < MAX_NUMBER_OF_CLIENTS){
                    
                    char client_key[64];
                    
                    strcpy(client_key, messbuff.key);
                    strcpy(clids.id[clids.next_id].key, client_key);
                    // clids.id[clids.next_id].qID = msgget(client_key, 0);
                    clids.id[clids.next_id].qID = mq_open(messbuff.key,O_WRONLY);
                    clids.id[clids.next_id].available = 1;
                    clids.id[clids.next_id].deleted = 0;
                    if(clids.id[clids.next_id].qID != -1){
                        printf("dodano nowego kienta\n");
                    }else{
                        perror("blad dodawania nowego klienta");
                    }
                    messbuff.mtype = INIT;

                    
                    sprintf(messbuff.mtext, "%d", clids.next_id);
                    
                     if (mq_send(clids.id[clids.next_id].qID, (char *) &messbuff, MSG_SIZE, 1) < 0){
                         perror("blad wysylania");
                     }
                    clids.next_id++;                    
               
               }else{

                   printf("nie moge obslugiwac kolejnego klienta przeciez \n");

               }

            }else if( messbuff.mtype == LIST){
                for(int i =0; i < clids.next_id; i++){
                    if(clids.id[i].deleted == 0)
                        printf("ID: %d, key: %s, qID: %d, availability: %d deleted: %d \n", i, clids.id[i].key, clids.id[i].qID, clids.id[i].available, clids.id[i].deleted);
                }
            }else if( messbuff.mtype == CONNECT){
                clids.id[messbuff.who].available = 0;
                clids.id[messbuff.connect_to].available = 0;
                printf("lacze %d do %d \n", messbuff.who, messbuff.connect_to);

                strcpy(messbuff.key, clids.id[messbuff.connect_to].key);

                messbuff.mtype = CONNECT;
                if(mq_send(clids.id[messbuff.who].qID,(char *) &messbuff, MSG_SIZE, 1) == -1){
                    perror("blad wyslania komunikatu do");
                    printf("%d", messbuff.who);
                    exit(-1);
                }                

                strcpy( messbuff.key, clids.id[messbuff.who].key);
                messbuff.mtype = CONNECT;
                if(mq_send(clids.id[messbuff.connect_to].qID,(char *) &messbuff, MSG_SIZE, 1) == -1){
                    perror("blad wyslania komunikatu do");
                    printf("%d", messbuff.connect_to);
                    exit(-1);
                } 
                printf("wyslalem polaczenia\n");

            }else if(messbuff.mtype == DISCONNECT){
                printf("disconnect client %d\n", messbuff.who);
                clids.id[messbuff.who].available = 1;
            }else if(messbuff.mtype == STOP){
                printf("clearing client %d\n", messbuff.who);
                mq_close(clids.id[messbuff.who].qID);
                //TODO ZAMYKANIE
                clids.id[messbuff.who].deleted = 1;
            }
        }
    }
}