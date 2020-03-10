#ifndef __SYSLOG_H__
#define __SYSLOG_H__

    int syslog_print(const char *fmt,...);
    int syslog_vprint(const char *fmt,va_list args);
    void syslog_debug(const char *str,const char *fmt,...);
    void syslog_info(const char *str,const char *fmt,...);
    void syslog_warning(const char *str,const char *fmt,...);
    void syslog_error(const char *str,const char *fmt,...);
    void syslog_dump_memory(void *mem,ulong length);
    void syslog_dump_otp(ulong mem,uint32_t length);
    void syslog_flag();
    void syslog_flagn(int n);
    void syslog_flagp(ulong p);

#endif