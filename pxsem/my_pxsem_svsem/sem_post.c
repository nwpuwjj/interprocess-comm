#include "unpipc.h"
#include "semaphore.h"

int sem_post(sem_t *sem)
{
    struct sembuf op;

    if(sem->sem_magic != SEM_MAGIC)
    {
        errno = EINVAL;
        return -1;
    }

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg=0;
    if(semop(sem->sem_semid,&op,1)<0)
        return -1;

    return 0;
}
