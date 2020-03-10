#include "common.h"
#include "linux/kernel.h"

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args,fmt);
	i = syslog_vprint(fmt,args);
	va_end(args);
	
	return i;
}