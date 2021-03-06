#include "unpipc.h"

int main(int argc, char ** argv)
{
    int i, id;
    struct shmid_ds buff;
    unsigned char *ptr;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: shmwrite <pathname>\n");
        return 0;
    }

    id = shmget(ftok(argv[1],0),0, SVMSG_MODE);
    ptr = shmat(id,NULL,0);
    shmctl(id,IPC_STAT, &buff);

    /* set: ptr[0] =0, ptr[1]=1, etc. */
    for(i=0;i<buff.shm_segsz;i++)
        *ptr++ = i %256;

    return 0;
}

