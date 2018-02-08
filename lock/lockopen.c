#include "unpipc.h"

#define LOCKFILE "/tmp/seqno.lock"

void my_lock(int fd)
{
    int tmpfd;
    while ((tmpfd=open(LOCKFILE,O_RDWR|O_CREAT|O_EXCL,FILE_MODE)) < 0)
    {
        if(errno != EEXIST)
            DEBUG_PRINTF_RED("open error for lock file");
        /*someone else has the lock , loop around and try again*/
    }/*open the file, we have the lock */

    close(tmpfd);
}

void my_unlock(int fd)
{
    unlink(LOCKFILE);  /*release lock by removing file */
}