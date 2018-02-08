#include "unpipc.h"

#define PATH_PIDFILE "pidfile"

int main(int argc, char ** argv)
{
    int pidfd;
    char line[MAXLINE];

    /*open the pid file, create if nonexistent*/
    pidfd = open (PATH_PIDFILE, O_RDWR | O_CREAT, FILE_MODE);

    /*try to write lock the entire file */
    if( write_lock(pidfd, 0, SEEK_SET, 0) < 0)
    {
        if(errno==EACCES || errno== EAGAIN)
        {
            DEBUG_PRINTF_RED("unable to lock %s, is %s already running?", PATH_PIDFILE, argv[0]);
            return 0;
        }
        else
            DEBUG_PRINTF_RED("unable to lock %s",PATH_PIDFILE);
    }

    /*write my PID, leave file open to hold the write lock*/
    snprintf(line,sizeof(line),"%ld\n",(long)getpid());
    ftruncate(pidfd,0);
    write(pidfd,line,strlen(line));

    /* then do whatever the daemons does ...*/
    sleep(30);
}
