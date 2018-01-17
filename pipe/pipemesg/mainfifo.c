#include "mesg.h"

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
    struct mymesg mesg;

    //read pathname
    fgets(mesg.mesg_data,MAXMESGDATA,stdin);

    len=strlen(mesg.mesg_data);
    if (mesg.mesg_data[len-1]=='\n')
        len--; //delete newline from gets();
    mesg.mesg_len=len;
    mesg.mesg_type=1;

    /*write pathname to IPC channel*/
    mesg_send(writefd,&mesg);

    /*read from IPC, write to standard output*/
    while( (n=mesg_recv(readfd, &mesg)) > 0)
        write(STDOUT_FILENO,mesg.mesg_data,n);

}

void server(int readfd, int writefd)
{
    int fd;
    ssize_t n;
    struct mymesg mesg;

    //read pathname from IPC channel
    mesg.mesg_type=1;
    if( (n=mesg_recv(readfd,&mesg)) == 0)
    {
        DEBUG_PRINTF_RED("pathname missing\n");
        exit(-1);
    }

    mesg.mesg_data[n]='\0'; /*null terminate pathname*/

    if((fd=open(mesg.mesg_data,O_RDONLY))<0)
    {
        snprintf(mesg.mesg_data+n,sizeof(mesg.mesg_data)-n,":can't open,%s\n",strerror(errno));
        mesg.mesg_len=strlen(mesg.mesg_data);
        mesg_send(writefd,&mesg);
    }
    else
    {
        //open succeeded: copy file to IPC channel
        while((n=read(fd,mesg.mesg_data,MAXLINE))>0)
        {
            mesg.mesg_len = strlen(mesg.mesg_data);
            mesg_send(writefd, &mesg);
        }

        close(fd);
    }
}
