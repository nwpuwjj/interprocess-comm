#include "unpipc.h"
#include "pthread_rwlock.h"

int pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
    int result;

    if(rw->rw_magic != RW_MAGIC)
        return EINVAL;

    if((result=pthread_mutex_lock(&rw->rw_mutex))!=0)
        return result;

    if(rw->rw_refcount !=0)
        result = EBUSY; /*held by either writer or readers*/
    else
        rw->rw_refcount= -1; /*available, indicate a writer has it */

    pthread_mutex_unlock(&rw->rw_mutex);
    return result;
}
