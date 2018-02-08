#include "unpipc.h"

char *gf_time() /*get the time*/
{
    struct timeval tv;
    static char str[30];
    char *ptr;

    if(gettimeofday(&tv,NULL) < 0)
        DEBUG_PRINTF_RED("gettimeofday error");

    ptr=ctime(&tv.tv_sec);
    strcpy(str,&ptr[11]);
    /* Fri Sep 13 00:00:00 1986\n\0 */
    snprintf(str+8,sizeof(str)-8,".%06ld",tv.tv_usec);
    return str;
}
int main(int argc, char **argv)
{
    int fd;
    fd=open("test1.data",O_RDWR|O_CREAT, FILE_MODE);
    read_lock(fd,0,SEEK_SET,0);    /*parent read locks entire file */
    DEBUG_PRINTF("%s, parent has read lock\n",gf_time());

    if (fork()==0)
    {
        /*first child */
        sleep(1);
        DEBUG_PRINTF("%s: first child tries to obtain write lock\n",gf_time());
        writew_lock(fd, 0, SEEK_SET, 0); /*this should block*/
        DEBUG_PRINTF("%s: first child obtains write lock\n",gf_time());
        sleep(2);
        un_lock(fd, 0, SEEK_SET, 0);
        DEBUG_PRINTF("%s:first child releases write lock\n",gf_time());
        exit(0);
    }

    if(fork()==0)
    {
        /*second child */
        sleep(3);
        DEBUG_PRINTF("%s:second child tries to obtain read lock\n",gf_time());
        readw_lock(fd, 0, SEEK_SET, 0);
        DEBUG_PRINTF("%s:second child contains read lock\n",gf_time());
        sleep(4);
        un_lock(fd,0,SEEK_SET,0);
        DEBUG_PRINTF("%s:second child release read lock\n",gf_time());
        exit(0);
    }

    /*parent*/
    sleep(5);
    un_lock(fd,0,SEEK_SET,0);
    DEBUG_PRINTF("%s,parent releases read lock\n",gf_time());
    exit(0);
    
   
}
