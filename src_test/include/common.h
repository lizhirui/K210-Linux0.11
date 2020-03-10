#ifndef __COMMON_H__
#define __COMMON_H__
    
    #define _SYSLOG_DEBUG 0
    #define _SYSLOG_INFO 0
    #define _SYSLOG_WARNING 0
    #define _SYSLOG_ERROR 0

    #ifndef _VS
	    typedef unsigned int uint;
	    typedef unsigned long ulong;
	    typedef unsigned long uint64;
        typedef signed char int8_t;
        typedef short int int16_t;
        typedef int int32_t;
        typedef long int64_t;
        typedef unsigned char uint8_t;
        typedef unsigned short int uint16_t;
        typedef unsigned int uint32_t;
        typedef unsigned long uint64_t;
        typedef unsigned long uintptr_t;
        #define ALIGN4BYTE __attribute__((packed, aligned(4)))
        #define ALIGN4K __attribute__((packed, aligned(4096)))
    #else
        typedef unsigned int uint;
	    typedef unsigned long long ulong;
	    typedef unsigned long long uint64;
        typedef signed char int8_t;
        typedef short int int16_t;
        typedef int int32_t;
        typedef long long int64_t;
        typedef unsigned char uint8_t;
        typedef unsigned short int uint16_t;
        typedef unsigned int uint32_t;
        typedef unsigned long long uint64_t;
        typedef unsigned long long uintptr_t;
        #define asm
        #define volatile(x)
        #define typeof(x)
        #define __attribute__(x)
        #define ALIGN4BYTE
        #define ALIGN4K
    #endif

    #define NULL ((void *)0)
    #define EOF (-1)

    #define UINT32_MAX 0xFFFFFFFFU
    #define INT32_MAX 0x7FFFFFFF

    #define get_member_offset(structname,membername) ((ulong)(&(((structname *)(0)) -> membername)))
    #define bool_to_bit(v) (((v) != 0) ? 1 : 0)

    #include "stdio.h"
    #include "sys/lock.h"
    #include "stdarg.h"
    #include "stdbool.h"
    #include "math.h"
    #include "string.h"
    #include "time.h"
    #include "trap.h"
    #include "drivers.h"
    #include "riscvfunc.h"

    #define UART_DEBUG_DEVICE UART_DEVICE_3

    int64_t bitscan(uint64_t v);

#endif