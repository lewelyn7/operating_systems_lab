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


#define MAX_NUMBER_OF_CLIENTS 10

struct clients clids;
int qID;
struct msqid_ds buff;
struct msgbuf messbuff;

struct client_params{
    int qID;
    int available;
    int deleted;
    key_t key;
};
struct clients{
    struct client_params id[MAX_NUMBER_OF_CLIENTS];
    int next_id;
};

void clean_up(){

    messbuff.mtype = STOP;
    for(int i = 0; i < clids.next_id; i++){
        if(clids.id[i].deleted == 0){
            if(msgsnd(clids.id[i].qID, &messbuff, MSG_SIZE, 0) == -1){
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

        if(msgrcv(qID, &messbuff, MSG_SIZE, STOP, 0) == -1){
            perror("blad odbierania komunikatu STOP ");
        }else{
            printf("odebrano STOP od %d\n", messbuff.who);
        }
    }
    printf("zamykam swoja kolejke\n");
    msgctl(qID, IPC_RMID, &buff);

}

void sig_handler(int num){
    printf("Otrzymałem: %d\n", num);
    exit(0);
}

int main(int argc, char ** argv){

    const char * home = getenv("HOME");

    clids.next_id = 0;
    signal(SIGINT, sig_handler);
    atexit(clean_up);

    qID = msgget(ftok(home, 'A'), IPC_CREAT | S_IRWXU);
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
        size_received = msgrcv(qID, &messbuff, MSG_SIZE, -10, 0);
        if(size_received > 0){
            if(messbuff.mtype == INIT){
                if(clids.next_id < MAX_NUMBER_OF_CLIENTS){
                    
                    key_t client_key;
                    
                    sscanf(messbuff.mtext, "%d", &client_key);
                    clids.id[clids.next_id].key = client_key;
                    clids.id[clids.next_id].qID = msgget(client_key, 0);
                    clids.id[clids.next_id].available = 1;
                    clids.id[clids.next_id].deleted = 0;
                    if(clids.id[clids.next_id].qID != -1){
                        printf("dodano nowego kienta\n");
                    }else{
                        perror("blad dodawania nowego klienta");
                    }
                    messbuff.mtype = INIT;

                    
                    sprintf(messbuff.mtext, "%d", clids.next_id);
                    
                     if (msgsnd(clids.id[clids.next_id].qID, &messbuff, MSG_SIZE, 0) < 0){
                         perror("blad wysylania");
                     }
                    clids.next_id++;                    
               
               }else{

                   printf("nie moge obslugiwac kolejnego klienta przeciez \n");

               }

            }else if( messbuff.mtype == LIST){
                for(int i =0; i < clids.next_id; i++){
                    if(clids.id[i].deleted == 0)
                        printf("ID: %d, key: %d, qID: %d, availability: %d deleted: %d \n", i, (int)clids.id[i].key, clids.id[i].qID, clids.id[i].available, clids.id[i].deleted);
                }
            }else if( messbuff.mtype == CONNECT){
                clids.id[messbuff.who].available = 0;
                clids.id[messbuff.connect_to].available = 0;
                printf("lacze %d do %d \n", messbuff.who, messbuff.connect_to);

                messbuff.key = clids.id[messbuff.connect_to].key;

                messbuff.mtype = CONNECT;
                if(msgsnd(clids.id[messbuff.who].qID,&messbuff, MSG_SIZE, 0) == -1){
                    perror("blad wyslania komunikatu do");
                    printf("%d", messbuff.who);
                    exit(-1);
                }                

                messbuff.key = clids.id[messbuff.who].key;
                messbuff.mtype = CONNECT;
                if(msgsnd(clids.id[messbuff.connect_to].qID,&messbuff, MSG_SIZE, 0) == -1){
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
                msgctl(clids.id[messbuff.who].qID, IPC_RMID, &buff);
                clids.id[messbuff.who].deleted = 1;
            }
        }
    }
}