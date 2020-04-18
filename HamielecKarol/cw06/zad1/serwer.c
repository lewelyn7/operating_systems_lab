#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>

#include "common.h"

#define MAX_NUMBER_OF_CLIENTS 10
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




int main(int argc, char ** argv){

    const char * home = getenv("HOME");
    struct msqid_ds buff;
    struct msgbuf messbuff;
    struct clients clids;
    clids.next_id = 0;


    int qID = msgget(ftok(home, 'A'), IPC_CREAT | S_IRWXU);
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
        size_received = msgrcv(qID, &messbuff, MSG_SIZE, 0, 0);
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
                    printf("ID: %d, key: %d, qID: %d, availability: %d\n deleted: %d", i, (int)clids.id[i].key, clids.id[i].qID, clids.id[i].available, clids.id[i].deleted);
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
    msgctl(qID, IPC_RMID, &buff);
}