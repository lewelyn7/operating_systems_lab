#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define _BSD_SOURCE
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <endian.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>
#include <pthread.h>

#define LOGIN_MAX_LEN 32
#define CLIENT_MAX 15
#define SOCKET_MAX_LEN 40
#define print(x) write(STDOUT_FILENO, x, sizeof(x))
struct board{
    // 1 for x, 0 for O
    int v[9];
};


#define YOU_LOOSE 5
#define YOU_WIN 4
#define DRAW 7
struct move{
    int place;
    int value;
    int flags;
};


#define PING 10
#define PONG 20
#define START_GAME1 51
#define START_GAME0 52
#define WAITING_FOR_PLAYER 43
#define SHUTDOWN 98
struct ping{
    int v;
};

struct login{
    char msg[LOGIN_MAX_LEN];
    int id;
};


struct client{
    char login[LOGIN_MAX_LEN];
    int fd;
    int free;
    int logged;
    int pair_id;

    int active;
};

int check_board(struct board *brd){
    for(int i = 1; i < 9; i+=3){
        if(brd->v[i] == brd->v[i-1] && brd->v[i-1] == brd->v[i+1]){
            return brd->v[i];
        }
    }

    for(int i = 3; i < 6; i++){
        if(brd->v[i] == brd->v[i-3] && brd->v[i-3] == brd->v[i+3]){
            return brd->v[i];
        }
    }

    if(brd->v[0] == brd->v[4] && brd->v[4] == brd->v[8]){
        return brd->v[0];
    }

    if(brd->v[2] == brd->v[4] && brd->v[4] == brd->v[6]){
        return brd->v[2];
    }

    int cnt = 0;
    for(int i =0; i < 9; i++){
        if(brd->v[i] == 1 || brd->v[i] == 0){
            cnt++;
        }
    }
    if(cnt == 9){
        return DRAW;
    }
    return -1;
    
}

int my_write(int sockfd, const void *buf, size_t len){
    int val = write(sockfd, buf, len);
    
    // printf("wyslano %d bajtow\n", val);

    if(val == -1){
        perror("blad wysylania\n");
    }
    return val;
}

int my_read(int sockfd, void *buf, size_t len){
    int val = recv(sockfd, buf, len, 0);

    if(val == -1){
        perror("blad odbierania\n");
    }
    return val;
}



void render_board(struct board *brd){
    for(int i = 0; i < 3; i++){
        
        for(int j = 0; j < 3; j++){

            if(brd->v[i*3+j] == 1){
                printf("x|");
            }else if(brd->v[i*3+j] == 0){
                printf("o|");
            }else{
                printf("-|");
            }
            
        }
        printf("\n-----\n");

    }
}
