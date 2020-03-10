#include "common.h"

otp_read_func otp_read_inner = (otp_read_func)0x8800453C;

uint32_t otp_read_serial()
{
    uint32_t id;
    int rv = otp_read_inner(0x3D9C,(uint8_t *)&id,sizeof(uint32_t));

    if(rv == 0)
    {
        return id;
    }

    return 0xFFFFFFFF;
}