#include "unpipc.h"


int main(int argc, char ** argv)
{
    int c,flag,mqid;
    long type;
    ssize_t n;
    key_t key;
    struct my_msgbuf *buff;

    type=flag=0;
    while( (c=getopt(argc,argv,"nt:")) != -1 )
    {
        switch(c)
        {
            case 'n':
                flag |= IPC_NOWAIT;
                break;

            case 't':
                type = atol(optarg);
                break;
        }
    }

    if( optind != argc -1)
    {
        DEBUG_PRINTF_RED("usage: msgrcv [-n] [-t type] <pathname>");
        return -1;
    }
    key = ftok(argv[optind],0);
    if(key == -1)
    {
        DEBUG_PRINTF_RED("ftok error \n");
        return -1;
    }
    mqid=msgget(key,MSG_R);
    buff=malloc(MAXMSG);

    n=msgrcv(mqid,buff,MAXMSG,type,flag);
    DEBUG_PRINTF_GRE("read %d bytes, type= %ld\n", n, buff->mtype);
    DEBUG_PRINTF_GRE(" msgtest:%s ----\n", buff->mtext);    
    return 0;
}
