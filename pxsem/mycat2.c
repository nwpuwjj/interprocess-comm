#include "unpipc.h"

#define NBUFF 8
#define BUFFSIZE 256

struct{                   /*data shared by producer and consumer*/
    struct {
        char data[BUFFSIZE];  /*a buffer */
        ssize_t n;            /* count of #bytes in the buffer */
    } buff[NBUFF];           /* NBUFF of these buffers/counts */

    sem_t mutex,nempty,nstored;    /* semaphores, not pointers */
}shared;

int fd;                   /* input file to copy to stdout */
void *produce(void *), *consume(void *);

int main(int argc, char **argv)
{
    pthread_t tid_produce, tid_consume;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: mycat2 <pathname> \n");
        return 0;
    }

    fd = open(argv[1],O_RDONLY);

    /*initialize three semaphores*/
    sem_init(&shared.mutex,0,1);
    sem_init(&shared.nempty,0,NBUFF);
    sem_init(&shared.nstored,0,0);

    /* one producer thread, one consumer thread */
    pthread_setconcurrency(2);
    pthread_create(&tid_produce,NULL,produce,NULL); /*read thread*/
    pthread_create(&tid_consume,NULL,consume,NULL); /*write thread*/

    pthread_join(tid_produce,NULL);
    pthread_join(tid_consume,NULL);

    sem_destroy(&shared.mutex);
    sem_destroy(&shared.nempty);
    sem_destroy(&shared.nstored);
    close(fd);

    return 0;
}

void * produce(void *arg)
{
    int i;
    for(i=0; ;)
    {
        sem_wait(&shared.nempty); /*wait for at least 1 empty slot */
        sem_wait(&shared.mutex);

        /*critical region*/
        sem_post(&shared.mutex);

        shared.buff[i].n = read(fd,shared.buff[i].data,BUFFSIZE);
        DEBUG_PRINTF_RED("produce read: %s ======\n",shared.buff[i].data);
        if(shared.buff[i].n == 0){
            sem_post(&shared.nstored); /*1 more stored item*/
            return NULL;
        }

        if(++i >= NBUFF)
            i=0;    /*circular buffer */

        sem_post(&shared.nstored); /*1 more stored item*/
    }

                

    
}


void *consume(void *arg)
{
    int i;
    int fd2= open("wjj",O_WRONLY);

    for(i=0; ;)
    {
        sem_wait(&shared.nstored);             /*wait for at least 1 stored item */

        sem_wait(&shared.mutex);

        /*cirtical region*/
        sem_post(&shared.mutex);

        if(shared.buff[i].n==0)
        {
            close(fd2);
            return NULL;
        }

        write(fd2, shared.buff[i].data, shared.buff[i].n);
        //DEBUG_PRINTF_GRE("consume write: %s =====\n",shared.buff[i].data);
        //fflush();

        if(++i >= NBUFF)
            i=0;        /* circular buffer */

        sem_post(&shared.nempty); /*1 more empty slot */
    }
}
