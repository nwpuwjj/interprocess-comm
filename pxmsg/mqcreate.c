#include "unpipc.h"

struct mq_attr attr; /*max_maxmsg and mq_msgsize both init to 0 */


int main(int argc, char **argv)
{
    int c,flags;
    mqd_t mqd;
    memset(&attr,0,sizeof(struct mq_attr));
    flags=O_RDWR|O_CREAT;

    while((c=getopt(argc,argv,"em:z:"))!=-1)
    {
        switch(c)
        {
            case 'e':
                flags |= O_EXCL;
                break;
            
            case 'm':
                attr.mq_maxmsg = atol(optarg);
                break;

            case 'z':
                attr.mq_msgsize = atol(optarg);
                break;
        }

    }

    if(optind != argc-1)
    {
        DEBUG_PRINTF_RED("usage: mqcreate [-e ] [-m maxmsg -z msgsize ] <name>");
        exit(0);
    }

    if ((attr.mq_maxmsg!=0 && attr.mq_msgsize == 0) ||
            (attr.mq_maxmsg==0 && attr.mq_msgsize!=0))
    {
        DEBUG_PRINTF_RED("must specify both -m maxmsg and -z msgsize");

        exit(0);
    }

    mqd=mq_open(argv[optind], flags, FILE_MODE, &attr);
    
    if(mqd == -1)
    {
        DEBUG_PRINTF_RED("mq_open; errstr:%s===\n",strerror(errno));
    }
    mq_close(mqd);
    exit(0);
}
