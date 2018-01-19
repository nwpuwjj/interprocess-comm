#include "unpipc.h"

int main(int argc, char ** argv)
{
    int mqid;
    size_t len;
    long type;
    struct my_msgbuf *ptr;

    if (argc != 4)
    {
        DEBUG_PRINTF_RED("usage: msgsnd <pathname> <#bytes> <type>");
        return -1;
    }

    len =atoi(argv[2]);
    type =atoi(argv[3]);
    int id=ftok(argv[1],0);
    DEBUG_PRINTF("id:%d\n",id);
    if(id==-1)
    {
        DEBUG_PRINTF_RED("ftok fail===\n");
        return -1;
    }
    mqid=msgget(id,MSG_W);

    ptr=calloc(sizeof(long)+len,sizeof(char));
    char str[]="this is wjj";
    strcpy(ptr->mtext,str);
    ptr->mtype=type;

    msgsnd(mqid,ptr,len,0);

    exit(0);
}
