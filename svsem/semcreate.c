#include "unpipc.h"

int main(int argc, char **argv)
{
    int c,oflag,semid,nsems;

    oflag= SVSEM_MODE | IPC_CREAT;

    while( (c=getopt(argc,argv,"e"))!=-1)
    {
        switch(c)
        {
            case 'e': 
                oflag |= IPC_EXCL;
                break;
        }
    }

    if (optind != argc -2)
    {
        DEBUG_PRINTF_RED("usage: semcreate [-e] <pathname> <nsems> \n");
        return 0;
    }

    nsems = atoi(argv[optind+1]);

    semid=semget(ftok(argv[optind],0),nsems,oflag);

    DEBUG_PRINTF_GRE("semcreate: semid: %d \n", semid);

    return 0;
}
