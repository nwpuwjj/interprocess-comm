#include "unpipc.h"

#define SEM_NAME "mysem"

int main(int argc, char **argv)
{
    int fd,i,nloop,zero=0;
    int *ptr;
    sem_t *mutex;

    if(argc != 3)
    {
        DEBUG_PRINTF_RED("usage: incr2 <pathname> <#loops> \n");
        return 0;
    }

    nloop= atoi(argv[2]);

    /*open file, initialize to 0, map into memory */
    fd=open(argv[1],O_RDWR|O_CREAT, FILE_MODE);
    write(fd,&zero,sizeof(int));
    ptr=mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    /*create, initialize, and unlink semaphore */
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
    sem_unlink(SEM_NAME);

    setbuf(stdout, NULL); /*stdout is unbuffered*/

    if(fork()==0)
    {
        //child
        for(i=0;i<nloop;i++)
        {
            sem_wait(mutex);
            DEBUG_PRINTF("child: %d\n",(*ptr)++);
            sem_post(mutex);
        }

        exit(0);
    }

    /*parent*/
    for(i=0;i<nloop;i++)
    {
        sem_wait(mutex);
        DEBUG_PRINTF("parent: %d\n",(*ptr)++);
        sem_post(mutex);
    }

    exit(0);
}
