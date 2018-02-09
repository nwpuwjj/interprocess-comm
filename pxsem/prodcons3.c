#include "unpipc.h"

#define NBUFF 10 
#define MAXNTHREADS 100

int nitems, nproduces; /*read-only by producer and consumer*/

struct{
    int buff[NBUFF];
    int nput;
    int nputval;
    sem_t mutex,nempty, nstored;  /*semaphores, not pointers*/
}shared;

void *produce(void *), *consume(void *);

int main(int argc, char **argv)
{
    int i, count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS],tid_consume;

    if(argc !=3 )
    {
        DEBUG_PRINTF_GRE("usage: prodcons3 <#items> <#producers>");
        return 0;
    }

    nitems = atoi(argv[1]);
    nproduces = atoi(argv[2]);

    /*initialize three semaphores */
    sem_init(&shared.mutex, 0, 1);
    sem_init(&shared.nempty,0,NBUFF);
    sem_init(&shared.nstored,0,0);

    /*create all producers and one consumer */
    pthread_setconcurrency(nproduces+1);

    for(i=0;i<nproduces;i++)
    {
        count[i]=0;
        pthread_create(&tid_produce[i],NULL,produce,&count[i]);
    }
    pthread_create(&tid_consume,NULL,consume,NULL);

    /*wait for all producers and the consumer */
    for(i=0; i<nproduces; i++)
    {
        pthread_join(tid_produce[i],NULL);
        DEBUG_PRINTF("count[%d]=%d\n",i,count[i]);
    }

    pthread_join(tid_consume,NULL);

    sem_destroy(&shared.mutex);
    sem_destroy(&shared.nempty);
    sem_destroy(&shared.nstored);
    return 0;
}

void *produce(void *arg)
{
    for(;;)
    {
        sem_wait(&shared.nempty);  /*wait for at least 1 empty slot */
        sem_wait(&shared.mutex);

        if(shared.nput >= nitems)
        {
            sem_post(&shared.nempty);
            sem_post(&shared.mutex);
            return NULL;           /* all done */
        }

        shared.buff[shared.nput % NBUFF] = shared.nputval;
        shared.nput++;
        shared.nputval++;

        sem_post(&shared.mutex);
        sem_post(&shared.nstored);  /*1 more stored item*/
        *((int *)arg) += 1;
    }
}

void *consume(void *arg)
{
    int i;

    for(i=0;i<nitems; i++)
    {
        sem_wait(&shared.nstored);   /*wait for at least 1 stored item */
        sem_wait(&shared.mutex); 

        DEBUG_PRINTF_GRE("buff[%d]= %d \n",i, shared.buff[i%NBUFF]);

        sem_post(&shared.mutex);
        sem_post(&shared.nempty);  /*1 more empty slot */
    }

    return NULL;
}