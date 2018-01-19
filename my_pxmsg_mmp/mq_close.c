#include "unpipc.h"
#include "mqueue.h"

int mq_close(mqd_t mqd)
{
    long msgsize, filesize;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct mq_info *mqinfo;

    mqinfo =mqd;

    if(mqinfo->mqi_magic != MQI_MAGIC)
    {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    attr=&mqhdr->mqh_attr;

    /*unregister calling process */
    if(mq_notify(mqd,NULL)!=0)
        return (-1);

    msgsize = MSGSIZE(attr->mq_msgsize);
    filesize = sizeof(struct mq_hdr) + (attr->mq_maxmsg * (sizeof(struct msg_hdr)+msgsize));

    if(munmap(mqinfo->mqi_hdr,filesize)==-1)
        return (-1);

    mqinfo->mqi_magic=0; /*just in case*/
    free(mqinfo);
    return 0;
}
