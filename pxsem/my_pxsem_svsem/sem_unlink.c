#include "unpipc.h"
#include "semaphore.h"

int sem_unlink(const char * pathname)
{
    int semid;
    key_t key;

    if( (key=ftok(pathname,1))==(key_t)-1)
        return -1;

    if(unlink(pathname)== -1)
        return -1;

    if( (semid=semget(key,1,SVSEM_MODE))== -1)
        return -1;

    if(semctl(semid,0,IPC_RMID)== -1)
        return -1;

    return 0;
}
