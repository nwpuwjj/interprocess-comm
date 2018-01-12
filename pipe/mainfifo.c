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

void client(int,int),server(int,int);

int main(int argc, char **argv)
{
    int readfd,writefd;
    
    pid_t childpid;
    
    //create two FIFOs; ok if they already exist 
    if((mkfifo(FIFO1,FILE_MODE) <0) && (errno != EEXIST))
    {
        printf("[%s]===can't create %s===\n",FIFO1);
        return 2;
    }
    if((mkfifo(FIFO2,FILE_MODE)<0) &&(errno !=EEXIST))
    {
        unlink(FIFO1);
        printf("[%s]===can't create %s===\n",FIFO2);
        return 2;
    }
    
    //child
    if((childpid=fork())==0)
    {
        readfd=open(FIFO1,O_RDONLY,0);
        writefd=open(FIFO2,O_WRONLY,0);
        
        server(readfd,writefd);
        exit(0);
    }

    //parent
    writefd=open(FIFO1,O_WRONLY,0);
    readfd=open(FIFO2,O_RDONLY,0);

    client(readfd,writefd);

    waitpid(childpid,NULL,0);  //wait for child to terminate

    close(readfd);
    close(writefd);

    unlink(FIFO1);
    unlink(FIFO2);

    exit(0);
}

void client(int readfd, int writefd)
{
    size_t len;
    ssize_t n;
    char buff[MAXLINE];

    //read pathname 
    fgets(buff,MAXLINE,stdin);
    printf("[%s]===buff:%s===\n",__func__,buff);
    len=strlen(buff);
    //delete \n
    if(buff[len-1]=='\n')
        len--;  
    
   //write pathname to IPC channel
   write(writefd,buff,len);

   //read from IPC, write to standard output 
   while((n=read(readfd,buff,MAXLINE))>0)
   {
       printf("[%s]===buff:%s==\n",__func__,buff);
       memset(buff,0,MAXLINE);
       write(STDOUT_FILENO,buff,n);
   }

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
