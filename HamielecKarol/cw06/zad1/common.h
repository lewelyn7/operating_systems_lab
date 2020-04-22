#define LIST 3
#define CONNECT 4
#define DISCONNECT 2
#define STOP 1
#define INIT 5
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