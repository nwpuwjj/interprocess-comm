#include "unpipc.h"

int main(int argc, char ** argv)
{
    int signo;
    mqd_t mqd;
    void *buff;
    ssize_t n;
    sigset_t  newmask;
    
    struct mq_attr attr;
    struct sigevent sigev;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: mqnotifysig4 <name>");
        exit(0);
    }

    /* open queue, get attributes, allocate read buffer */
    mqd=mq_open(argv[1],O_RDONLY | O_NONBLOCK);
    mq_getattr(mqd,&attr);
    buff=malloc(attr.mq_msgsize);

    sigemptyset(&newmask);  
    sigaddset(&newmask,SIGUSR1);
    sigprocmask(SIG_BLOCK, &newmask, NULL); /*block SIGUSR1 */

    /*establish signal handler, enable notification*/
    sigev.sigev_notify=SIGEV_SIGNAL;
    sigev.sigev_signo=SIGUSR1;
    mq_notify(mqd,&sigev);

    for(;;)
    {
        sigwait(&newmask,&signo);
        if(signo == SIGUSR1)
        {
            mq_notify(mqd,&sigev); /*reregister first */
            while( (n=mq_receive(mqd,buff,attr.mq_msgsize,NULL) ) >=0)
            {
                printf("read %ld bytes \n",(long)n);
            }
            if(errno != EAGAIN)
            {
                DEBUG_PRINTF("mq_receive error");
            }
        }
    }

    exit(0);
}

