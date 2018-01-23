#include "unpipc.h"

#define MAXMESGDATA 256

struct mymesg{
    long int mesg_len;
    long int mesg_type;
    
    char mesg_data[256];
};

#define MQ_KEY1 1234L
#define MQ_KEY2 2345L
