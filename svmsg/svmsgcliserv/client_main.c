#include "svmsg.h"

void client(int,int);

int main(int argc, char **argv)
{
    int msqid;

    /*server must create the queue*/
    msqid=msgget(MQ_KEY1,0);

    client(msqid,msqid); /*same queue for both directions*/

    exit(0);
}


void client(int readfd, int writefd)
{
    size_t len;
    ssize_t n;
    char *ptr;
    struct mymesg mesg;

    /*start buffer with pid and a blank*/
    snprintf(mesg.mesg_data, MAXMESGDATA, "%ld ", (long) getpid());

    len=strlen(mesg.mesg_data);
    ptr=mesg.mesg_data+len;

    /*read pathname*/
    fgets(ptr,MAXMESGDATA-len,stdin);
    len=strlen(mesg.mesg_data);
    DEBUG_PRINTF_GRE("mesg: %s==\n",mesg.mesg_data);
    DEBUG_PRINTF_GRE("mesg: len:%d===\n",len);
    
    if(mesg.mesg_data[len-1]=='\n')
    {
        len--;   /*delete newline from fets()*/
    }

    mesg.mesg_len = len;
    mesg.mesg_type=1;

    /*write pid and pathname to IPC channel */
    mesg_send(writefd, &mesg);

    /*read from IPC write to standard output*/
    mesg.mesg_type =getpid();

    while( (n=mesg_recv(readfd,&mesg)) >0 )
        write(STDOUT_FILENO, mesg.mesg_data,n);
}
