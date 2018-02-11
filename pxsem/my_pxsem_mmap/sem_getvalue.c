#include "unpipc.h"
#include "semaphore.h"

int sem_getvalue(sem_t *sem, int *pvalue)
{
    int n;

    if(sem->sem_magic != SEM_MAGIC)
    {
        errno = EINVAL;
        return -1;
    }
    if ((n=pthread_mutex_lock(&sem->sem_mutex))!=0)
    {
        errno =n;
        return -1;
    }

    *pvalue = sem->sem_count;
    pthread_mutex_unlock(&sem->sem_mutex);
    return 0;
}
