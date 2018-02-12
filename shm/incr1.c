//默认情况下通过fork派生的子进程并不与父进程共享内存区

#include "unpipc.h"

#define SEM_NAME "mysem"

int count = 0;

int main(int argc, char **argv)
{
    int i, nloop;
    sem_t *mutex;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: incr1 <#loops>");
        return 0;
    }

    nloop = atoi(argv[1]);

    /*create, initialize, and unlink semaphore */
    mutex = sem_open(SEM_NAME,O_CREAT|O_EXCL, FILE_MODE,1);
    sem_unlink(SEM_NAME);

    setbuf(stdout,NULL); /*stdout is unbuffered*/
    if(fork()==0)
    {
        //child
        for(i=0;i<nloop;i++)
        {
            sem_wait(mutex);
            DEBUG_PRINTF("child: %d\n",count++);
            sem_post(mutex);
        }

        exit(0);
    }

    /*parent */
    for(i=0;i<nloop;i++)
    {
        sem_wait(mutex);
        DEBUG_PRINTF("parent: %d\n",count++);
        sem_post(mutex);
    }

    exit(0);
    
}
