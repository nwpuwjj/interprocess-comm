#include "unpipc.h"

int main(int argc, char **argv)
{
    int mqid;
    key_t key;
    if (argc !=2 )
    {
        DEBUG_PRINTF_RED("usage: msgrmid <pathname>");
        return -1;
    }
    key=ftok(argv[1],0);
    if(key==-1)
    {
        DEBUG_PRINTF_RED("ftok fail===\n");
        return -1;
    }
    mqid=msgget(key,0);

    msgctl(mqid,IPC_RMID,NULL);
    return 0;
}
