#include "unpipc.h"
#include <sys/msg.h>

int main(int argc ,char ** argv)
{
    int msqid;
    struct msqid_ds info;
    struct my_msgbuf buf;

    msqid = msgget(IPC_PRIVATE, SVMSG_MODE | IPC_CREAT);

    buf.mtype=1;
    buf.mtext[0]=1;
    msgsnd(msqid,&buf,1,0);

    msgctl(msqid,IPC_STAT,&info);
    
    DEBUG_PRINTF_GRE("read-write: %03o, cbytes=%lu, qnum=%lu, qbytes=%lu\n", info.msg_perm.mode & 0777, (ulong) info.msg_cbytes, (ulong) info.msg_qnum, (ulong) info.msg_qbytes);

    system("ipcs -q");

    msgctl(msqid, IPC_RMID, NULL);

    exit(0);
}
