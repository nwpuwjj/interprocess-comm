//#include "unpipc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define PATH_MAX 256
char * px_ipc_name(const char *name)
{
    char *dir, *dst, *slash;

    if ( (dst = malloc(PATH_MAX)) == NULL)
        return NULL;

    /*can override default directory with environment variable */

    if((dir = getenv("PX_IPC_NAME"))==NULL)
    {
 #ifdef POSIX_IPC_PEFIX
        dir=POSIX_IPC_PREFIX; // from "config.h"
 #else
        dir="/tmp";
 #endif
    }

    /*dir must end in slash*/
    slash = (dir[strlen(dir)-1]=='/')?"":"/";
    snprintf(dst,PATH_MAX,"%s%s%s",dir,slash,name);

    return (dst); //caller can free this pointer
}

/* debug
void main()
{
    printf("====main===\n");
    char *path=px_ipc_name("wjj");
    printf("===path:%s===\n",path);

}
*/
