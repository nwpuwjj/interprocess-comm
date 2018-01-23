#include "unpipc.h"
#include "mesg.h"
#include "svmsg.h"

ssize_t mesg_recv(int id, struct mymesg *mptr)
{
    ssize_t n;

    n=msgrcv(id, &(mptr->mesg_type), MAXMESGDATA, mptr->mesg_type,0);
    mptr->mesg_len=n;            /*return #bytes of data */

    return(n);                   /*-1 on error, 0 at EOF, else >0 */
}

ssize_t Mesg_recv(int id, struct mymesg *mptr)
{
    ssize_t n;

    do {
        n=mesg_recv(id, mptr);
    }while(n == -1 && errno==EINTR);

    if(n==-1)
    {
        DEBUG_PRINTF_RED("mesg_recv error\n");
    }

    return n;
}
