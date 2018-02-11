#include "unpipc.h"
#include "semaphore.h"

int sem_unlink(const char *pathname)
{
    if(unlink(pathname) == -1)
        return -1;

    return 0;
}
