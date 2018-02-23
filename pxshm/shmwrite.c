#include "unpipc.h"

int main(int argc, char ** argv)
{
    int i, fd;
    struct stat stat;
    unsigned char *ptr;

    if( argc != 2)
    {
        DEBUG_PRINTF_RED("usage: shmwrite <name> \n");
        return 0;
    }

    /*open get size, map*/
    fd = shm_open(argv[1],O_RDWR,FILE_MODE);
    fstat(fd,&stat);
    DEBUG_PRINTF("length: %d===\n",stat.st_size);
    ptr=mmap(NULL,stat.st_size,PROT_READ| PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

    /* set: ptr[0]=0, ptr[1]=1, etc. */
    for(i=0;i<stat.st_size;i++)
        *ptr++ = i%256;

    return 0;
}
