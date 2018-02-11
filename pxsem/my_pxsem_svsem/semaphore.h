#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/*the fundamental datatype*/
typedef struct {
    int sem_semid;  /*the system V semaphore ID */
    int sem_magic;  /* magic number if open*/
}

#define SEM_MAGIC 0x45678923

#ifdef SEM_FAILED
#undef SEM_FAILED
#define  SEM_FAILED ((sem_t *)(-1))    /*avoid compiler warnings*/
#endif

#ifndef SEMVMX
#define SEMVMX 32767   /*historical system v max value for sem */
#endif