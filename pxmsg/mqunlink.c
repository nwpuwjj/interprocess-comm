#include "unpipc.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        DEBUG_PRINTF_RED("usage: mqunlink <name>");
        exit(0);
    }

    mq_unlink(argv[1]);

    exit(0);
}
