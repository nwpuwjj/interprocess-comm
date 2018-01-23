#include "svmsg.h"

void server(int,int);

int main(int argc, char **argv)
{
    int msqid;
    msqid=msgget(MQ_KEY1, SVMSG_MODE | IPC_CREAT);

    server(msqid,msqid);   /*same queue for both directions */

    exit(0);
}

void server(int readfd, int writefd)
{
    FILE *fp;
    char *ptr;
    pid_t pid;
    ssize_t n;
    struct mymesg mesg;
    memset(&mesg,0,sizeof(struct mymesg));
    for(;;)
    {
        /*read pathname from IPC channel*/
        mesg.mesg_type=1;

        if( (n=mesg_recv(readfd,&mesg))==0 )
        {
            DEBUG_PRINTF_RED("pathname missing\n");
            continue;
        }

        mesg.mesg_data[n]='\0';   /*null terminate pathname*/
        DEBUG_PRINTF_GRE("mesg:%s=====\n",mesg.mesg_data);
        if( (ptr=strchr(mesg.mesg_data,' ')) == NULL)
        {
            DEBUG_PRINTF_RED("bogus request:%s\n",mesg.mesg_data);
            continue;
        }

        *ptr++ = 0;              /*null terminate PID, ptr=pathname*/
        
        pid=atol(mesg.mesg_data);
        mesg.mesg_type = pid; /*for messages back to client */

        if( (fp=fopen(ptr,"r")) == NULL)
        {
            /*error: must tell client */
            snprintf(mesg.mesg_data+n,sizeof(mesg.mesg_data)-n,": can't open, %s\n", strerror(errno));
            mesg.mesg_len=strlen(ptr);
            memmove(mesg.mesg_data,ptr,mesg.mesg_len);
            mesg_send(writefd,&mesg);
        }
        else
        {
            /*fopen succeeded: copy file to IPC channel */
            while (fgets(mesg.mesg_data, 256,fp) != NULL)
            {
                mesg.mesg_len=strlen(mesg.mesg_data);
                mesg_send(writefd,&mesg);
            }
            fclose(fp);
        }

        /*send a 0-length message to signify the end */
        mesg.mesg_len=0;
        mesg_send(writefd,&mesg);
    }
}
