// This program is an example of how to run a command such as
// ps aux | grep root | grep sbin
// using C and Unix.

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int pid;
int fd1[2];
int fd2[2];



void exec1() {
  // input from stdin (already done)
  // output to fd1
  dup2(fd1[1], 1);
  // close fds
  close(fd1[0]);
  close(fd1[1]);
  // exec
  execlp("ps", "ps", "aux", NULL);
  // exec didn't work, exit
  perror("bad exec ps");
  _exit(1);
}

void exec2() {
  // input from fd1
  dup2(fd1[0], 0);
  // output to fd2
  dup2(fd2[1], 1);
  // close fds
  close(fd1[0]);
  close(fd1[1]);
  close(fd2[0]);
  close(fd2[1]);
  // exec
  execlp("grep", "grep", "root", NULL);
  // exec didn't work, exit
  perror("bad exec grep root");
  _exit(1);
}

void exec3() {
  // input from fd2
  dup2(fd2[0], 0);
  // output to stdout (already done)
  // close fds
  close(fd2[0]);
  close(fd2[1]);
  // exec
  execlp("grep", "grep", "sbin", NULL);
  // exec didn't work, exit
  perror("bad exec grep sbin");
  _exit(1);
}

int main() {

  // create fd1
  if (pipe(fd1) == -1) {
    perror("bad fd1");
    exit(1);
  }

  // fork (ps aux)
  if ((pid = fork()) == -1) {
    perror("bad fork1");
    exit(1);
  } else if (pid == 0) {
    // stdin --> ps --> fd1
    exec1();
  }
  // parent

  // create fd2
  if (pipe(fd2) == -1) {
    perror("bad fd2");
    exit(1);
  }

  // fork (grep root)
  if ((pid = fork()) == -1) {
    perror("bad fork2");
    exit(1);
  } else if (pid == 0) {
    // fd1 --> grep --> fd2
    exec2();
  }
  // parent

  // close unused fds
  close(fd1[0]);
  close(fd1[1]);

  // fork (grep sbin)
  if ((pid = fork()) == -1) {
    perror("bad fork3");
    exit(1);
  } else if (pid == 0) {
    // fd2 --> grep --> stdout
    exec3();
  }
  // parent


}
