#include "unpipc.h"

int main(int argc, char **argv)
{
    int c,flags;
    mqd_t mqd;

    flags = O_RDWR | O_CREAT;

    while( (c=getopt(argc,argv,"e")) !=-1)
    {
        switch(c){
            case 'e':
                flags |= O_EXCL;
                break;
        }
    }

    if(optind !=argc -1 )
    {
        DEBUG_PRINTF("usage: mqcreate [-e ] <name>");
        exit(0);
    }
    
    mqd = mq_open(argv[optind],flags, FILE_MODE, NULL);
    
    if(mqd == -1)
    {
        DEBUG_PRINTF_RED("mq_open");
        DEBUG_PRINTF_RED("errstring:%s==\n",strerror(errno));
        return 2;
    }

    mq_close(mqd);
    exit(0);
}
