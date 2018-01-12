#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 256

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  //default permissions for new fles

void server(int,int);

int main(int argc, char **argv)
{
    int readfd,writefd;

    //create two FIFOs; OK if they already exist
    if((mkfifo(FIFO1,FILE_MODE)<0) && (errno !=EEXIST))
    {
        printf("[server_main]can't create %s===\n",FIFO1);
        return -1;
    }

    if((mkfifo(FIFO2,FILE_MODE)<0) && (errno!=EEXIST))
    {
        unlink(FIFO1);
        printf("[server_main]can't create %s===\n",FIFO2);
        return -1;
    }

    readfd=open(FIFO1,O_RDONLY,0);
    writefd=open(FIFO2,O_WRONLY,0);

    server(readfd,writefd);
    exit(0);
}

void server(int readfd, int writefd)
{
    int fd;
    ssize_t n;
    char buff[MAXLINE+1];

    //read pathname from IPC channel
    if((n=read(readfd,buff,MAXLINE))==0)
    {
        printf("[%s]==end-of-file while reading pathname",__func__);
        exit(0);
    }

    buff[n]='\0'; //add null terminate pathname

    if((fd=open(buff,O_RDONLY))<0)
    {
        snprintf(buff+n,sizeof(buff)-n,":can't open,%s\n",strerror(errno));
        n=strlen(buff);
        write(writefd,buff,n);
    }
    else
    {
        //open succeeded: copy file to IPC channel
        while((n=read(fd,buff,MAXLINE))>0)
            write(writefd,buff,n);

        close(fd);
    }
}
