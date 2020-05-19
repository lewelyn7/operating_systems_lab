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

    shutdown(net_socket, SHUT_RDWR);
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
void add_new_client(struct epoll_event *evs, int epoll_fd){
        unsigned int size_struct = sizeof(struct sockaddr_un);
        int client_socket;
        int id = find_free(cls);

        printf("dodaje....");
        if(evs[0].data.fd == loc_socket){
            struct sockaddr_un client_addr;
            client_socket = accept(loc_socket,(struct sockaddr *) &client_addr, &size_struct);
        }
        if( evs[0].data.fd == net_socket){
            struct sockaddr_in client_addr;
            client_socket = accept(net_socket, (struct sockaddr *) &client_addr, &size_struct);
        }
        if(client_socket == -1){
            perror("klient accept blad\n");
        }
        cls[id].free = 0;
        cls[id].fd = client_socket;
        struct epoll_event eve;
        eve.data.fd = client_socket;
        eve.events = EPOLLIN | EPOLLRDHUP;
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &eve) == -1){
            perror("blad dodawania klienta epoll\n");
        }
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
        my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg));
        return -1;
    }

    struct ping msg;
    msg.v = START_GAME1;
    my_write(cls[id1].fd, (const void *) &msg, sizeof(msg));
    msg.v = START_GAME0;
    my_write(cls[id0].fd, (const void *) &msg, sizeof(msg));

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

int login_client(int sockid, struct epoll_event *evs){
    struct login msg;
    my_read(evs[0].data.fd, &msg, sizeof(msg));
    // bt_rcv = my_read(evs[1].data.fd, &msg, sizeof(msg));
    if(!is_login_free(msg.msg)){
        printf("odrzucono probe zalogowania: taki login juz istnieje\n");
        strcpy(msg.msg, "DEN");
        my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg));   
        return 0;     
    }
    strcpy(cls[sockid].login, msg.msg);
    printf("zalogowano nowego klienta: %s\n", msg.msg);
    cls[sockid].logged = 1;
    msg.id = sockid;
    strcpy(msg.msg, "ACK");
    my_write(cls[sockid].fd, (const void *) &msg, sizeof(msg));
    return 1;
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
                    my_write(cls[i].fd, (const void *) &msg, sizeof(msg));
                }else{
                    printf("client to deletion\n");
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


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, ping_thread, NULL);

    //create mutex
    if(pthread_mutex_init (&lock, NULL) != 0){
        perror("blad tworzenia mutexa\n");
        exit(-1);
    }

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
    net_socket = socket(AF_INET,SOCK_STREAM,0);
    if( net_socket == -1){
        perror("blad utworzenia socketu");
        exit(-1);
    }
    if(bind(net_socket,(const struct sockaddr *) &net_sockaddr, sizeof(net_sockaddr)) == -1){
        perror("blad bind remote");
        exit(-1);
    }

    if(listen(net_socket, CLIENT_MAX) == -1){
        perror("blad sluchania rmeote");
    }



    
    loc_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
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
    if(listen(loc_socket, CLIENT_MAX) == -1){
        perror("blad sluchania local");
    }    


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
    int bt_rcv;
    struct ping pmsg;
    int client_socket;
    unsigned int size_struct = sizeof(net_sockaddr);
    while(1){
        ev_rcv = epoll_wait(epoll_fd, evs, 1, -1);
        printf("\nodebralem %d  zdarzen\n", ev_rcv);

        if(evs[0].data.fd == loc_socket || evs[0].data.fd == net_socket){
            add_new_client(evs, epoll_fd);
        }else{

            int sockid = find_sock_id(cls, evs[0].data.fd);
            cls[sockid].active = 1;
            if( sockid == -1){
                printf("odebralem cos od klienta ktory nie istnieje\n");
            }

            if(evs[0].events & EPOLLRDHUP){
                printf("usunieto klienta: %s\n", cls[sockid].login);
                shutdown(cls[sockid].fd, SHUT_RDWR);
                close(cls[sockid].fd);
                cls[sockid].free = 1;
                cls[sockid].logged = 0;
                cls[sockid].pair_id = -1;
            }else if(cls[sockid].logged == 0){
                if(login_client(sockid, evs)){
                    pair_clients(sockid);
                }
            }else if(cls[sockid].pair_id != -1){ // 
                struct move mv;
                printf("ruch...\n");
                my_read(evs[0].data.fd, (void *) &mv, sizeof(mv));
                if(mv.flags == PONG){
                    printf("PONG\n");
                }else{
                    my_write(cls[cls[sockid].pair_id].fd, (const void *) &mv, sizeof(mv));
                    if(mv.flags == YOU_WIN || mv.flags == YOU_LOOSE){
                        printf("koniec gry\n");
                    }                    
                }

            }else{
                struct ping msg;
                my_read(evs[0].data.fd, (void*) &msg, sizeof(msg));
                if(msg.v == PONG){
                    printf("PONG\n");
                }
            }
            // sleep(1);
        }

    }
    // TODO sprawdzanie wolnych loginow

    // TODO remis

    // Sprawdzanie wolnych miejsc




}