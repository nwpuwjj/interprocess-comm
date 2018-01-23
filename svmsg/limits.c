#include "unpipc.h"

#define MAX_DATA 64*1024
#define MAX_NMESG 4096
#define MAX_NIDS 4096
int max_mesg;

struct mymesg{
    long type;
    char data[MAX_DATA];
} mesg;

int main(int argc, char ** argv)
{
    int i,j,msqid,qid[MAX_NIDS];

    /*first try and determine maximum amount of data we can send */
    msqid = msgget(IPC_PRIVATE, SVMSG_MODE | IPC_CREAT);
    mesg.type =1;

    for( i=MAX_DATA; i>0; i-=128) {
        if (msgsnd(msqid,&mesg,i,0)==0)
        {
            DEBUG_PRINTF_GRE("maximum amount of data per message =%d\n",i);
            max_mesg=i;
            break;
        }

        if(errno != EINVAL)
        {
            DEBUG_PRINTF_RED("msgsnd error for length %d\n",i);
        }
    }

    if(i==0)
    {
        DEBUG_PRINTF_RED("i==0\n");
        return 0;
    }

    msgctl(msqid,IPC_RMID,NULL);

    /*see how many messages of varying size can be put onto a queue */
    mesg.type=1;
    for(i=8;i<=max_mesg;i*=2)
    {
        msqid=msgget(IPC_PRIVATE,SVMSG_MODE | IPC_CREAT);
        for(j=0;j<MAX_NMESG;j++)
        {
            if(msgsnd(msqid,&mesg,i,IPC_NOWAIT) !=0 )
            {
                if( errno == EAGAIN)
                    break;

                DEBUG_PRINTF_RED("msgsnd error, i=%d, j=%d\n",i,j);
                break;
            }
        }

        DEBUG_PRINTF_GRE("%d %d-byte messages were placed on queue,", i, j);
        DEBUG_PRINTF_GRE(" %d byte total\n",i*j);
        msgctl(msqid,IPC_RMID,NULL);
    }

    /*see how many identifiers we can open */
    mesg.type=1;
    for(i=0;i<=MAX_NIDS;i++)
    {
        if( (qid[i]=msgget(IPC_PRIVATE,SVMSG_MODE|IPC_CREAT)) == -1 )
        {
            DEBUG_PRINTF_GRE("%d identifiers open at once\n",i);
            break;
        }
    }
    DEBUG_PRINTF("i:%d\n",i);
    for(j=0;j<i;j++)
        msgctl(qid[j],IPC_RMID,NULL);

    return 0;
}
