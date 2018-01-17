#include "unpipc.h"

int main(int argc, char ** argv)
{
    mqd_t mqd;
    void *ptr;
    size_t len;
    unsigned int prio;

    if(argc !=4 )
    {
        DEBUG_PRINTF_RED("usage: mqsend <name> <#bytes> <priority>");
        exit(0);
    }

    len =atoi(argv[2]);

    prio=atoi(argv[3]);

    mqd=mq_open(argv[1],O_WRONLY);

    ptr=calloc(len,sizeof(char));
    mq_send(mqd,ptr,len,prio);
    exit(0);
}
