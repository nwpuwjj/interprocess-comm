#include "unpipc.h"

mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;

static void notify_thread(union sigval);  /* our thread function */

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        DEBUG_PRINTF("usage: mqnotifythread1 <name>");
        exit(0);
    }

    mqd=mq_open(argv[1],O_RDONLY | O_NONBLOCK);

    mq_getattr(mqd,&attr);

    sigev.sigev_notify=SIGEV_THREAD;
    sigev.sigev_value.sival_ptr= NULL;
    sigev.sigev_notify_function=notify_thread;
    sigev.sigev_notify_attributes=NULL;
    mq_notify(mqd,&sigev);

    for(;;)
        pause();   /*each new thread does everything */
    exit(0);
}

static void notify_thread(union sigval arg)
{
    ssize_t n;
    void *buff;

    DEBUG_PRINTF("notify_thread started \n");
    buff=malloc(attr.mq_msgsize);

    mq_notify(mqd,&sigev); /*reregister*/

    while((n=mq_receive(mqd,buff,attr.mq_msgsize,NULL))>=0)
    {
        DEBUG_PRINTF("read %ld bytes\n",(long)n);
    }
    if(errno != EAGAIN)
    {
        DEBUG_PRINTF("mq_receive error");
        exit(0);
    }
    free(buff);
    pthread_exit(NULL);
}
