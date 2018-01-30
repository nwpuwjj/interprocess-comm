#include "unpipc.h"

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems;   /*read-only by producer and consumer*/

struct{
    pthread_mutex_t mutex;
    int buff[MAXNITEMS];
    int nput;  //buff 数组中下一次存放的元素小标
    int nval;  //下一次存放的值
} shared ={
    PTHREAD_MUTEX_INITIALIZER
};

void *produce(void *);
void *consume(void *);

int main(int argc, char **argv)
{
    int i, nthreads, count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;

    if(argc !=3 )
    {
        DEBUG_PRINTF_RED("usage: prodcons2 <#items> <#threads>");
        return 0;
    }
    nitems = MIN(atoi(argv[1]), MAXNITEMS);
    nthreads = MIN(atoi(argv[2]), MAXNTHREADS);

    pthread_setconcurrency(nthreads);

    /*start all the producer threads*/
    for(i=0;i<nthreads;i++)
    {
        count[i]=0;
        pthread_create(&tid_produce[i],NULL,produce,&count[i]);
    }

    /*wait for all the producer threads */
    for(i=0;i<nthreads;i++)
    {
        pthread_join(tid_produce[i],NULL);
        printf("count[%d]=%d\n",i,count[i]);
    }

    /*start, then wait for the consumer thread */
    pthread_create(&tid_consume,NULL,consume,NULL);
    pthread_join(tid_consume,NULL);

    exit(0);

}
    
void *produce (void *arg)
{
    for (;;){
        pthread_mutex_lock(&shared.mutex);
        if(shared.nput >= nitems){
            pthread_mutex_unlock(&shared.mutex);
            return(NULL); /*array is full, we're done */
        }
        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock(&shared.mutex);

        *((int*) arg) += 1;
    }
}

void *consume(void *arg)
{
    int i;
    for(i=0; i<nitems;i++)
    {
        if(shared.buff[i] != i)
            printf("buff[%d] = %d \n",i,shared.buff[i]);
    }

    return NULL;
}

//生产者在计数，消费者负责确认数
