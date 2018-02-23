#include "unpipc.h"

int main(int argc, char ** argv)
{
    int id;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: shmrmid <pathname>\n");
        return 0;
    }

    id=shmget(ftok(argv[1],0),0,SVSHM_MODE);
    shmctl(id,IPC_RMID,NULL);

    return 0;
}
