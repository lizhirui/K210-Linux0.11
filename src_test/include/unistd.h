#ifndef __UNISTD_H__
#define __UNISTD_H__

    /* ok, this may be a joke, but I'm working on it */
    #define _POSIX_VERSION 198808L

    #define _POSIX_CHOWN_RESTRICTED	/* only root can do a chown (I think..) */
    #define _POSIX_NO_TRUNC		/* no pathname truncation (but see in kernel) */
    #define _POSIX_VDISABLE '\0'	/* character to disable things like ^C */
    /*#define _POSIX_SAVED_IDS */	/* we'll get to this yet */
    /*#define _POSIX_JOB_CONTROL */	/* we aren't there quite yet. Soon hopefully */

    #define STDIN_FILENO	0
    #define STDOUT_FILENO	1
    #define STDERR_FILENO	2

    #ifndef NULL
    #define NULL    ((void *)0)
    #endif

    /* access */
    #define F_OK	0
    #define X_OK	1
    #define W_OK	2
    #define R_OK	4

    /* lseek */
    #define SEEK_SET	0
    #define SEEK_CUR	1
    #define SEEK_END	2

    /* _SC stands for System Configuration. We don't use them much */
    #define _SC_ARG_MAX		1
    #define _SC_CHILD_MAX		2
    #define _SC_CLOCKS_PER_SEC	3
    #define _SC_NGROUPS_MAX		4
    #define _SC_OPEN_MAX		5
    #define _SC_JOB_CONTROL		6
    #define _SC_SAVED_IDS		7
    #define _SC_VERSION		8

    /* more (possibly) configurable things - now pathnames */
    #define _PC_LINK_MAX		1
    #define _PC_MAX_CANON		2
    #define _PC_MAX_INPUT		3
    #define _PC_NAME_MAX		4
    #define _PC_PATH_MAX		5
    #define _PC_PIPE_BUF		6
    #define _PC_NO_TRUNC		7
    #define _PC_VDISABLE		8
    #define _PC_CHOWN_RESTRICTED	9

    //#include <sys/stat.h>
    #include "sys/times.h"
    #include "sys/utsname.h"
    //#include <utime.h>

    #ifdef __LIBRARY__

    /*#define __NR_setup	0
    #define __NR_exit	1
    #define __NR_fork	2
    #define __NR_read	3
    #define __NR_write	4
    #define __NR_open	5
    #define __NR_close	6
    #define __NR_waitpid	7
    #define __NR_creat	8
    #define __NR_link	9
    #define __NR_unlink	10
    #define __NR_execve	11
    #define __NR_chdir	12
    #define __NR_time	13
    #define __NR_mknod	14
    #define __NR_chmod	15
    #define __NR_chown	16
    #define __NR_break	17
    #define __NR_stat	18
    #define __NR_lseek	19
    #define __NR_getpid	20
    #define __NR_mount	21
    #define __NR_umount	22
    #define __NR_setuid	23
    #define __NR_getuid	24
    #define __NR_stime	25
    #define __NR_ptrace	26
    #define __NR_alarm	27
    #define __NR_fstat	28
    #define __NR_pause	29
    #define __NR_utime	30
    #define __NR_stty	31
    #define __NR_gtty	32
    #define __NR_access	33
    #define __NR_nice	34
    #define __NR_ftime	35
    #define __NR_sync	36
    #define __NR_kill	37
    #define __NR_rename	38
    #define __NR_mkdir	39
    #define __NR_rmdir	40
    #define __NR_dup	41
    #define __NR_pipe	42
    #define __NR_times	43
    #define __NR_prof	44
    #define __NR_brk	45
    #define __NR_setgid	46
    #define __NR_getgid	47
    #define __NR_signal	48
    #define __NR_geteuid	49
    #define __NR_getegid	50
    #define __NR_acct	51
    #define __NR_phys	52
    #define __NR_lock	53
    #define __NR_ioctl	54
    #define __NR_fcntl	55
    #define __NR_mpx	56
    #define __NR_setpgid	57
    #define __NR_ulimit	58
    #define __NR_uname	59
    #define __NR_umask	60
    #define __NR_chroot	61
    #define __NR_ustat	62
    #define __NR_dup2	63
    #define __NR_getppid	64
    #define __NR_getpgrp	65
    #define __NR_setsid	66
    #define __NR_sigaction	67
    #define __NR_sgetmask	68
    #define __NR_ssetmask	69
    #define __NR_setreuid	70
    #define __NR_setregid	71
    #define __NR_debug 72*/

    #define __NR_setup 0
    #define __NR_fork 1
    #define __NR_waitpid 2
    #define __NR_creat 3
    #define __NR_execve 4
    #define __NR_mknod 5
    #define __NR_chmod 6
    #define __NR_chown 7
    #define __NR_break 8
    #define __NR_mount 9
    #define __NR_umount 10
    #define __NR_setuid 11
    #define __NR_stime 12
    #define __NR_ptrace 13
    #define __NR_alarm 14
    #define __NR_pause 15
    #define __NR_utime 16
    #define __NR_stty 18
    #define __NR_gtty 19
    #define __NR_nice 20
    #define __NR_ftime 21
    #define __NR_sync 22
    #define __NR_dup 23
    #define __NR_rename 24
    #define __NR_fcntl 25
    #define __NR_rmdir 26
    #define __NR_pipe 27
    #define __NR_prof 28
    #define __NR_setgid 29
    #define __NR_signal 30
    #define __NR_acct 31
    #define __NR_phys 32
    #define __NR_lock 33
    #define __NR_ioctl 34
    #define __NR_mpx 35
    #define __NR_setpgid 36
    #define __NR_ulimit 37
    #define __NR_umask 38
    #define __NR_chroot 39
    #define __NR_ustat 40
    #define __NR_dup2 41
    #define __NR_getppid 42
    #define __NR_getpgrp 43
    #define __NR_setsid 44
    #define __NR_sigaction 45
    #define __NR_sgetmask 46
    #define __NR_ssetmask 47
    #define __NR_chdir 49
    #define __NR_setreuid 50
    #define __NR_setregid 51
    #define __NR_debug 52
    #define __NR_close 57
    #define __NR_lseek 62
    #define __NR_read 63
    #define __NR_write 64
    #define __NR_fstat 80
    #define __NR_exit 93
    #define __NR_kill 129
    #define __NR_times 153
    #define __NR_uname 160
    #define __NR_getpid 172
    #define __NR_getuid 174
    #define __NR_geteuid 175
    #define __NR_getgid 176
    #define __NR_getegid 177
    #define __NR_brk 214
    #define __NR_open 1024
    #define __NR_link 1025
    #define __NR_unlink 1026
    #define __NR_mkdir 1030
    #define __NR_access 1033
    #define __NR_stat 1038
    #define __NR_time 1062

    //此处是用a0进行的参数传递，所以对于多参数的syscall，li a0,%1必须放在最后，否则第一个mv a1,%2会变成mv a1,a0，结合前面的li a0,%1会出错，以此类推，要倒序排列
    /*#define _syscall0(type,name) \
    type usersyscall_##name(void) \
    { \
    int64_t __res; \
    asm volatile ("li a7,%1;ecall;mv %0,a0" \
	    : "=r" (__res) \
	    : "i" (__NR_##name)); \
    if (__res >= 0) \
	    return (type) __res; \
    errno = -__res; \
    return -1; \
    }

    #define _syscall1(type,name,atype,a) \
    type usersyscall_##name(atype a) \
    { \
    int64_t __res; \
    asm volatile ("mv a0,%2;li a7,%1;ecall;mv %0,a0" \
	    : "=r" (__res) \
	    : "i" (__NR_##name),"r" ((long)(a))); \
    if (__res >= 0) \
	    return (type) __res; \
    errno = -__res; \
    return -1; \
    }

    #define _syscall2(type,name,atype,a,btype,b) \
    type usersyscall_##name(atype a,btype b) \
    { \
    int64_t __res; \
    asm volatile ("mv a1,%3;mv a0,%2;li a7,%1;ecall;mv %0,a0" \
	    : "=r" (__res) \
	    : "i" (__NR_##name),"r" ((int64_t)(a)),"r" ((int64_t)(b))); \
    if (__res >= 0) \
	    return (type) __res; \
    errno = -__res; \
    return -1; \
    }

    #define _syscall3(type,name,atype,a,btype,b,ctype,c) \
    type usersyscall_##name(atype a,btype b,ctype c) \
    { \
    int64_t __res; \
    asm volatile ("mv a2,%4;mv a1,%3;mv a0,%2;li a7,%1;ecall;mv %0,a0" \
	    : "=x2" (__res) \
	    : "i" (__NR_##name),"x2" ((long)(a)),"x3" ((long)(b)),"x4" ((long)(c))); \
    if (__res>=0) \
	    return (type) __res; \
    errno=-__res; \
    return -1; \
    }*/

    #define _syscall0(type,name) \
    type usersyscall_##name(void) \
    { \
        int64_t __res; \
        register ulong a7 asm("a7") = __NR_##name;\
        asm volatile ("ecall;mv %0,a0" \
	        : "=r" (__res) \
	        : "r"(a7)); \
        \
        if(__res >= 0) \
        {\
	        return (type) __res; \
        }\
        \
        errno = -__res; \
        return -1; \
    }

    #define _syscall1(type,name,atype,a) \
    type usersyscall_##name(atype a) \
    { \
        int64_t __res; \
        register ulong a7 asm("a7") = __NR_##name;\
        register ulong a0 asm("a0") = (ulong)a;\
        asm volatile ("ecall;mv %0,a0" \
	        : "=r" (__res) \
	        : "r"(a7),"r"(a0)); \
        \
        if(__res >= 0) \
        {\
	        return (type) __res; \
        }\
        \
        errno = -__res; \
        return -1; \
    }

    #define _syscall2(type,name,atype,a,btype,b) \
    type usersyscall_##name(atype a,btype b) \
    { \
        int64_t __res; \
        register ulong a7 asm("a7") = __NR_##name;\
        register ulong a0 asm("a0") = (ulong)a;\
        register ulong a1 asm("a1") = (ulong)b;\
        asm volatile ("ecall;mv %0,a0" \
	        : "=r" (__res) \
	        : "r"(a7),"r"(a0),"r"(a1)); \
        \
        if(__res >= 0) \
        {\
	        return (type) __res; \
        }\
        \
        errno = -__res; \
        return -1; \
    }

    #define _syscall3(type,name,atype,a,btype,b,ctype,c) \
    type usersyscall_##name(atype a,btype b,ctype c) \
    { \
        int64_t __res; \
        register ulong a7 asm("a7") = __NR_##name;\
        register ulong a0 asm("a0") = (ulong)a;\
        register ulong a1 asm("a1") = (ulong)b;\
        register ulong a2 asm("a2") = (ulong)c;\
        asm volatile ("ecall;mv %0,a0" \
	        : "=r" (__res) \
	        : "r"(a7),"r"(a0),"r"(a1),"r"(a2)); \
        \
        if(__res >= 0) \
        {\
	        return (type) __res; \
        }\
        \
        errno = -__res; \
        return -1; \
    }

    #endif /* __LIBRARY__ */

    extern int errno;

    /*int access(const char * filename, mode_t mode);
    int acct(const char * filename);
    int alarm(int sec);
    int brk(void * end_data_segment);
    void * sbrk(ptrdiff_t increment);
    int chdir(const char * filename);
    int chmod(const char * filename, mode_t mode);
    int chown(const char * filename, uid_t owner, gid_t group);
    int chroot(const char * filename);
    int close(int fildes);
    int creat(const char * filename, mode_t mode);
    int dup(int fildes);
    int execve(const char * filename, char ** argv, char ** envp);
    int execv(const char * pathname, char ** argv);
    int execvp(const char * file, char ** argv);
    int execl(const char * pathname, char * arg0, ...);
    int execlp(const char * file, char * arg0, ...);
    int execle(const char * pathname, char * arg0, ...);
    volatile void exit(int status);
    volatile void _exit(int status);
    int fcntl(int fildes, int cmd, ...);
    int fork(void);
    int getpid(void);
    int getuid(void);
    int geteuid(void);
    int getgid(void);
    int getegid(void);
    int ioctl(int fildes, int cmd, ...);
    int kill(pid_t pid, int signal);
    int link(const char * filename1, const char * filename2);
    int lseek(int fildes, off_t offset, int origin);
    int mknod(const char * filename, mode_t mode, dev_t dev);
    int mount(const char * specialfile, const char * dir, int rwflag);
    int nice(int val);
    int open(const char * filename, int flag, ...);
    int pause(void);
    int pipe(int * fildes);
    int read(int fildes, char * buf, off_t count);
    int setpgrp(void);
    int setpgid(pid_t pid,pid_t pgid);
    int setuid(uid_t uid);
    int setgid(gid_t gid);
    void (*signal(int sig, void (*fn)(int)))(int);
    int stat(const char * filename, struct stat * stat_buf);
    int fstat(int fildes, struct stat * stat_buf);
    int stime(time_t * tptr);
    int sync(void);
    time_t time(time_t * tloc);
    time_t times(struct tms * tbuf);
    int ulimit(int cmd, long limit);
    mode_t umask(mode_t mask);
    int umount(const char * specialfile);
    int uname(struct utsname * name);
    int unlink(const char * filename);
    int ustat(dev_t dev, struct ustat * ubuf);
    int utime(const char * filename, struct utimbuf * times);
    pid_t waitpid(pid_t pid,int * wait_stat,int options);
    pid_t wait(int * wait_stat);
    int write(int fildes, const char * buf, off_t count);
    int dup2(int oldfd, int newfd);
    int getppid(void);
    pid_t getpgrp(void);
    pid_t setsid(void);*/

#endif