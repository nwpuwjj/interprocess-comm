#include "unpipc.h"

int main(int argc, char **argv)
{
    if ( argc != 2)
    {
        DEBUG_PRINTF_RED("usage: semunlink <name>");
        return 0;
    }

    sem_unlink(argv[1]);

    exit(0);
}
