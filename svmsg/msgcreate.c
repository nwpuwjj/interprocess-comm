#include "unpipc.h"

int main(int argc, char ** argv)
{
    int c, oflag,mqid;
    key_t key;
    oflag = SVMSG_MODE | IPC_CREAT;

    while( (c=getopt(argc,argv,"e")) != -1)
    {
        switch(c)
        {
            case 'e':
                oflag |= IPC_EXCL;
                break;
        }
    }

    if (optind != argc -1)
    {
        DEBUG_PRINTF_RED("usage: msgcrate [-e] <pathname>");
    }
    
    key=ftok(argv[optind],0);
    if (key == -1)
    {
        DEBUG_PRINTF_RED("ftok fail===\n");
        return -1;
    }
    mqid =msgget(key,oflag);
    
    exit(0);
}
