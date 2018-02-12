#include "unpipc.h"

int main(int argc, char **argv)
{
    int semid, nsems, i;
    struct semid_ds seminfo;
    unsigned short *ptr;
    union semun arg;

    if(argc!=2)
    {
        DEBUG_PRINTF_RED("usage: semgetvalues <pathname> \n");
        return 0;
    }

    /* first get the number of semaphores in the set */
    semid =semget(ftok(argv[1],0),0,0);
    DEBUG_PRINTF_RED("semgetvalues semid: %d \n",semid);
    arg.buf = &seminfo;
    semctl(semid,0,IPC_STAT,arg);
    nsems = arg.buf->sem_nsems;

    /*allocate memory to hold all the values in the set */
    ptr = calloc(nsems, sizeof(unsigned short));
    arg.array = ptr;

    /* fetch the values and print */
    semctl(semid, 0, GETALL, arg);

    for(i=0;i<nsems; i++)
        DEBUG_PRINTF_GRE("semval[%d] = %d\n",i,ptr[i]);


    return 0;
}
