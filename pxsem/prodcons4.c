#include "unpipc.h"

#define NBUFF 10
#define MAXNTHREADS 100

int nitems,nproducers,nconsumers;  /*read-only*/

struct {                         /*data shared by producers and consumers*/
    int buff[NBUFF];
    int nput;                /*item number:0,1,2,... */
    int nputval;             /*value to store in buff[] */
    int nget;                /*item number: 0,1,2,...*/
    int ngetval;             /*value fetched from buff[] */
    sem_t mutex, nempty, nstored;    /*semaphores, not pointers */
}shared;

void *produce(void *), *consume(void *);

int main(int argc, char **argv)
{
    int i, prodcount[MAXNTHREADS], conscount[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume[MAXNTHREADS];

    if(argc != 4)
    {
        DEBUG_PRINTF_RED("usage: prodcons4 <#items> <#producers> <#consumers> \n");
        return 0;
    }

    nitems = atoi(argv[1]);
    nproducers = atoi(argv[2]);
    nconsumers= atoi(argv[3]);

    /*initialize three semaphores */
    sem_init(&shared.mutex, 0, 1);
    sem_init(&shared.nempty,0, NBUFF);
    sem_init(&shared.nstored,0,0);

    /*create all producers and all consumers */
    pthread_setconcurrency(nproducers+nconsumers);

    for(i=0; i<nproducers;i++)
    {
        prodcount[i]=0;
        pthread_create(&tid_produce[i],NULL,produce,&prodcount[i]);
    }

    for(i=0;i<nconsumers;i++)
    {
        conscount[i]=0;
        pthread_create(&tid_consume[i],NULL,consume,&conscount[i]);
    }

    /*wait for all producers and all consumers */
    for(i=0;i<nproducers;i++)
    {
        pthread_join(tid_produce[i],NULL);
        DEBUG_PRINTF("producer count[%d] = %d\n",i,prodcount[i]);
    }

    for(i=0;i<nconsumers;i++)
    {
        pthread_join(tid_consume[i],NULL);
        DEBUG_PRINTF("consumer count[%d] = %d\n",i,conscount[i]);
    }

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
            sem_post(&shared.nstored);    /*let consumers terminate*/
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

void *consume(void * arg)
{
    int i;

    for(;;)
    {
        sem_wait(&shared.nstored); /*wait for at least 1 stored item */
        sem_wait(&shared.mutex);

        if(shared.nget>= nitems)
        {
            sem_post(&shared.nstored);
            sem_post(&shared.mutex);
            return NULL;  /*all done*/
        }

        i = shared.nget % NBUFF;
        
        DEBUG_PRINTF("buff[%d]=%d\n",i,shared.buff[i]);

        shared.nget++;
        shared.ngetval++;

        sem_post(&shared.mutex);
        sem_post(&shared.nempty);         /*1 more empty slot */
       *((int *)arg) +=1;
    }
}