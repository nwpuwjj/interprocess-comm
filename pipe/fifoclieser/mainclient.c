#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define MAXLINE 256

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define SERVER_FIFO "/tmp/fifo.serv"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  //default permissions for new fles

#define DEBUG_PRINTF(format,...) printf(format, ##__VA_ARGS__);
#define DEBUG_PRINTF_RED(format,...) printf("\e[1;31m"format"\e[0m",##__VA_ARGS__)
#define DEBUG_PRINTF_GRE(format,...) printf("\e[1;32m"format"\e[0m",##__VA_ARGS__)


int readline(int fd,char *buff, int maxnum)
{
    int result=-1;

    char c;
    int count=0;

    if(fd<0)
    {
        printf("[%s] open file error\n",__func__);
        return 0;
    }

    while((result=read(fd,&c,1))>0)
    {
        if(count==maxnum)
        {
            printf("[%s]==memory full\n",__func__);
            return count;
        }
        buff[count++]=c;
        if(c=='\n' || c==EOF)
        {
            break;
        }
    }
    buff[count]='\0';
    return count;
}

int main(int argc, char **argv)
{
    int readfifo,writefifo;
    size_t len;
    ssize_t n;
    char *ptr, fifoname[MAXLINE], buff[MAXLINE];

    pid_t pid;

    //create fifo with our pid as part of name
    pid=getpid();
    snprintf(fifoname,sizeof(fifoname),"/tmp/fifo.%ld",(long)pid);
    DEBUG_PRINTF("fifoname:%s===\n",fifoname);
    if((mkfifo(fifoname,FILE_MODE)<0) &&(errno!=EEXIST))
    {
        printf("can't create %s\n",fifoname);
        return -1;
    }

    //start buffer with pid and a blank
    snprintf(buff,sizeof(buff),"%ld ",(long) pid);
    len=strlen(buff);
    ptr=buff+len;

    //read pathname
    fgets(ptr,MAXLINE,stdin);
    len=strlen(buff);  //fgets() guarantees null byte at end

    writefifo=open(SERVER_FIFO,O_WRONLY,0);
    write(writefifo,buff,len);
    DEBUG_PRINTF_RED("[%s]===buff:%s===\n",__func__,buff);
    //now open our FIFO;blocks until server opens for writing
    readfifo=open(fifoname,O_RDONLY,0);
    DEBUG_PRINTF("[%s]====%d===\n",__func__,__LINE__); 
    //read from IPC,write to standard output
    while((n=read(readfifo,buff,MAXLINE))>0)
    {
        DEBUG_PRINTF_GRE("[%s]===buff:%s===\n",__func__,buff);
    }
    close(readfifo);
    unlink(fifoname);
    exit(0);
}

