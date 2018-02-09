#include "unpipc.h"

int main(int argc, char ** argv)
{
    sem_t *sem;
    int val;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: semgetvalue <name>");
        return 0;
    }

    sem=sem_open(argv[1],0);
    sem_getvalue(sem,&val);
    DEBUG_PRINTF("value=%d\n",val);

    exit(0);
}
