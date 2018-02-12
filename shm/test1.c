/*
    内核知道被映射的底层支撑对象的大小，即使该对象的描述符已经关闭也一样。
    内核允许我们给mmap指定一个大于该对象大小的大小参数，但是我们访问不了该对象以外的部分
*/
#include "unpipc.h"
#define max(x,y) x>y?x:y

int main(int argc, char ** argv)
{
    int fd, i;
    char *ptr;
    size_t filesize, mmapsize, pagesize;

    if(argc != 4)
    {
        DEBUG_PRINTF_RED("usage: test1 <pathname> <filesize> <mmapsize> /n");
        return 0;
    }

    filesize = atoi(argv[2]);
    mmapsize = atoi(argv[3]);

    /*open file : create or truncate; set file size */
    fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE);
    lseek(fd, filesize-1, SEEK_SET);
    write(fd,"",1);
    ptr=mmap(NULL,mmapsize,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    pagesize = sysconf(_SC_PAGESIZE);
    DEBUG_PRINTF("PAGESIZE = %ld\n", (long)pagesize);

    for(i=0;i<max(filesize,mmapsize);i+=pagesize)
    {
        DEBUG_PRINTF("ptr[%d]= %d\n",i,ptr[i]);
        ptr[i]=1;
        DEBUG_PRINTF("ptr[%d]=%d\n",i+pagesize-1, ptr[i+pagesize-1]);
        ptr[i+pagesize-1]=1;
    }

    DEBUG_PRINTF("ptr[%d]=%d\n",i,ptr[i]);
    return 0;
}
