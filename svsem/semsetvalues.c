#include "unpipc.h"

int main(int argc, char **argv)
{
    int semid,nsems,i;
    struct semid_ds seminfo;
    unsigned short *ptr;
    union semun arg;

    if(argc<2)
    {
        DEBUG_PRINTF_RED("usage: semsetvalues <pathname> [values ...] \n");
        return 0;
    }

    /*first get the number of semaphores in the set */
    semid=semget(ftok(argv[1],0),0,0);
    DEBUG_PRINTF_GRE("semsetvalues semid:%d===\n",semid);
    arg.buf=&seminfo;
    semctl(semid, 0, IPC_STAT, arg);
    nsems= arg.buf->sem_nsems;
    DEBUG_PRINTF_GRE("semsetvalues  has %d psemaphores==\n",nsems);
    /*now get the values from the command line */
    if(argc !=nsems +2)
    {
        DEBUG_PRINTF_RED("%d psemaphores in set, %d values specified\n",nsems,argc-2);
        return 0;
    }

    /*allocate memory to hold all the values in the set, and store */
    ptr=calloc(nsems, sizeof(unsigned short));
    arg.array = ptr;
    for(i=0;i<nsems;i++)
    {
        ptr[i] = atoi(argv[i+2]);
    }
    semctl(semid,0,SETALL,arg);

    return 0;
}
