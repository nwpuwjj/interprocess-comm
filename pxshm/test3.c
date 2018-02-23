#include "unpipc.h"

int main(int argc, char ** argv)
{
    int fd1, fd2, *ptr1, *ptr2;
    pid_t childpid;
    struct stat stat;

    if(argc != 2)
    {
        DEBUG_PRINTF_RED("usage: test3 <name>\n");
        return 0;
    }


    shm_unlink(argv[1]);
    fd1=shm_open(argv[1],O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
    ftruncate(fd1,sizeof(int));
    fd2=open("/etc/motd",O_RDONLY);
    fstat(fd2,&stat);

    if( (childpid = fork()) == 0 )
    {
        //child 
        ptr2= mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
        ptr1= mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

        DEBUG_PRINTF_GRE("child: shm ptr= %p, motd ptr= %p\n",ptr1,ptr2);

        sleep(5);
        DEBUG_PRINTF_GRE("shared memory integer = %d\n", *ptr1);

        return 0;
    }

    /*parent: mmap in reverse order from child */
    ptr1 = mmap(NULL, sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    ptr2 = mmap(NULL, stat.st_size,PROT_READ ,MAP_SHARED, fd2, 0);

    DEBUG_PRINTF_GRE("parent: shm ptr=%p, motd ptr= %p \n",ptr1,ptr2);
    *ptr1=777;
    waitpid(childpid,NULL,0);
    return 0;
        
}
