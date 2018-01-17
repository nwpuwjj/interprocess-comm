#include "unpipc.h"

int main(int argc, char **argv)
{
    mqd_t mqd;

    struct mq_attr attr;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: mqgetattr <name>");
        exit(0);
    }

    mqd=mq_open(argv[1],O_RDONLY);

    mq_getattr(mqd,&attr);

    DEBUG_PRINTF("max #msgs = %ld, max #bytes/msg= %ld, "
                 "#currently on queue = %ld\n",
                 attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    mq_close(mqd);

    exit(0);
}
