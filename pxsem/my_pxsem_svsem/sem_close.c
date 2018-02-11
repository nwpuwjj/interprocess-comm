#include "unpipc.h"
#include "semaphore.h"

int sem_close(sem_t * sem)
{
    if(sem->sem_magic != SEM_MAGIC)
    {
        errno = EINVAL;
        return -1;
    }
    sem->sem_magic =0;  /*just in case */

    free(sem);
    return 0;
}