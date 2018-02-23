#include "cliserv2.h"

int main(int argc, char ** argv)
{
    int fd,i,nloop,nusec;
    pid_t pid;
    char mesg[MESGSIZE];
    long offset;
    struct shmstruct *ptr;

    if(argc != 4)
    {
        DEBUG_PRINTF_RED("usage: client2 <name> <#loops> <#usec>\n");
        return 0;
    }

    nloop = atoi(argv[2]);
    nusec = atoi(argv[3]);

    /*open and map shared memory that server must create */
    fd = shm_open(argv[1],O_RDWR, FILE_MODE);
    ptr=mmap(NULL,sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    pid=getpid();

    for(i=0;i<nloop;i++)
    {
        sleep(nusec);
        snprintf(mesg,MESGSIZE,"pid %ld: message %d \n",(long)pid,i);

        if(sem_trywait(&ptr->nempty) == -1)
        {
            if(errno == EAGAIN)
            {
                sem_wait(&ptr->noverflowmutex);
                ptr->noverflow++;
                sem_post(&ptr->noverflowmutex);
                continue;
            }
            else
            {
                DEBUG_PRINTF_RED("sem_trywait error \n");
                return 0;
            }
        }

        sem_wait(&ptr->mutex);
        offset=ptr->msgoff[ptr->nput];

        if(++(ptr->nput)>=NMESG)
            ptr->nput=0;    /*circular buffer */

        sem_post(&ptr->mutex);
        strcpy(&ptr->msgdata[offset],mesg);
        sem_post(&ptr->nstored);
    }

    return 0;
}
