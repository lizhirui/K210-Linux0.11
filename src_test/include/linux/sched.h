#ifndef __SCHED_H__
#define __SCHED_H__

    #include "common.h"
    #include "linux/fs.h"
    #include "linux/mm.h"
    #include "signal.h"

    #define NR_TASKS 64
    #define HZ 100
    
    #define FIRST_TASK task[0]
    #define LAST_TASK task[NR_TASKS - 1]

    #if (NR_OPEN > 32)
        #error "Currently the close-on-exec-flags are in one word, max 32 files/proc"
    #endif

    #define TASK_RUNNING 0
    #define TASK_INTERRUPTIBLE 1
    #define TASK_UNINTERRUPTIBLE 2
    #define TASK_ZOMBIE 3
    #define TASK_STOPPED 4

    #define KERNEL_STACK_SIZE 32768

    extern int copy_page_tables(ulong from,ulong to,volatile pte_sv39 *to_dir,ulong size);
    extern int free_page_tables(ulong from,volatile pte_sv39 *dir,ulong size);

    extern void sched_init();
    extern bool schedule();
    extern void panic(const char *str);
    typedef int64_t (*fn_ptr)();

    struct tss_struct
    {
        ulong regs[32];
        ulong fregs[32];
        ulong epc;
    };

    struct task_struct
    {
        int64_t state;//-1 unrunnable 0 runnable >0 stopped
        int64_t counter;
        int64_t priority;
        int64_t signal;
        struct sigaction sigaction[32];
        int64_t blocked;//bitmap of masked signals
        int exit_code;
        ulong start_code,end_code,end_data,brk,start_stack;
        int64_t pid,father,pgrp,session,leader;
        uint16_t uid,euid,suid;
        uint16_t gid,egid,sgid;
        int64_t alarm;
        int64_t utime,stime,cutime,cstime,start_time;
        //file system info
        int tty;//-1 if no tty,so it must be signed
        uint16_t umask;
        struct m_inode *pwd;
        struct m_inode *root;
        struct m_inode *executable;
        ulong close_on_exec;
        struct file *filp[NR_OPEN];
        volatile pte_sv39 *page_dir_table;
        ulong code_base;
        ulong data_base;
        ulong code_limit;
        ulong data_limit;
        struct tss_struct tss;
    };

    extern struct task_struct *task[NR_TASKS];
    extern struct task_struct *current;
    extern ulong kernel_stack[NR_TASKS];
    extern ulong kernel_stackbottom[NR_TASKS];
    extern int64_t volatile jiffies;
    extern int64_t startup_time;

    #define CURRENT_TIME (startup_time + jiffies / HZ)

    extern void add_timer(long jiffies,void (*fn)());
    extern void sleep_on(struct task_struct **p);
    extern void interruptible_sleep_on(struct task_struct **p);
    extern void wake_up(struct task_struct **p);

    #define PAGE_ALIGN(n) (((n) + 0xFFF) & 0xFFFFF000)

#endif