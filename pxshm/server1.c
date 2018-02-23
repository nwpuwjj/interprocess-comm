//创建并初始化共享内存区和信号量的程序
#include "unpipc.h"

/* struct stored in shared memory*/
struct shmstruct{
    int count;
};

sem_t *mutex;  /*pointer to named semaphore*/

int main(int argc, char ** argv)
{
    int fd;
    struct shmstruct *ptr;

    if( argc !=3)
    {
        DEBUG_PRINTF_RED("usage: server1 <name> <semname> \n");
        return 0;
    }

    shm_unlink(argv[1]);    /*OK if this fails*/
    /*create shm, set its size, map it, close descriptor */
    fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
    ftruncate(fd,sizeof(struct shmstruct));

    ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sem_unlink(argv[2]);    /* OK if this fail*/
    mutex=sem_open(argv[2], O_CREAT | O_EXCL, FILE_MODE,1);
    sem_close(mutex);

    return 0;
}
