#include "common.h"
#include "linux/tty.h"

static char buf[1024];

#define _SYSLOG_PRINT_TEMPLATE(definename,outputname) \
    va_list args; \
    \
    if(_SYSLOG_##definename == 1) \
    { \
        syslog_print("[%s]\t%s : ",outputname,str); \
        va_start(args,fmt); \
        syslog_vprint(fmt,args); \
        va_end(args); \
        syslog_println(); \
    }

int syslog_print(const char *fmt,...)
{
    va_list args;
    int i;

    va_start(args,fmt);
    i = vsprintf(buf,fmt,args);
    va_end(args);
    uart_send_data(UART_DEVICE_3,buf,i);
    //tty_write(0,buf,i);
    return i;
}

int syslog_vprint(const char *fmt,va_list args)
{
    int i;

    i = vsprintf(buf,fmt,args);
    uart_send_data(UART_DEVICE_3,buf,i);
    //tty_write(0,buf,i);
    return i;
}

void syslog_println()
{
    syslog_print("\r\n");
}

void syslog_debug(const char *str,const char *fmt,...)
{
    _SYSLOG_PRINT_TEMPLATE(DEBUG,"debug");
}

void syslog_info(const char *str,const char *fmt,...)
{
    _SYSLOG_PRINT_TEMPLATE(INFO,"info");
}

void syslog_warning(const char *str,const char *fmt,...)
{
    _SYSLOG_PRINT_TEMPLATE(WARNING,"warning");
}

void syslog_error(const char *str,const char *fmt,...)
{
    _SYSLOG_PRINT_TEMPLATE(ERROR,"error");
}

void syslog_dump_memory(void *mem,ulong length)
{
    syslog_print("syslog_dump_memory:addr = 0x%08X%08X,length = 0x%08X%08X",((ulong)mem) >> 32,((ulong)mem) & 0xFFFFFFFFUL,length >> 32,length & 0xFFFFFFFFUL);
    syslog_print("                   ");

    for(int i = 0;i < 16;i++)
    {
        syslog_print("%02X ",i);
    }

    char *buf = (char *)(((ulong)mem) & (~0x0F));
    length += ((ulong)mem) - ((ulong)buf);
    
    for(int i = 0;i < length;i++)
    {
        if((i & 0x0F) == 0x00)
        {
            syslog_print("\r\n");
            syslog_print("0x%08X%08X ",(((ulong)buf) + i) >> 32,(((ulong)buf) + i) & 0xFFFFFFFFUL);
        }

        syslog_print("%02X ",(ulong)buf[i]);
    }

    syslog_print("\r\n");
}

void syslog_dump_otp(ulong mem,uint32_t length)
{
    syslog_debug("syslog_dump_otp","addr = 0x%04X,length = 0x%04X",mem,length);
    syslog_print("       ");

    for(int i = 0;i < 16;i++)
    {
        syslog_print("%02X ",i);
    }
    
    uint32_t buf = (((uint32_t)mem) & (~0x0F));
    length += ((uint32_t)mem) - ((uint32_t)buf);
    
    for(int i = 0;i < length;i++)
    {
        if((i & 0x0F) == 0x00)
        {
            syslog_print("\r\n");
            syslog_print("0x%04X ",(ulong)(buf + i));
        }

        char t;
        int rv = otp_read_inner(buf + i,(uint8_t *)&t,1);

        if(rv != 0)
        {
            syslog_print("XX ");
        }
        else
        {
            syslog_print("%02X ",(ulong)t);
        }
    }

    syslog_print("\r\n");
}

void syslog_flag()
{
    syslog_debug("syslog_flag","%s","flag");
}

void syslog_flagn(int n)
{
    syslog_debug("syslog_flag","%s = %d","flag",n);
}

void syslog_flagp(ulong p)
{
    //syslog_debug("syslog_flag","%s = %p","flag",p);
    syslog_print("syslog_flag:%s = %p\r\n","flag",p);
}