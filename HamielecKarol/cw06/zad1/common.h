#define LIST 5
#define CONNECT 1
#define DISCONNECT 2
#define STOP 3
#define INIT 4
#define MSG_SIZE 32
#define CHAT 6

#define MTEXT_SIZE 32-12

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[MTEXT_SIZE];    /* message data */
    int who;
    int connect_to;
    key_t key;
};