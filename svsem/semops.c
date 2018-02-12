#include "unpipc.h"

int main(int argc, char **argv)
{
    int c,i,flag,semid,nops;
    struct sembuf *ptr;

    flag=0;
    while( (c=getopt(argc,argv,"nu")) != -1)
    {
        switch(c){

            case 'n':
                flag |= IPC_NOWAIT;  /*for each operation */
                break;
            case 'u':
                flag |= SEM_UNDO;   /* for each operation*/
                break;
        }
               
    }

    if(argc-optind <2)      /*argc - optind = #args reamaining*/
    {
        DEBUG_PRINTF_RED("usage: semops [-n] [-u] <pathname> operation ... \n");
        return 0;
    }

    semid = semget(ftok(argv[optind],0),0,0);

    DEBUG_PRINTF_GRE("semops semid: %d \n",semid);
    optind++;
    nops = argc - optind;

    /*allocate memory to hold operations, store and perform */
    ptr = calloc(nops, sizeof(struct sembuf));
    for(i=0;i<nops;i++)
    {
        ptr[i].sem_num = i;
        ptr[i].sem_op = atoi(argv[optind+i]);   /* <0, 0, or >0 */
        ptr[i].sem_flg = flag;
    }

    semop(semid,ptr,nops);

    return 0;
}
