#include "common.h"

int64_t bitscan(uint64_t v)
{
    int i;

    for(i = 0;i < 64;i++)
    {
        if(v & 0x01)
        {
            return i;
        }

        v >>= 1;
    }

    return -1;
}