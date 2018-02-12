#include "unpipc.h"

#define LOCK_PATH "/tmp/svsemlock"
#define MAX_TRIES 10

int semid, initflag;
struct sembuf postop, waitop;

void my_lock(int fd)
{
    int oflag, i;
    union semun arg;
    struct semid_ds seminfo;

    if(initflag == 0)
    {
        oflag = IPC_CREAT | IPC_EXCL | SVSEM_MODE;
        if( (semid =semget(ftok(LOCK_PATH,0),1,oflag))>=0)
        {
            /* success, we're the first so initialize */
            arg.val = 1;
            semctl(semid,0, SETVAL, arg);
        }else if (errno == EEXIST )
        {
            /* someone else has created; make sure it's initialized */
            semid = semget(ftok(LOCK_PATH,0),1,SVSEM_MODE);
            arg.buf=&seminfo;
            for(i=0;i<MAX_TRIES;i++)
            {
                semctl(semid, 0 , IPC_STAT, arg);
                /*
                    查看信号量的sem_otime值，如果该值不为0，我们就知道创建该信号量的进程已对它初始化，并已调用semop(semop调用在本函数队尾)
                */
                if(arg.buf->sem_otime != 0)
                    goto init;
                sleep(1);
            }
            DEBUG_PRINTF_RED("semget ok, but semaphore not initialized\n");
            return;
        }
        else
        {
            DEBUG_PRINTF_RED("semget error\n");
            return;
        }

init:
        initflag = 1; 
        postop.sem_num = 0;     /* and init the two semop() structures */
        postop.sem_op = 1;
        postop.sem_flg = SEM_UNDO; //我们指定了sem_undo标志，这样的话如果某个进程在持有锁期间终止了，内核会释放该锁。
        waitop.sem_num = 0;
        waitop.sem_op = -1;       
        waitop.sem_flg = SEM_UNDO;
    }
    semop(semid, &waitop, 1);       /*down by 1 */ //等待值>=1, 若满足，则减一       
}

void my_unlock(int fd)
{
    semop(semid,&postop,1);     /* up by 1 */
}