#ifndef __OTP_H__
#define __OTP_H__

    typedef int (*otp_read_func)(uint32_t offset,uint8_t *dest,uint32_t size);

    extern otp_read_func otp_read_inner;
    uint32_t otp_read_serial();

#endif