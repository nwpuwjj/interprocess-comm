#include "unpipc.h"

struct shared{
    sem_t mutex;        /*the mutex: a posix memory-based semaphore */
    int count;
}shared;

int main(int argc, char ** argv)
{
    int fd, i, nloop;
    struct shared *ptr;

    if(argc != 3)
    {
        DEBUG_PRINTF_RED("usage: incr3 <pathname> <#loops> /n");
        return 0;
    }

    nloop=atoi(argv[2]);

    /*open file, initialize to 0, map into memory */
    fd=open(argv[1],O_RDWR|O_CREAT, FILE_MODE);
    write(fd,&shared,sizeof(struct shared));

    ptr=mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    /*initialize semaphore that is shared between processes */
    sem_init(&ptr->mutex,1,1);

    setbuf(stdout,NULL);        /*stdout is unbuffered*/
    if(fork()==0){
        /*child */
        for(i=0;i<nloop;i++)
        {
            sem_wait(&ptr->mutex);
            DEBUG_PRINTF("child: %d\n", ptr->count++);
            sem_post(&ptr->mutex);
        }

        exit(0);
    }

    /*parent*/
    for(i=0;i<nloop;i++)
    {
        sem_wait(&ptr->mutex);
        DEBUG_PRINTF("parent: %d\n",ptr->count++);
        sem_post(&ptr->mutex);
    }

    exit(0);
}
