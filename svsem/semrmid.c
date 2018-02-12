#include "unpipc.h"

int main(int argc, char ** argv)
{
    int semid;

    if(argc!=2)
    {
        DEBUG_PRINTF_RED("usage: semrmid <pathname> /n");
        return 0;
    }

    semid = semget(ftok(argv[1],0),0,0);
    DEBUG_PRINTF_GRE("semrmid semid:%d \n",semid);
    semctl(semid,0,IPC_RMID);
    return 0;
}
