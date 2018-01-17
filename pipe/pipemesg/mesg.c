#include "mesg.h"

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
ssize_t mesg_send(int fd, struct mymesg *mptr)
{
    return(write(fd, mptr, MESGHDRSIZE + mptr->mesg_len));
}

ssize_t mesg_recv(int fd, struct mymesg *mptr)
{
    size_t len;
    ssize_t n;

    /* read message header first, to get len of data that follows */
    if ( (n = read(fd,mptr,MESGHDRSIZE)) == 0 )
        return 0;  /* end of file */
    else if (n != MESGHDRSIZE)
    {
        DEBUG_PRINTF_RED("message header: expected %d, got %d", MESGHDRSIZE, n);
        exit(-1);
    }

    if ( (len=mptr->mesg_len) > 0)
    {
        if( (n=read(fd,mptr->mesg_data,len))!=len)
        {
            DEBUG_PRINTF_RED("message data: expected %d, got %d", len, n);
            exit(-1);
        }
    }
    return len;
}
