#include "unpipc.h"
#include "semaphore.h"

int sem_getvalue(sem_t *sem, int * pvalue)
{
    int val;

    if(sem->sem_magic != SEM_MAGIC)
    {
        errno = EINVAL;
        return -1;
    }

    if( (val=semctl(sem->sem_semid,0,GETVAL))<0)
        return -1;

    *pvalue = val;
    return 0;
}