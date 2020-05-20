#include "common.h"


struct config{
    int port;
    char unipath[SOCKET_MAX_LEN];
};


void sig_handler(int num){
    exit(0);
}

struct config cfg;
int loc_socket;
int net_socket;
struct client cls[CLIENT_MAX];
pthread_mutex_t lock;

void exit_handler(){
    printf("koncze prace i sprzatam\n");
    
    for(int i = 0; i < CLIENT_MAX; i++){
        if(cls[i].free == 0){
            shutdown(cls[i].fd, SHUT_RDWR);
            close(cls[i].fd);
        }
    }

    close(loc_socket);
    close(net_socket);
    unlink(cfg.unipath);
}

int find_free(struct client *cls){
    for(int i = 0; i < CLIENT_MAX; i++){
        if(cls[i].free == 1){
            return i;
        }
    }
    return -1;
}

int find_sock_id(struct client *cls, int sock){
    for(int i = 0; i < CLIENT_MAX; i++){
        if(cls[i].fd == sock){
            return i;
        }
    }
    return -1;
}
void clear_cls(struct client * cls){
    for(int i = 0; i < CLIENT_MAX; i++){
        cls[i].free = 1;
        cls[i].logged = 0;
        cls[i].pair_id = -1;
    }
}
void add_new_client(int client_socket, struct sockaddr * addr, int addrlen ){

        // struct sockaddr_un *addrun = calloc(sizeof(struct sockaddr_un), 1);
        // struct sockaddr_in *addrin = calloc(sizeof(struct sockaddr_in), 1);
        // if(addrlen == sizeof(addrun)){
        //     addrun = (struct sockaddr_un) *addr;
        // }else if(addrlen == sizeof(addrin)){
        //     addrin = (struct sockaddr_in) *addr;
        // }

        // unsigned int size_struct = sizeof(struct sockaddr_un);
        int id = find_free(cls);

        // struct sockaddr_un addrun;
        // struct sockaddr_in addrin;
        // if(addrlen == sizeof(addrun)){
        //     addrun = (struct sockaddr_un) (*addr);
        // }else if(addrlen == sizeof(addrin)){
        //     addrin = (struct sockaddr_in) (*addr);
        // }

        cls[id].addrsize = addrlen;
        cls[id].addr = calloc(addrlen, 1);
        *cls[id].addr = *addr;

        printf("dodaje....");
        // if(evs[0].data.fd == loc_socket){
        //     struct sockaddr_un client_addr;
        //     client_socket = accept(loc_socket,(struct sockaddr *) &client_addr, &size_struct);
        // }
        // if( evs[0].data.fd == net_socket){
        //     struct sockaddr_in client_addr;
        //     client_socket = accept(net_socket, (struct sockaddr *) &client_addr, &size_struct);
        // }
        // if(client_socket == -1){
        //     perror("klient accept blad\n");
        // }
        cls[id].free = 0;
        cls[id].fd = client_socket;
        struct ping msg;
        msg.who = id;
        msg.v = HELLO;
        my_write(cls[id].fd, (const void *) &msg, sizeof(msg), 0, cls[id].addr, cls[id].addrsize);

        // struct epoll_event eve;
        // eve.data.fd = client_socket;
        // eve.events = EPOLLIN | EPOLLRDHUP;
        // if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &eve) == -1){
        //     perror("blad dodawania klienta epoll\n");
        // }
        printf("dodano nowego klienta\n");
}

int pair_clients(int sockid){
    int id0 = -1;
    int id1 = -1;

    for(int i = 0; i < CLIENT_MAX; i++){
        if(cls[i].free == 0 && cls[i].logged == 1 && cls[i].pair_id == -1){
            if(id1 == -1){
                id1 = i;
            }else{
                id0 = i;
                break;
            }
        }
    }
    if(id1 == -1 || id0 == -1){
        struct ping msg;
        msg.v = WAITING_FOR_PLAYER;
        my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg),0, cls[sockid].addr, cls[sockid].addrsize);
        return -1;
    }

    struct ping msg;
    msg.v = START_GAME1;
    my_write(cls[id1].fd, (const void *) &msg, sizeof(msg), 0,cls[id1].addr, cls[id1].addrsize);
    msg.v = START_GAME0;
    my_write(cls[id0].fd, (const void *) &msg, sizeof(msg), 0,cls[id0].addr, cls[id0].addrsize);

    cls[id1].pair_id = id0;
    cls[id0].pair_id = id1;

    printf("sparowano klientow");
    return 1;

}

int is_login_free(char * name){
    for(int i = 0; i < CLIENT_MAX; i++){
        if(cls[i].free == 0 && cls[i].logged == 1){
            if(!strcmp(cls[i].login, name)){
                return 0;
            }
        }
    }
    return 1;
}

int login_client(int sockid, struct login msg){
    // bt_rcv = my_read(evs[1].data.fd, &msg, sizeof(msg));
    if(!is_login_free(msg.msg)){
        printf("odrzucono probe zalogowania: taki login juz istnieje\n");
        strcpy(msg.msg, "DEN");
        my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg), 0, cls[sockid].addr, cls[sockid].addrsize);
        return 0;     
    }
    strcpy(cls[sockid].login, msg.msg);
    printf("zalogowano nowego klienta: %s\n", msg.msg);
    cls[sockid].logged = 1;
    msg.id = sockid;
    strcpy(msg.msg, "ACK");
    my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg), 0, cls[sockid].addr, cls[sockid].addrsize);
    return 1;
}
void delete_client(int sockid){
    printf("usunieto klienta: %s\n", cls[sockid].login);
    cls[sockid].free = 1;
    cls[sockid].logged = 0;
    if(cls[sockid].pair_id != -1 && cls[cls[sockid].pair_id].pair_id != -1){
        struct ping msg;
        msg.v = SHUTDOWN;
        my_write(cls[cls[sockid].pair_id].fd, (const void*) &msg, sizeof(msg), 0, cls[cls[sockid].pair_id].addr, cls[cls[sockid].pair_id].addrsize);
    }
    cls[sockid].pair_id = -1;

}
void * ping_thread(void * arg){

    struct ping msg;
    msg.v = PING;

    while(1){
        pthread_mutex_lock(&lock);
        printf("pinging..\n");
        for(int i = 0; i < CLIENT_MAX; i++){

            if(cls[i].free == 0 && cls[i].logged == 1){
                if(cls[i].active == 1){
                    cls[i].active =  0;
                    my_write(cls[i].fd, (const void *) &msg, sizeof(msg), 0, cls[i].addr, cls[i].addrsize);
                }else{
                    delete_client(i);
                }
            }

        }
        pthread_mutex_unlock(&lock);
        sleep(3);        
    }

    return NULL;
}

int main(int argc, char ** argv){
    clear_cls(cls);

    //create mutex
    if(pthread_mutex_init (&lock, NULL) != 0){
        perror("blad tworzenia mutexa\n");
        exit(-1);
    }
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, ping_thread, NULL);



    atexit(exit_handler);
    signal(SIGINT, sig_handler);
    if(argc != 3){
        printf("bledna ilosc argumentow: port unix_path");
        exit(-1);
    }
    sscanf(argv[1], "%d", &cfg.port);
    strcpy(cfg.unipath, argv[2]);


    struct sockaddr_in net_sockaddr;
    struct in_addr net_inaddr;
    net_sockaddr.sin_family = AF_INET;
    net_sockaddr.sin_port = htons(cfg.port);
     inet_aton("127.0.0.1", &net_inaddr);
    net_sockaddr.sin_addr.s_addr = net_inaddr.s_addr;
    net_socket = socket(AF_INET,SOCK_DGRAM,0);
    if( net_socket == -1){
        perror("blad utworzenia socketu");
        exit(-1);
    }
    if(bind(net_socket,(const struct sockaddr *) &net_sockaddr, sizeof(net_sockaddr)) == -1){
        perror("blad bind remote");
        exit(-1);
    }

    // if(listen(net_socket, CLIENT_MAX) == -1){
    //     perror("blad sluchania rmeote");
    // }



    
    loc_socket = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(loc_socket == -1){
        perror("blad tworzenia lokalnego gniazda");
        exit(-1);
    }
    struct sockaddr_un  loc_sockaddr;
    loc_sockaddr.sun_family = AF_UNIX;
    strcpy(loc_sockaddr.sun_path, cfg.unipath);
    if(bind(loc_socket,(const struct sockaddr *) &loc_sockaddr, sizeof(loc_sockaddr)) == -1){
        perror("blad bind lokalnego");
        exit(-1);
    }
    // if(listen(loc_socket, CLIENT_MAX) == -1){
    //     perror("blad sluchania local");
    // }    


    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1){
        perror("epoll create blad");
        exit(-1);
    }
    struct epoll_event ev1;
    ev1.data.fd = loc_socket;
    ev1.events = EPOLLIN | EPOLLRDHUP;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, loc_socket, &ev1) == -1){
        perror("epoll ctl blad");
        exit(-1);
    }
    struct epoll_event ev2;
    ev2.data.fd = net_socket;
    ev2.events = EPOLLIN | EPOLLRDHUP;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, net_socket, &ev2) == -1){
        perror("epoll ctl blad");
        exit(-1);        
    }


    struct epoll_event evs[CLIENT_MAX];
    int ev_rcv;
    // int bt_rcv;
    struct ping pmsg;
    struct move mv;
    // int client_socket;
    // struct sockaddr_un unix_addr;
    struct sockaddr net_addr;
    // struct sockaddr * ptr_addr;
    unsigned int addrlen = sizeof(struct sockaddr);


    // unsigned int size_struct = sizeof(net_sockaddr);
    while(1){
        ev_rcv = epoll_wait(epoll_fd, evs, 1, -1);
        printf("\nodebralem %d  zdarzen\n", ev_rcv);
        pthread_mutex_lock(&lock);

            // add_new_client(evs, epoll_fd);

            addrlen = sizeof(net_addr);
            my_read(evs[0].data.fd, &mv, sizeof(mv), MSG_PEEK, &net_addr, &addrlen);
            // ptr_addr = (struct sockaddr * )&net_addr; 
            int sockid = mv.who;
            if(mv.who == NOBODY){
                my_read(evs[0].data.fd, &pmsg, sizeof(pmsg), 0, &net_addr, &addrlen);
                add_new_client(evs[0].data.fd, &net_addr, addrlen);
            }else if(cls[sockid].logged == 0 && mv.place != SHUTDOWN){
                struct login lmsg;
                my_read(evs[0].data.fd, &lmsg, sizeof(lmsg), 0, &net_addr, &addrlen);
                if(login_client(sockid, lmsg) == 1){
                    pair_clients(sockid);
                }

                cls[sockid].active = 1;
            }else{

                cls[sockid].active = 1;

                my_read(evs[0].data.fd, &mv, sizeof(mv), 0, &net_addr, &addrlen);

                if(evs[0].events & EPOLLRDHUP || mv.place == SHUTDOWN){
                    delete_client(sockid);
                }else if(cls[sockid].pair_id != -1){ // 
                    printf("ruch...\n");
                    if(mv.flags == PONG){
                        printf("PONG\n");
                    }else{
                        my_write(cls[cls[sockid].pair_id].fd, (const void *) &mv, sizeof(mv), 0, cls[cls[sockid].pair_id].addr, cls[cls[sockid].pair_id].addrsize);
                        if(mv.flags == YOU_WIN || mv.flags == YOU_LOOSE){
                            printf("koniec gry\n");
                        }                    
                    }

                }else{
                    // struct ping msg;
                    // my_read(evs[0].data.fd, (void*) &msg, sizeof(msg), 0, cls[sockid].addr, &cls[sockid].addrsize);
                    if(mv.place == PONG){
                        printf("PONG\n");
                    }
                }
            }
            // sleep(1);
        pthread_mutex_unlock(&lock);


    }
    // TODO

    // Sprawdzanie wolnych miejsc




}