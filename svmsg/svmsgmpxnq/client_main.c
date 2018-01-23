#include "unpipc.h"
#include "svmsg.h"
#include "mesg.h"


void client(int,int);

int main(int argc, char **argv)
{
    int readid, writeid;

    /*server must create its well-known queue */
    writeid =msgget(MQ_KEY1,0);

    /*we create our own private queue */
    readid =msgget(IPC_PRIVATE, SVMSG_MODE | IPC_CREAT);

    client(readid,writeid);

    /*and delete our private queue */
    msgctl(readid,IPC_RMID,NULL);

    exit(0);
}

void client(int readid, int writeid)
{
    size_t len;
    ssize_t n;
    char *ptr;
    struct mymesg mesg;

    /*start buff with msqid and a blank */
    snprintf(mesg.mesg_data,MAXMESGDATA, "%d ",readid);

    len=strlen(mesg.mesg_data);
    ptr=mesg.mesg_data+len;

    /*read pathname*/
    fgets(ptr, MAXMESGDATA-len, stdin);
    len=strlen(mesg.mesg_data);

    if(mesg.mesg_data[len-1]=='\n')
    {
        len--;               /*delete newline from fgets() */
    }

    mesg.mesg_len=len;
    mesg.mesg_type=1;

    DEBUG_PRINTF_GRE("out: mesg_len:%d  \n",mesg.mesg_len);
    DEBUG_PRINTF_GRE("mesg_type:%d   \n",mesg.mesg_type);
    DEBUG_PRINTF_GRE("mesg_data:%s   \n",mesg.mesg_data);

    /*write msqid and pathname to server's well-known queue */
    mesg_send(writeid,&mesg);

    /*read from our queue, write to standard output */
    while((n=mesg_recv(readid,&mesg))>0)
        write(STDOUT_FILENO, mesg.mesg_data,n);
}
