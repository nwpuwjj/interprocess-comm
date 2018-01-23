#ifndef _MESG_H
#define _MESG_H

#include "unpipc.h"

#define PIPE_BUF 1024
/*our own "messages" to use with pipes, fifos, and message queues */

/* want sizeof(struct  mymesg <= PIPE_BUF) */
#define MAXMESGDATA (PIPE_BUF - 2*sizeof(long))

/*lenth of mesg_len and mesg_type */
#define MESGHDRSIZE (sizeof(struct mymesg) - MAXMESGDATA)

struct mymesg {
    long mesg_len;  /*#bytes in mesg_data, can be 0 */
    long mesg_type;  /*message type ,must be > 0 */
    char mesg_data[MAXMESGDATA];
};

ssize_t mesg_send(int, struct mymesg *);
ssize_t mesg_recv(int,struct mymesg *);

#endif 
