#include "unpipc.h"
#include "semaphore.h"

int sem_post(sem_t *sem)
{
    int n;

    if(sem->sem_magic != SEM_MAGIC)
    {
        errno =EINVAL;
        return -1;
    }

    if((n=pthread_mutex_lock(&sem->sem_mutex))!=0)
    {
        errno = n;
        return -1;
    }

    if(sem->sem_count == 0)
        pthread_cond_signal(&sem->sem_cond);

    sem->sem_cond ++;
    pthread_mutex_unlock(&sem->sem_mutex);

    return 0;
}
