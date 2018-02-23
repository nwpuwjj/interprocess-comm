#include "unpipc.h"

int main(int argc, char ** argv)
{
    int i,fd;
    struct stat stat;
    unsigned char c, *ptr;

    if(argc !=2)
    {
        DEBUG_PRINTF_RED("usage: shmread <name> /n");
        return 0;
    }

    /*open, get size, map*/
    fd = shm_open(argv[1],O_RDONLY, FILE_MODE);
    fstat(fd,&stat);
    ptr=mmap(NULL,stat.st_size,PROT_READ,MAP_SHARED,fd,0);

    close(fd);

    /*check that ptr[0]=0,ptr[1]=1,etc.*/
    for(i=0;i<stat.st_size;i++)
    {
        DEBUG_PRINTF_GRE("ptr[%d]= %d \n",i,*ptr++);
    }

    return 0;
}
