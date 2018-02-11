#include "unpipc.h"
#include "semaphore.h"

int sem_close(sem_t *sem)
{
    if(sem->sem_magic != SEM_MAGIC)
    {
        errno =EINVAL;
        return -1;
    }

    if(munmap(sem,sizeof(sem_t))== -1)
        return -1;

    return 0;
}
