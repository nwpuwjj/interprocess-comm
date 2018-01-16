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

void server(int,int);

int main(int argc, char **argv)
{
    int readfifo,writefifo, dumyfd, fd;
    char *ptr,buff[MAXLINE+1],fifoname[MAXLINE];

    pid_t pid;
    ssize_t n;

    //*create server's well-known fifo; ok if already exists
    if((mkfifo(SERVER_FIFO,FILE_MODE) <0) && (errno != EEXIST))
    {
        printf("can't create %s",SERVER_FIFO);
        return -1;
    }

    //open server's well-know fifo for reading and writing
    readfifo=open(SERVER_FIFO,O_RDONLY,0);
    printf("===[%s]===%d===\n",__func__,__LINE__);

    dumyfd=open(SERVER_FIFO,O_WRONLY,0);  //never used

    printf("===[%s]===%d===\n",__func__,__LINE__);
    while( (n=readline(readfifo,buff,MAXLINE)) >0)
    {
        DEBUG_PRINTF_GRE("[%s]===buff:%s===\n",__func__,buff);
        if(buff[n-1]=='\n')
            n--;   //delete newline from readline

        buff[n]='\0'; //null terminate pathname

        if ((ptr=strchr(buff,' '))==NULL){
            printf("bognus request: %s \n",buff);
            continue;
        }

        *ptr++ =0;  //null terminate PID, ptr=pathname
        pid=atol(buff);
        snprintf(fifoname,sizeof(fifoname),"/tmp/fifo.%ld",(long)pid);
        DEBUG_PRINTF_GRE("fifoname:%s===\n",fifoname);
        if( (writefifo =open(fifoname,O_WRONLY,0))<0)
        {
            printf("cannot open:%s\n",fifoname);
            continue;
        }
        //work-flow
        if((fd=open(ptr,O_RDONLY)) <0) 
        {
            //error:must tell client
            snprintf(buff+n,sizeof(buff)-n,":can't open, %s\n",strerror(errno));
            n=strlen(ptr);
            DEBUG_PRINTF_RED("[%s]===ptr:%s===\n",__func__,ptr);
            write(writefifo,ptr,n);
            close(writefifo);
        }
        else
        {
            //open successeded, copy file to FIFO
            while((n=read(fd,buff,MAXLINE))>0)
            {
                DEBUG_PRINTF_RED("[%s]===buff:%s===\n",__func__,buff);
                write(writefifo,buff,n);
            }
            close(fd);
            close(writefifo);
        }
    }

    exit(0);

}
