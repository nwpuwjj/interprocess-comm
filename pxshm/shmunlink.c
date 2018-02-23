#include "unpipc.h"

int main(int argc, char **argv)
{
    if(argc !=2)
    {
        DEBUG_PRINTF_RED("usage:shmunlink <name>\n");
        return 0;
    }

    shm_unlink(argv[1]);

    return 0;
}
