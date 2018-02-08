#ifndef UNPIPC_H
#define UNPIPC_H

#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAXLINE 256

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define SERVER_FIFO "/tmp/fifo.serv"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  //default permissions for new fles
#define SVMSG_MODE 0600
#define DEBUG_PRINTF(format,...) printf(format, ##__VA_ARGS__);
#define DEBUG_PRINTF_RED(format,...) printf("\e[1;31m"format"\e[0m",##__VA_ARGS__)
#define DEBUG_PRINTF_GRE(format,...) printf("\e[1;32m"format"\e[0m",##__VA_ARGS__)

typedef void Sigfunc_rt(int,siginfo_t *,void*);

#ifdef __bsdi__
#define va_mode_t int
#else
#define va_mode_t mode_t
#endif 

struct my_msgbuf{
    long int mtype;
    char mtext[256];
};

#define MAXMSG (8192+sizeof(long))
#define MSG_W 0200
#define MSG_R 0400

#define MIN(x,y) (x>y)?y:x

#define read_lock(fd,offset,whence, len) \
        lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define readw_lock(fd,offset, whence,len) \
        lock_reg(fd, F_SETLKW, F_RDLCK,offset,whence,len)
#define write_lock(fd,offset,whence,len) \
        lock_reg(fd,F_SETLK, F_WRLCK, offset, whence, len)
#define writew_lock(fd,offset,whence,len) \
        lock_reg(fd,F_SETLKW, F_WRLCK, offset, whence, len)
#define un_lock(fd,offset,whence,len)   \
        lock_reg(fd,F_SETLK,F_UNLCK,offset,whence,len)
#define is_read_lockable(fd,offset,whence,len) \
        !lock_test(fd,F_RDLCK,offset,whence,len)
#define is_write_lockable(fd,offset,whence,len) \
        !lock_test(fd,F_WRLCK,offset,whence,len)
#endif
