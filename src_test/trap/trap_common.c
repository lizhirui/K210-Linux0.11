#include "common.h"
#include "linux/mm.h"
#include "linux/sched.h"

trap_info_t trap_info;
bool trap_from_interrupt;
const char *reg_str[] = {"zero","ra","sp","gp","tp","t0","t1","t2","fp","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};

trap_interrupt_multitype trap_interrupt_generate_multitype(ulong num,...)
{
    va_list args;
    trap_interrupt_multitype r = 0;

    va_start(args,num);
    
    while(num--)
    {
        r |= 1 << ((ulong)va_arg(args,trap_interrupt));
    }

    va_end(args);
    return r;
}

trap_exception_multitype trap_exception_generate_multitype(ulong num,...)
{
    va_list args;
    trap_exception_multitype r = 0;

    va_start(args,num);
    
    while(num--)
    {
        r |= 1 << ((ulong)va_arg(args,trap_exception));
    }

    va_end(args);
    return r;
}

void trap_listregs()
{
    printk("regs[32] and fregs[32]:\r\n");

    for(int i = 0;i < 32;i++)
    {
        printk("regs[%s] = %p\r\n",reg_str[i],trap_info.regs[i]);
    }

    for(int i = 0;i < 32;i++)
    {
        printk("fregs[%d] = %p\r\n",i,trap_info.fregs[i]);
    }
}

extern ulong cur_kernel_stack;

void trap_listuserstack()
{
    printk("current kernel stack = %p\r\n",cur_kernel_stack);
    printk("userstack:sp = %p\r\n",trap_info.regs[reg_sp]);
    ulong sp = trap_info.regs[reg_sp];

    if(privilege_get_previous_level() != privilege_level_machine)
    {
        user_addr_to_kernel(&sp);
    }

    ulong up = 128;

    if(((sp | ((1 << 10) - 1)) - sp) < up)
    {
        up = ((sp | ((1 << 10) - 1)) - sp);
    }

    for(ulong i = 0;i < 128;i += 8)
    {
        printk("sp[%d] = %p\r\n",i,*((ulong *)(sp + i)));
    }

    printk("userstack output finish\r\n");
}