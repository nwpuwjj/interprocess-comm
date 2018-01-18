#include "unpipc.h"

static void sig_rt(int, siginfo_t *, void *);

int main(int argc, char **argv)
{
    int i,j;
    pid_t pid;
    sigset_t newset;
    union sigval val;

    DEBUG_PRINTF("SIGRTMIN=%d, SIGRTMAX=%d\n", (int)SIGRTMIN, (int)SIGRTMAX);

    if( (pid=fork()) == 0)
    {
        /*child: block three realtime signals */
        sigemptyset(&newset);
        sigaddset(&newset, SIGRTMAX);
        sigaddset(&newset, SIGRTMAX-1);
        sigaddset(&newset, SIGRTMAX-2);
        sigprocmask(SIG_BLOCK, &newset, NULL);

        /*establish signal handler with SA_SIGINFO set */
        signal_rt(SIGRTMAX,sig_rt,&newset);
        signal_rt(SIGRTMAX-1,sig_rt,&newset);
        signal_rt(SIGRTMAX-2,sig_rt,&newset);

        sleep(6);  /*let parent send all signals*/
        sigprocmask(SIG_UNBLOCK, &newset,NULL); /*unblock*/
        sleep(3); /* let all queued signals be delivered */

        exit(0);
    }

    /*parent sends nine signals to child */
    sleep(3);
    for(i=SIGRTMAX;i>=SIGRTMAX-2;i--)
    {
        for(j=0;j<=2;j++)
        {
            val.sival_int=j;
            sigqueue(pid,i,val);
            DEBUG_PRINTF_GRE("send signal %d, val=%d \n",i,j);
        }
    }

    exit(0);
}

static void sig_rt(int signo, siginfo_t *info, void* context)
{
    DEBUG_PRINTF_RED("received signal #%d, code = %d, ival= %d\n",signo, info->si_code, info->si_value.sival_int);
}
