#ifndef _MQUEUE_H
#define _MQUEUE_H
typedef struct mq_info *mqd_t; /*opaque datatype*/

struct mq_attr{
    long mq_flags; /*message queue flag: O_NONBLOCK */
    long mq_maxmsg; /*max number of messages allowed on queue */
    long mq_msgsize; /*max size of a message (in bytes);*/
    long mq_curmsgs; /*number of messages currently on queues */
};

/* one mq_hdr() per queue, at beginning of mapped files */
struct mq_hdr
{
    struct mq_attr mqh_attr; /*the queue's attributes */
    long mqh_head; /*index of first message */
    long mqh_free; /* index of first free message */
    long mqh_nwait; /*#thread blocked in mq_receive() */
    pid_t mqh_pid; /*nonzero pid if mqh_event set */
    struct sigevent mqh_event; /*for mq_notify() */
    pthread_mutex_t mqh_lock; /*mutex lock */
    pthread_cond_t mqh_wait; /* and condition variable */
};

/*one msg_hdr{} at the front of each message in the mapped file */
struct msg_hdr
{
    long msg_next; /*index of next on linked list */
    
    /*msg_next must be first member in struct */
    ssize_t msg_len; /* actual lenght*/
    unsigned int msg_prio; /*priority */
};


/*one mq_info{} malloc'ed per process per mq_open() */
struct mq_info
{
    struct mq_hdr *mqi_hdr; /*start of mmap'ed region */
    long mqi_magic; /*magic num if open */
    int mqi_flags; /*flags for this process */
};

#define MQI_MAGIC 0x98765432

/*size of message in file is rounded up for alignment*/
#define MSGSIZE(i) ((((i) +sizeof(long) -1)/sizeof(long)) * sizeof(long))

#endif
