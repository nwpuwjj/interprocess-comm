#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/* the fundamental datatype */
typedef struct {
    pthread_mutex_t sem_mutex;   /*lock to test and set semaphore value */
    pthread_cond_t sem_cond;    /*for transition from 0 to nonzero */
    unsigned int sem_count;     /*the actual semaphore value */
    int sem_magic;              /*magic number if open*/
}sem_t;

#define SEM_MAGIC 0x67458923

#ifdef SEM_FAILED
#undef SEM_FAILED
#define SEMFAILED ((sem_t *)(-1))  /*avoid compiler warnings */

#endif 