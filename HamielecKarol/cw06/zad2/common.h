#define LIST 3
#define CONNECT 4
#define DISCONNECT 2
#define STOP 1
#define INIT 5
#define MSG_SIZE sizeof(struct msgbuf)
#define CHAT 6

#define MTEXT_SIZE 8

struct msgbuf {
    long mtype;       //8
    char mtext[MTEXT_SIZE];    //MTEXT_SIZE
    int who;    // 4
    int connect_to; // 4
    char key[10]; // 16
};

