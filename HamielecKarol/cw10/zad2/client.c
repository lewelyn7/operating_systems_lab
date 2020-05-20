#include "common.h"

#define UNIXT 1
#define NETT 2

struct config{
    char login[LOGIN_MAX_LEN];
    int type;
    char addr[SOCKET_MAX_LEN];
    int port;
};
struct config cfg;
int sock_fd;
int my_id;
struct sockaddr * srv_sockaddr;
struct sockaddr * my_sockaddr;
unsigned int srv_sockaddr_size;
unsigned int my_sockaddr_size;
char sokname[30];

struct sockaddr rcvaddr;
unsigned int rcvaddr_size;

void exit_func(void){
    printf("koncze prace i sprzatam\n");
    // shutdown(sock_fd, SHUT_RDWR);
    struct move msg;
    msg.place = SHUTDOWN;
    msg.who = my_id;
    my_write(sock_fd, (const void*) &msg, sizeof(msg), 0, srv_sockaddr, srv_sockaddr_size);
    close(sock_fd);
    unlink(sokname);
}
void sig_handler(int num){
    exit(-1);
}


void start_game(int who){
    int my_sign;
    struct board brd;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            brd.v[i*3+j] = -1;
        }
    }
    struct move mv;
    mv.flags = 0;

    my_sign = who;

    int pmsg_size = sizeof(struct ping);

    int game = 2;
    int byte_rcv = 0;
        while(game){
            if(who == 1 || game == 1){
                printf("czekam na ruch przeciwnika\n");
                do{
                    byte_rcv = my_read(sock_fd, (void *) &mv, sizeof(mv), 0, NULL, NULL);
                    if(byte_rcv == pmsg_size){
                        if(mv.place == PING){
                            struct move mvv;
                            mvv.who = my_id;
                            mvv.flags = PONG;
                            my_write(sock_fd, (const void*) &mvv, sizeof(mvv), 0, srv_sockaddr, srv_sockaddr_size);
                        }else if(mv.place == SHUTDOWN){
                            return;
                        }
                    }
                }while(pmsg_size == byte_rcv);

                if(mv.flags == YOU_LOOSE){
                    printf("przegrales :( \n");
                    return;
                }
                if(mv.flags == YOU_WIN){
                    printf("wygrales \n");
                    return;
                }
                if(mv.flags == DRAW){
                    printf("remis\n");
                    return;
                }
                brd.v[mv.place] = mv.value;
                game = 1;
                render_board(&brd);
            }
            if(who == 0 || game == 1){
                int readmv;
                printf("podaj ruch: \n");

                int epoll_fd = epoll_create1(0);
                struct epoll_event ev1;
                ev1.events = EPOLLIN;
                ev1.data.fd = STDIN_FILENO;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev1) == -1){
                    perror("epoll ctl blad");
                    exit(-1);        
                }
                ev1.data.fd = sock_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev1) == -1){
                    perror("epoll ctl blad");
                    exit(-1);
                }
                
                do{
                    epoll_wait(epoll_fd, &ev1, 1, -1);
                    if(ev1.data.fd == sock_fd){
                        struct ping pm;
                        my_read(sock_fd, (void*) &pm, sizeof(pm), 0, NULL, NULL);
                        if(pm.v == PING){
                            struct move mvv;
                            mvv.flags = PONG;
                            mvv.who = my_id;
                            my_write(sock_fd, (const void*) &mvv, sizeof(mvv), 0, srv_sockaddr, srv_sockaddr_size);                              
                        }else if(pm.v == SHUTDOWN){
                            return;
                        }
                    }
                }while(ev1.data.fd != STDIN_FILENO);

                scanf("%d", &readmv);
                readmv--;
                if(readmv < 0 || readmv > 8 || brd.v[readmv] == 0 || brd.v[readmv] == 1){
                    printf("niedozwolny ruch. tracisz kolejke :(\n");
                    mv.value = brd.v[readmv];
                    mv.place = readmv;

                }else{
                    brd.v[readmv] = my_sign;
                    mv.place = readmv;
                    mv.value = my_sign;
                }
                

                game = 1;
                render_board(&brd);
                if(check_board(&brd) == my_sign){
                    printf("wygrales !!!\n");
                    mv.flags = YOU_LOOSE;
                }else if(check_board(&brd) == DRAW){
                    printf("remis\n");
                    mv.flags = DRAW;
                }else if(check_board(&brd) != -1){
                    printf("przegrales :( \n");
                    mv.flags = YOU_WIN;
                }
                mv.who = my_id;
                my_write(sock_fd, (const void *) &mv, sizeof(mv), 0, srv_sockaddr, srv_sockaddr_size);
                if(mv.flags == YOU_WIN || mv.flags == YOU_LOOSE || mv.flags == DRAW){
                    printf("koniec gry\n");
                    return;
                }

            }
    }


}

int main(int argc, char ** argv){

    rcvaddr_size = sizeof(rcvaddr);

    atexit(exit_func);
    signal(SIGINT, sig_handler);

    if(argc != 4 && argc != 5){
        printf("bledna liczba argumentow");
        exit(-1);
    }
    sscanf(argv[1], "%s", cfg.login);
    if(!strcmp(argv[2], "unix")){
        cfg.type = unix;
        sscanf(argv[3], "%s", cfg.addr);
        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un loc_sockaddr;
        srv_sockaddr = (struct sockaddr*) &loc_sockaddr;
        srv_sockaddr_size = sizeof(loc_sockaddr);
        loc_sockaddr.sun_family = AF_UNIX;
        strcpy(loc_sockaddr.sun_path, cfg.addr);


        struct sockaddr_un my_unix_sockaddr;
        my_sockaddr = (struct sockaddr*) &my_unix_sockaddr;
        my_unix_sockaddr.sun_family = AF_UNIX;
        sprintf(sokname, "sok%d", (int)getpid());        
        strcpy(my_unix_sockaddr.sun_path, sokname);
        my_sockaddr_size = sizeof(my_unix_sockaddr);
        if(-1 == bind(sock_fd, my_sockaddr, my_sockaddr_size)){
            perror("bind error\n");
            exit(-1);
        }

    }else{
        cfg.type = NETT;
        sscanf(argv[3], "%s", cfg.addr);
        sscanf(argv[4], "%d", &cfg.port);
        sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in net_sockaddr;
        srv_sockaddr = (struct sockaddr*) &net_sockaddr;
        srv_sockaddr_size = sizeof(net_sockaddr);
        net_sockaddr.sin_family = AF_INET;
        net_sockaddr.sin_port = htons(cfg.port);
        inet_pton(AF_INET, cfg.addr, &net_sockaddr.sin_addr);

        struct sockaddr_in my_net_sockaddr;
        my_sockaddr = (struct sockaddr*) &my_net_sockaddr;
        my_net_sockaddr.sin_family = AF_INET;
        my_net_sockaddr.sin_port = 0;
        my_sockaddr_size = sizeof(my_net_sockaddr);
        inet_pton(AF_INET, "127.0.0.1", &net_sockaddr.sin_addr);


    }

        struct ping msg;
        msg.v = HELLO;
        msg.who = NOBODY;
        my_write(sock_fd, (const void *) &msg, sizeof(msg), 0, srv_sockaddr, srv_sockaddr_size);
        my_read(sock_fd,(void *) &msg, sizeof(msg), 0, NULL, NULL);
        if(msg.v == HELLO){
            my_id = msg.who;
        }

        struct login lmsg;
        lmsg.who = my_id;
        strcpy(lmsg.msg, cfg.login);
        my_write(sock_fd, (const void*) &lmsg, sizeof(lmsg), 0, srv_sockaddr, srv_sockaddr_size);
        my_read(sock_fd,(void *) &lmsg, sizeof(lmsg), 0, NULL, NULL);
        if(strcmp(lmsg.msg, "ACK")){
            printf("serwer odrzucil login\n");
            exit(-1);
        }
        printf("zalogowano\n");
        // my_id = lmsg.id;

        while(1){
            my_read(sock_fd, (void *) &msg, sizeof(msg), 0, NULL, NULL);
            if(msg.v == PING){
                msg.v = PONG;
                my_write(sock_fd, (const void *) &msg, sizeof(msg), 0, srv_sockaddr, srv_sockaddr_size);
            }else if(msg.v == START_GAME1 || msg.v == START_GAME0){
                if(msg.v == START_GAME1){
                    start_game(1);
                }else{
                    start_game(0);
                }
                exit(0);
            }else if(msg.v == WAITING_FOR_PLAYER){
                printf("nie ma dla ciebie graczy w tym momencie\n");
            }
        }
    


    


}


