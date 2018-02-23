#include "unpipc.h"

/* struct stored in shared memory*/
struct shmstruct{
    int count;
};

sem_t *mutex;

int main(int argc, char ** argv)
{
    int fd, i, nloop;
    pid_t pid;
    struct shmstruct *ptr;

    if(argc != 4)
    {
        DEBUG_PRINTF_RED("usage: client1 <shmname> <semname> <#loops> \n");
        return 0;
    }

    nloop = atoi(argv[3]);
    fd = shm_open(argv[1],O_RDWR,FILE_MODE);

    ptr= mmap(NULL,sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

    mutex= sem_open(argv[2],0);

    pid = getpid();
    for(i=0;i<nloop;i++)
    {
        sem_wait(mutex);
        DEBUG_PRINTF_GRE("pid %ld： %d \n", (long)pid, ptr->count++);
        sem_post(mutex);
    }

    return 0;
}
