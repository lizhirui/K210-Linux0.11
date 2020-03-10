#ifndef __SYS_UTSNAME_H__
#define __SYS_UTSNAME_H__

    #include <sys/types.h>

    struct utsname
    {
	    char sysname[9];
	    char nodename[9];
	    char release[9];
	    char version[9];
	    char machine[9];
    };

    extern int uname(struct utsname * utsbuf);

#endif
