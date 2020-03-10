#include "common.h"
#include "linux/sched.h"
#include "linux/kernel.h"
#include "asm/segment.h"
#include "signal.h"

volatile void do_exit(int error_code);
extern trap_info_t trap_info;

//signal get mask
int sys_sgetmask()
{
    return current -> blocked;
}

//signal set mask
int sys_ssetmask(int newmask)
{
    int old = current -> blocked;

    current -> blocked = newmask & ~(1 << (SIGKILL - 1));
    return old;
}

//copy sigaction from "from"(kernel) to "to"(user)
static inline void save_old(char *from,char *to)
{
    int i;

    verify_area(to,sizeof(struct sigaction));
    //user_ptr_to_kernel((void **)&to);
    //memcpy((void *)to,(const void *)from,sizeof(struct sigaction));
    mem_copy_from_kernel((ulong)from,(ulong)to,sizeof(struct sigaction));
}

//copy sigaction from "from"(user) to "to"(kernel)
static inline void get_new(char *from,char *to)
{
    //user_ptr_to_kernel((void **)&from);
    //memcpy((void *)to,(const void *)from,sizeof(struct sigaction));
    mem_copy_to_kernel((ulong)from,(ulong)to,sizeof(struct sigaction));
}

int64_t sys_signal(int signum,int64_t handler,int64_t restorer)
{
    struct sigaction tmp;

    if((signum < 1) || (signum > 32) || (signum == SIGKILL))
    {
        return -1;
    }

    tmp.sa_handler = (void (*)(int))handler;
    tmp.sa_mask = 0;
    tmp.sa_flags = SA_ONESHOT | SA_NOMASK;
    tmp.sa_restorer = (void (*)(void))restorer;
    handler = (int64_t)current -> sigaction[signum - 1].sa_handler;
    current -> sigaction[signum - 1] = tmp;
    return handler;
}

int64_t sys_sigaction(int signum,const struct sigaction *action,struct sigaction *oldaction)
{
    struct sigaction tmp;

    if((signum < 1) || (signum > 32) || (signum == SIGKILL))
    {
        return -1;
    }

    tmp = current -> sigaction[signum - 1];
    get_new((char *)action,(char *)(signum - 1 + current -> sigaction));

    if(oldaction)
    {
        save_old((char *)&tmp,(char *)oldaction);
    }

    if(current -> sigaction[signum - 1].sa_flags & SA_NOMASK)
    {
        current -> sigaction[signum - 1].sa_mask = 0;
    }
    else
    {
        current -> sigaction[signum - 1].sa_mask |= (1 << (signum - 1));
    }

    return 0;
}

void do_signal(int64_t signr)
{
    ulong sa_handler;
    struct sigaction *sa = current -> sigaction + signr - 1;
    sa_handler = (ulong)sa -> sa_handler;

    if(sa_handler == ((ulong)SIG_IGN))
    {
        return;
    }

    if(!sa_handler)
    {
        if(signr == SIGCHLD)
        {
            return;
        }
        else
        {
            do_exit(1 << (signr - 1));
        }
    }

    if(sa -> sa_flags & SA_ONESHOT)
    {
        sa -> sa_handler = NULL;
    }

    ulong *stack = trap_info.regs[reg_sp] - TRAP_CUSTOMBACKUPSTACKSIZE;
    ulong longs = 66 << 3;
    verify_area(stack,longs);
    /*user_ptr_to_kernel((void **)&stack);
    memcpy(stack,trap_info.regs,32 * sizeof(trap_info.regs[0]));
    memcpy(stack + 32,trap_info.fregs,32 * sizeof(trap_info.fregs[0]));
    stack[64] = trap_info.epc;*/
    mem_copy_from_kernel((ulong)&trap_info.regs,(ulong)stack,32 * sizeof(trap_info.regs[0]));
    mem_copy_from_kernel((ulong)&trap_info.fregs,(ulong)(stack + 32),32 * sizeof(trap_info.fregs[0]));
    mem_copy_from_kernel((ulong)&trap_info.epc,(ulong)(stack + 64),sizeof(trap_info.epc));

    if(!(sa -> sa_flags & SA_NOMASK))
    {
        mem_copy_from_kernel((ulong)&(current -> blocked),(ulong)(stack + 65),sizeof(current -> blocked));
    }

    trap_info.regs[reg_ra] = sa -> sa_restorer;
    trap_info.regs[reg_sp] -= longs;
    trap_info.regs[reg_a0] = signr;
    trap_info.newepc = sa_handler;
    current -> blocked |= sa -> sa_mask;
}