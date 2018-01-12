#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 256
//pipe[0]===read
//pipe[1]===write
void client(int,int),server(int,int);

int main(int argc, char **argv)
{
    int pipe1[2],pipe2[2];
    
    pid_t childpid;

    //create two pipes
    pipe(pipe1); 
    pipe(pipe2);

    //child
    if((childpid=fork())==0)
    {
        close(pipe1[1]);
        close(pipe2[0]);
        server(pipe1[0],pipe2[1]);
        exit(0);
    } 

    //parent
    close(pipe1[0]);
    close(pipe2[1]);

    client(pipe2[0],pipe1[1]);

    waitpid(childpid,NULL,0);  //wait for child to terminate

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
        printf("[%s]==end-of-file while reading pathname");
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
