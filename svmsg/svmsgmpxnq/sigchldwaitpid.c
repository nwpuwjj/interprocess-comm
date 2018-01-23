#include "unpipc.h"

void sig_child(int signo)
{
    pid_t pid;
    int stat;

    /*
    使用了WNOHANG（wait no hung) 参数调用waitpid，即使没有子进程退出，
    他也会立即返回，不会像wait那样永远等下。
    */
    while ( (pid=waitpid(-1,&stat,WNOHANG)) > 0);

    return;
}
