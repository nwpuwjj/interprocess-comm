#include "unpipc.h"
#include "semaphore.h"

#include <stdarg.h>                 /*for variable arg lists */
#define MAX_TRIES 10                /*for waiting for initialization*/


sem_t *sem_open(const char *pathname, int oflag,...)
{
    int fd,i,created,save_errno;
    mode_t mode;
    va_list ap;
    sem_t *sem,seminit;
    struct stat statbuff;
    unsigned int value;
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;

    created=0;
    sem=MAP_FAILED;              /*[sic]*/

again:
     if(oflag&O_CREAT){
        va_start(ap,oflag);  /*init ap to final named argument*/
        mode = va_arg(ap,va_mode_t) & ~S_IXUSR;
        value = va_arg(ap,unsigned int);
        va_end(ap);

        /*open and specify O_EXCL and user-execute */
        fd=open(pathname,oflag | O_EXCL | O_RDWR, mode | S_IXUSR);

        if(fd<0)
        {
            if(errno==EEXIST && (oflag&O_EXCL)==0)
                goto exists;       /*already exists, ok */
            else
                return SEM_FAILED;
        }

        created = 1;

        /*first one to create the file initializes it*/
        /*set the file size*/
        bzero(&seminit, sizeof(seminit));
        if(write(fd,&seminit,sizeof(seminit))!=sizeof(seminit))
            goto err;

        /*memory map the file */
        sem=mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if(sem==MAP_FAILED)
            goto err;

        /* initialize mutex, condition variable, and value */
        if( (i=pthread_mutexattr_init(&mattr)) !=0)
            goto pthreaderr;

        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        i=pthread_mutex_init(&sem->sem_mutex, &mattr);
        pthread_mutexattr_destroy(&mattr); /*be sure to destroy*/
        if(i!=0)
            goto pthreaderr;

        if( (i=pthread_condattr_init(&cattr))!=0)
            goto pthreaderr;

        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        i=pthread_cond_init(&sem->sem_cond,&cattr);

        pthread_condattr_destroy(&cattr); /*be sure to destroy*/

        if(i!=0)
            goto pthreaderr;

        if( (sem->sem_count = value) > sysconf(_SC_SEM_VALUE_MAX))
        {
            errno = EINVAL;
            goto err;
        }

        /*initialization complete, turn off user-execute bit */
        if(fchmod(fd,mode) == -1)
            goto err;

        close(fd);
        sem->sem_magic = SEM_MAGIC;
        return sem;
     }
     
exists:
    if( (fd=open(pathname,O_RDWR)) <0)
    {
        if(errno = ENOENT && (oflag & O_CREAT))
            goto again;
        goto err;
    }

    sem=mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(sem==MAP_FAILED)
        goto err;

    /*make certain initialization is complete */
    for(i=0;i<MAX_TRIES;i++)
    {
        if(stat(pathname,&statbuff)== -1)
        {
            if(errno == ENOENT && (oflag & O_CREAT))
            {
                close(fd);
                goto again;
            }
            goto err;
        }
        if((statbuff.st_mode & S_IXUSR)== 0)
        {
            close(fd);
            sem->sem_magic = SEM_MAGIC;
            return sem;
        }
        sleep(1);
    
    }
    errno = ETIMEDOUT;
    goto err;
pthreaderr:
    errno =i;
err:
    /* don't let munmap() or close() change errno*/
    save_errno=errno;
    if(created)
        unlink(pathname);
    if(sem!=MAP_FAILED)
        munmap(sem,sizeof(sem_t));

    close(fd);
    errno = save_errno;
    return SEM_FAILED;

}