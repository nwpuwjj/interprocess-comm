/*
    内核跟song着被内存映射的底层支撑对象的大小，而且我们总是能访问到能在当前文件大小以内又在内存映射区以内的那些字节
*/

#include "unpipc.h"
#define FILE "test.data"
#define SIZE 32768

int main(int argc, char **argv)
{
    int fd, i;
    char *ptr;

    /*open: create or truncate; then mmap file*/
    fd = open(FILE,O_RDWR|O_CREAT|O_TRUNC,FILE_MODE);
    ptr=mmap(NULL,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    for(i=4096; i<=SIZE; i+=4096)
    {
        DEBUG_PRINTF("setting file size to %d\n",i);
        ftruncate(fd,i);
        DEBUG_PRINTF("ptr[%d]=%d\n",i-1,ptr[i-1]);
    }

    return 0;
}
