//#include "common.h"

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

typedef long ssize_t;

#define NAME_LEN 14

#define NULL ((void *)0)
#define EOF (-1)

#define UINT32_MAX 0xFFFFFFFFU
#define INT32_MAX 0x7FFFFFFF

#define get_member_offset(structname,membername) ((ulong)(&(((structname *)(0)) -> membername)))
#define bool_to_bit(v) (((v) != 0) ? 1 : 0)
#define __LIBRARY__
#include "unistd.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>

static char buf[1024];

struct dir_entry 
{
	uint16_t inode;
	char name[NAME_LEN];
};


static inline _syscall1(int64_t,exit,int,exit_code);
static inline _syscall3(int64_t,write,int,fd,const char *,buf,off_t,count);
static inline _syscall3(int64_t,read,int,fd,const char *,buf,off_t,count);
static inline _syscall3(int64_t,ioctl,int,fd,uint,cmd,ulong,arg);
static inline _syscall3(int64_t,open,const char *,filename,int,flag,int,mode);
static inline _syscall1(int64_t,close,int,fd);
static inline _syscall2(int64_t,stat,const char *,filename,struct stat *,stat_buf);
static inline _syscall0(int64_t,fork);
static inline _syscall3(int64_t,waitpid,pid_t,pid,uint *,stat_addr,int,options);
static inline _syscall3(int64_t,execve,const char *,file,char **,argv,char **,envp);

int main(int argc,char **argv,char **envp);

void _lock_acquire_recursive(_lock_t *lock)
{
	
}

void _lock_release_recursive(_lock_t *lock)
{
    
}

void _lock_init_recursive(_lock_t *lock)
{
    
}

void _lock_close_recursive(_lock_t *lock)
{
    
}

int pthread_setcancelstate(int __state, int *__oldstate)
{
    return 0;
}

void clear_buffer()
{
    usersyscall_ioctl(0,TCFLSH,0);
}

pid_t wait(int *wait_stat)
{
	return usersyscall_waitpid(-1,wait_stat,0);
}

void _start(int argc,char **argv,char **envp)
{
    extern ulong _heap_start;
    while(malloc(4096) < _heap_start);
    clear_buffer();
	usersyscall_exit(main(argc,argv,envp));
}

/*static int printf(const char *fmt, ...)
{
	va_list args;
	int i = 0;

	va_start(args,fmt);
	usersyscall_write(1,printbuf,i = vsprintf(printbuf,fmt,args));
	va_end(args);
	return i;
}*/

void dump_memory(void *mem,ulong length)
{
    printf("syslog_dump_memory:addr = 0x%08X%08X,length = 0x%08X%08X",((ulong)mem) >> 32,((ulong)mem) & 0xFFFFFFFFUL,length >> 32,length & 0xFFFFFFFFUL);
    printf("                   ");

    for(int i = 0;i < 16;i++)
    {
        printf("%02X ",i);
    }

    char *buf = (char *)(((ulong)mem) & (~0x0F));
    length += ((ulong)mem) - ((ulong)buf);
    
    for(int i = 0;i < length;i++)
    {
        if((i & 0x0F) == 0x00)
        {
            printf("\r\n");
            printf("0x%08X%08X ",(((ulong)buf) + i) >> 32,(((ulong)buf) + i) & 0xFFFFFFFFUL);
        }

        printf("%02X ",(ulong)buf[i]);
    }

    printf("\r\n");
}

char buf2[100];

int main(int argc,char **argv,char **envp)
{
    char ch[10];
    char ch2[10];
    int i,j;
    
    
    printf("the first user program for linux 0.11 RISCV Version by LiZhirui 2019.12.17\r\n");
    printf("argc = %d\r\n",argc);

    for(ulong i = 0;i < argc;i++)
    {
        printf("argv[%d] = %s\r\n",i,argv[i]);
    }

    for(ulong i = 0;envp[i] != NULL;i++)
    {
        printf("envp[%d] = %s\r\n",i,envp[i]);
    }

    //usersyscall_write(0,":",1);
    //printf("keyboard test:\r\n");
    
    while(1)
    {
        clear_buffer();
        usersyscall_write(0,">>",2);
        gets(buf);

        if((memcmp(buf,"ls",2) == 0) && (buf[2] == NULL || buf[2] == ' '))
        {
            long fd;
            char *ptr = buf + 2;
            int pathlen;
            
            while(*ptr == ' ')
            {
                ptr++;
            }
            
            if(*ptr == NULL)
            {
                buf2[0] = '/';
                pathlen = 1;
                fd = usersyscall_open("/",O_RDONLY,0);
            }
            else
            {
                printf("path is %s\r\n",ptr);
                strcpy(buf2,ptr);
                pathlen = strlen(ptr);

                if(ptr[pathlen - 1] != '/')
                {
                    buf2[pathlen++] = '/';
                }

                fd = usersyscall_open(ptr,O_RDONLY,0);
            }
            
            if(fd < 0)
            {
                printf("error:unknown path,errno = %d!\r\n",errno);
                continue;
            }

            long size = usersyscall_read(fd,buf,1024);

            if(size < 0)
            {
                printf("error:read path error,errno = %d!\r\n",errno);
                usersyscall_close(fd);
                continue;
            }

            for(long i = 0;i < size;i += sizeof(struct dir_entry))
            {
                struct dir_entry *p = buf + i;

                if(p -> inode != 0)
                {
                    struct stat s;
                    strcpy(buf2 + pathlen,p -> name);
                    usersyscall_stat(buf2,&s);
                    
                    if(S_ISREG(s.st_mode))
                    {
                        putchar('-');
                    }
                    else if(S_ISDIR(s.st_mode))
                    {
                        putchar('d');
                    }
                    else if(S_ISFIFO(s.st_mode))
                    {
                        putchar('p');
                    }
                    else if(S_ISBLK(s.st_mode))
                    {
                        putchar('b');
                    }
                    else if(S_ISCHR(s.st_mode))
                    {
                        putchar('c');
                    }
                    else
                    {
                        putchar('?');
                    }

                    putchar(s.st_mode & S_IRUSR ? 'r' : '-');
                    putchar(s.st_mode & S_IWUSR ? 'w' : '-');
                    putchar(s.st_mode & S_IXUSR ? 'x' : '-');
                    putchar(s.st_mode & S_IRGRP ? 'r' : '-');
                    putchar(s.st_mode & S_IWGRP ? 'w' : '-');
                    putchar(s.st_mode & S_IXGRP ? 'x' : '-');
                    putchar(s.st_mode & S_IROTH ? 'r' : '-');
                    putchar(s.st_mode & S_IWOTH ? 'w' : '-');
                    putchar(s.st_mode & S_IXOTH ? 'x' : '-');

                    printf("\t%s\r\n",p -> name);
                }
            }

            usersyscall_close(fd);
        }
        else if(strcmp(buf,"help") == 0)
        {
            printf("help:\r\n");
            printf("ls [path]\r\n");
        }
        else if(strcmp(buf,"exit") == 0)
        {
            return 0;
        }
        else
        {
            pid_t pid;
            int stat;
            printf("prepare to execute %s\r\n",buf);

            if(!(pid = usersyscall_fork()))
            {
                printf("prepare to execute %s\r\n",buf);
                usersyscall_execve(buf,NULL,NULL);
                printf("unknown command:%s,you can input \"help\"\r\n",buf);
                usersyscall_exit(-1);
            }
            else
            {
                //printf("waiting %s\r\n",buf);
                while(pid != wait(&stat));
            }
        }
    }
    
	return 0;
}