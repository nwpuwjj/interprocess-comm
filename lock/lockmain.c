#include "unpipc.h"

#define SEQFILE "seqno" /*filename*/

void my_lock(int);
void my_unlock(int);

int main(int argc, char **argv)
{
    int fd;
    long i,seqno;
    pid_t pid;
    ssize_t n;
    char line[MAXLINE+1];

    pid=getpid();
    fd=open(SEQFILE,O_RDWR, FILE_MODE);

    for(i=0;i<20;i++)
    {
        my_lock(fd);        /*lock the file */

        lseek(fd,0L,SEEK_SET);  /*rewind before read */
        n=read(fd,line,MAXLINE);
        line[n]='\0';   /*null terminate for sscanf*/

        n=sscanf(line,"%ld\n",&seqno);

        DEBUG_PRINTF("%s: pid=%ld, seq#= %ld\n", argv[0],(long)pid, seqno);

        seqno++;   /*increment sequence number*/

        snprintf(line,sizeof(line),"%ld\n",seqno);
        lseek(fd,0L,SEEK_SET); /*rewind before write*/
        write(fd,line,strlen(line));
        my_unlock(fd);   /*unlock the file */
    }
    return 0;
}
