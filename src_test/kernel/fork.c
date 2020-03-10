#include "common.h"
#include "errno.h"
#include "linux/sched.h"
#include "linux/kernel.h"
#include "asm/segment.h"

extern void write_verify(ulong address);
extern void fork_process_exit();

extern trap_info_t trap_info;

int last_pid = 0;

void verify_area(void *addr,int size)
{
    ulong start;
    
    start = (ulong)addr;
    size += start & (PAGE_SIZE - 1);
    start &= ~(PAGE_SIZE - 1);

    while(size > 0)
    {
        size -= 4096;
        write_verify(start);
        start += 4096;
    }
}

int copy_mem(int nr,struct task_struct *p)
{
    ulong old_code_base,old_data_base;
    ulong new_code_base,new_data_base;
    ulong code_limit,data_limit;

    old_code_base = p -> code_base;
    old_data_base = p -> data_base;
    code_limit = p -> code_limit;
    data_limit = p -> data_limit;
    new_data_base = new_code_base = USER_START_ADDR;
    p -> start_code = new_code_base;
    //syslog_print("data_limit = %d\r\n",data_limit);

    if(copy_page_tables(old_data_base,new_data_base,p -> page_dir_table,data_limit / PAGE_SIZE))
    {
        free_page_tables(new_data_base,p -> page_dir_table,data_limit);
        return -ENOMEM;
    }

    return 0;
}

int copy_process(int nr)
{
    struct task_struct *p;
    int i;
    struct file *f;
    p = (struct task_struct *)get_free_pages(5);

    if(!p)
    {
        return -EAGAIN;
    }

    syslog_debug("copy_process","task %d seted %p",nr,p);
    task[nr] = p;
    *p = *current;//this doesn't copy the supervisor stack
    p -> state = TASK_UNINTERRUPTIBLE;
    p -> pid = last_pid;
    p -> father = current -> pid;
    p -> counter = p -> priority;
    p -> signal = 0;
    p -> alarm = 0;
    p -> leader = 0;//process leadership doesn't inherit
    p -> utime = p -> stime = 0;
    p -> cutime = p -> cstime = 0;
    p -> start_time = jiffies;
    kernel_stackbottom[nr] = kernel_stack[nr] = get_free_pages(KERNEL_STACK_SIZE >> PAGING_SHIFT);

    if(!kernel_stack[nr])
    {
        free_pages(p,5);
        return -EAGAIN;
    }

    kernel_stack[nr] += KERNEL_STACK_SIZE;
    p -> tss.regs[reg_sp] = kernel_stack[nr] - sizeof(trap_info.regs[0]) * 32 - sizeof(trap_info.fregs[0]) * 32;
    
    /*memcpy(p -> tss.regs,trap_info.regs,sizeof(trap_info.regs[0]) * 32);
    memcpy(p -> tss.fregs,trap_info.fregs,sizeof(trap_info.fregs[0]) * 32);*/
    memcpy((void *)(p -> tss.regs[reg_sp]),trap_info.regs,sizeof(trap_info.regs[0]) * 32);
    memcpy((void *)(p -> tss.regs[reg_sp] + sizeof(trap_info.regs[0]) * 32),trap_info.fregs,sizeof(trap_info.fregs[0]) * 32);
    p -> tss.epc = trap_info.epc + 4;
    //p -> tss.regs[reg_a0] = 0;
    syslog_debug("copy_process","sp = %p,epc = %p,p = %p",p -> tss.regs[reg_sp],p -> tss.epc,p);
    *(((ulong *)(p -> tss.regs[reg_sp])) + reg_a0) = 0;
    *(((ulong *)(p -> tss.regs[reg_sp])) + reg_ra) = fork_process_exit;
    *((ulong *)(p -> tss.regs[reg_sp] - 8)) = trap_info.regs[reg_ra];
    p -> page_dir_table = ((ulong)p) + PAGE_SIZE;
    //syslog_print("task %d,p -> page_dir_table = %p\r\n",nr,p -> page_dir_table);
    pte_common_init(p -> page_dir_table,PAGE_DIR_TABLE_NUM);
    
    syslog_debug("copy_process","4");
    
    if(copy_mem(nr,p))
    {
        syslog_debug("copy_process","5");
        task[nr] = NULL;
        free_pages(kernel_stack[nr] - KERNEL_STACK_SIZE,KERNEL_STACK_SIZE >> PAGING_SHIFT);
        free_pages((ulong)p,5);
        return -EAGAIN;
    }
    
    for(i = 0;i < NR_OPEN;i++)
    {
        if(f = p -> filp[i])
        {
            f -> f_count++;
        }
    }
    
    if(current -> pwd)
    {
        current -> pwd -> i_count++;
    }

    if(current -> root)
    {
        current -> root -> i_count++;
    }

    if(current -> executable)
    {
        current -> executable -> i_count++;
    }

    p -> state = TASK_RUNNING;
    return last_pid;
}

int find_empty_process()
{
    int i;

    repeat:
        if((++last_pid) < 0)
        {
            last_pid = 1;
        }

        for(i = 0;i < NR_TASKS;i++)
        {
            if(task[i] && (task[i] -> pid == last_pid))
            {
                goto repeat;
            }
        }

    for(i = 1;i < NR_TASKS;i++)
    {
        if(!task[i])
        {
            return i;
        }
    }

    return -EAGAIN;
}