#include "unpipc.h"
#include "mesg.h"
#include "svmsg.h"

void server(int,int);

int main(int argc, char **argv)
{
    int msqid;
    msqid=msgget(MQ_KEY1, SVMSG_MODE | IPC_CREAT);

    server(msqid,msqid);  /*same queue for both directions */

    exit(0);
}

void server(int readid,int writeid)
{
    FILE *fp;
    char *ptr;
    ssize_t n;
    struct mymesg mesg;
    void sig_child(int);

    signal(SIGCHLD,sig_child);

    for(;;)
    {
        /*read pathname from our well-known queue */
        mesg.mesg_type=1;
        if( (n=Mesg_recv(readid,&mesg))==0)
        {
            DEBUG_PRINTF_RED("pathname missing\n");
            continue;
        }

        mesg.mesg_data[n]='\0'; /*null terminate pathname*/
        DEBUG_PRINTF_RED("mesg_data: %s \n",mesg.mesg_data);
        if((ptr=strchr(mesg.mesg_data, ' '))==NULL)
        {
            DEBUG_PRINTF_RED("bogus request:%s",mesg.mesg_data);
            continue;
        }

        *ptr++=0;  /*null terminate msgid, ptr=pathname*/

        writeid=atoi(mesg.mesg_data);

        if(fork()==0)
        {
            /*child*/

            if( (fp=fopen(ptr,"r"))==NULL)
            {
                /*error: must tell client*/
                snprintf(mesg.mesg_data+n, sizeof(mesg.mesg_data)-n,": can't open, %s\n", strerror(errno));
                mesg.mesg_len = strlen(ptr);
                memmove(mesg.mesg_data,ptr,mesg.mesg_len);
                mesg_send(writeid,&mesg);
            }
            else
            {
                /*fopen succeeded; copy file to client's queue */
                while(fgets(mesg.mesg_data,MAXMESGDATA,fp)!=NULL)
                {
                    mesg.mesg_len = strlen(mesg.mesg_data);
                    mesg_send(writeid,&mesg);
                }

                fclose(fp);
            }

            /*send a 0-length message to signify the end */
            mesg.mesg_len=0;
            mesg_send(writeid,&mesg);
            exit(0);  /*child terminates */
        }
        /*parent just loops around*/

    }
}
