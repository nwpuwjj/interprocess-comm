#include "unpipc.h"

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

/* globals shared by threads */
int nitems;   /*read-only by producer and consumer*/
int buff[MAXNITEMS];

struct{
    pthread_mutex_t mutex;
    int nput;  //buff 数组中下一次存放的元素小标
    int nval;  //下一次存放的值
} put ={
    PTHREAD_MUTEX_INITIALIZER
};

struct{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;   /*number ready for consumer */
} nready ={
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
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

    pthread_setconcurrency(nthreads+1);

    /*start all the producer threads*/
    for(i=0;i<nthreads;i++)
    {
        count[i]=0;
        pthread_create(&tid_produce[i],NULL,produce,&count[i]);
    }
    pthread_create(&tid_consume,NULL,consume,NULL);

    /*wait for all the producer threads and the consumer*/
    for(i=0;i<nthreads;i++)
    {
        pthread_join(tid_produce[i],NULL);
        printf("count[%d]=%d\n",i,count[i]);
    }

   
    pthread_join(tid_consume,NULL);

    exit(0);

}
    
void *produce (void *arg)
{
    for (;;){
        pthread_mutex_lock(&put.mutex);
        if(put.nput >= nitems){
            pthread_mutex_unlock(&put.mutex);
            return(NULL); /*array is full, we're done */
        }
        buff[put.nput] = put.nval;
        put.nput++;
        put.nval++;
        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&nready.mutex);
        if(nready.nready == 0)
            pthread_cond_signal(&nready.cond);
        nready.nready++;
        pthread_mutex_unlock(&nready.mutex);

        *((int *)arg) += 1;
    }
}


void *consume(void *arg)
{
    int i;
    for(i=0; i<nitems;i++)
    {
        pthread_mutex_lock(&nready.mutex);
        while(nready.nready==0)
            pthread_cond_wait(&nready.cond,&nready.mutex);
        nready.nready--;
        pthread_mutex_unlock(&nready.mutex);

        if(buff[i] != i)
        {
            printf("buff[%d]=%d \n",i,buff[i]);
        }
    }

    return NULL;
}

//生产者在计数，消费者负责确认数

