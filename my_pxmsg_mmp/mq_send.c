#include "unpipc.h"
#include "mqueue.h"

int mq_send(mqd_t mqd, const char *ptr, size_t len, unsigned int prio)
{
    int n;
    long index, freeindex;
    int8_t *mptr;
    struct sigevent *sigev;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct msg_hdr *msghdr, *nmsghdr, *pmsghdr;
    struct msg_info *mqinfo;

    mqinfo=mqd;
    if( mqinfo->mqi_magic != MQI_MAGIC)
    {
        errno=EBADF;
        return -1;
    }
    mqhdr=mqinfo->mqi_hdr;        /*struct pointer*/
    mptr=(int8_t *)mqhdr;         /*byte point*/
    attr= &mqhdr->mqh_attr;

    if( (n=pthread_mutex_lock(&mqhdr->mqh_lock))!=0)
    {
        errno=n;
        return -1;
    }

    if(len > attr->mq_msgsize)
    {
        errno=EMSGSIZE;
        goto err;
    }
    /*如果是往空队列中放置消息，则检查是否有某个进程被注册为接收队列通知，并检查是否有某个线程阻塞在mq_receive 调用中*/
    if (attr->mq_curmsgs==0)
    {
        if(mqhdr->mqh_pid !=0 && mqhdr->mqh_nwait==0)
        {
            sigev=&mqhdr->mqh_event;
            if(sigev->sigev_notify==SIGEV_SIGNAL)
            {
                sigqueue(mqhdr->mqh_pid,sigev->sigev_signo,sigev->sigev_value);
            }
            mqhdr->mqh_pid =0;                 /*unregister*/
        }        
    }
    //如果调用者指定的队列已填满，但是O_NONBLOCK标志已设置，我们就返回一个EAGAIN错误。否则，我们等待在条件变量mqh_wait 上，该条件变量由我们的mq_receive函数在从某个填满的队列中读出一个消息时发给信号
    else if (attr->mq_curmsgs >= attr->mq_maxmsg)
    {
        /*queue is full */
        if(mqinfo->mqi_flags & O_NONBLOCK)
        {
            errno = EAGAIN;
            goto err;
        }

        /*wait for room for one message on the queue */
        while (attr->mq_curmsgs >= attr->mq_maxmsg)
            pthread_cond_wai(&mqhdr->mqh_wait,&mqhdr->mqh_lock);
    }

    /*nmsghdr will point to new message */
    if ((freeindex = mqhdr->mqh_free)==0)
    {
        DEBUG_PRINTF_RED("mq_send: curmsg=%ld; free =0 ", attr->mq_curmsgs);
        return (0);
    }

    nmsghdr = (struct msg_hdr *)&mptr[freeindex];
    nmsghdr->msg_prio=prio;
    nmsghdr->msg_len=len;
    memcpy(nmsghdr+1,ptr,len);           /*copy message from caller*/
    mqhdr->mqh_free = nmsghdr->msg_next;   /*new freelist head*/  //mqh_free指向空闲块，不必再加入下面的逻辑

    /*find right place for message in linked list */
    //我们的链表中各消息的顺序是从开始处（mqh_head)的最高优先级到结束处的最低优先级。当一个新消息加入调用者指定的队列中，
    //并且一个或多个同样优先级的消息已在该队列中时，这个新消息就加在最后一个优先级相同的消息之后。使用这样的排序方式后，mq_receive
    //总是返回链表中的第一个消息（它是该队列上优先级最高的最早的消息）
    index=mqhdr->mqh_head;
    pmsghdr=(struct msg_hdr *) &(mqhdr->mqh_head);

    while(index!=0)
    {
        msghdr=(struct msg_hdr *) &mptr[index];
        if(prio>msghdr->msg_prio)
        {
            nmsghdr->msg_next=index;
            pmsghdr->msg_next=freeindex; //freeindex为新加mesage的地址
            break;
        }
        index=msghdr->msg_next;
        pmsghdr=msghdr;
    }

    if(index==0)
    {
        /*queue was empty or new goes at end of list */
        pmsghdr->msg_next = freeindex;
        nmsghdr->msg_next = 0;
    }

    /*wake up anyone blocked in mq_receive waiting for a message*/
    if(attr->mq_curmsgs == 0)
        pthread_cond_signal(&mqhdr->mqh_wait);

    attr->mq_curmsgs++;

    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return 0;

err:
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return -1;
}
