#include "unpipc.h"

int main(int argc, char ** argv)
{
    sem_t *sem;
    int val;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: semwait <name>");
        return 0;
    }
    sem = sem_open(argv[1],0);
    sem_wait(sem);

    sem_getvalue(sem,&val);

    DEBUG_PRINTF("pid %ld has semaphore, value = %d\n", (long)getpid(),val);

    pause();   /*blocks until killed */
    exit(0);
}
