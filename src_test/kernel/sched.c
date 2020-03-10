#include "common.h"
#include "linux/sched.h"
#include "linux/kernel.h"
#include "linux/sys.h"
#include "signal.h"
#include "plic.h"

#define _S(nr) (1 << ((nr) - 1))
#define _BLOCKABLE (~(_S(SIGKILL) | _S(SIGSTOP)))

volatile pte_sv39 init_task_page_dir_table[PAGE_DIR_TABLE_NUM] ALIGN4K;

extern trap_info_t trap_info;
extern bool trap_from_interrupt;
extern void switch_to_tss(struct tss_struct *oldtss,struct tss_struct *newtss);
extern void stack_overflow_check();

void show_task(int nr,struct task_struct *p)
{
    int i,j = 4096 - sizeof(struct task_struct);

    printk("%d: pid = %d,state = %d,",nr,p -> pid,p -> state);
    i = 0;

    while((i < j) && (!((char *)(p + 1))[i]))
    {
        i++;
    }

    printk("%d(of %d) chars free in kernel stack\r\n",i,j);
}

void show_stat()
{
    int i;

    for(i = 0;i < NR_TASKS;i++)
    {
        if(task[i])
        {
            show_task(i,task[i]);
        }
    }
}

union task_union
{
    struct task_struct task;
    char stack[PAGE_SIZE + KERNEL_STACK_SIZE];
};

static union task_union init_task ALIGN4BYTE;

int64_t volatile jiffies = 0;
long startup_time = 0;
struct task_struct *current = &(init_task.task);
struct task_struct *task[NR_TASKS] = {&(init_task.task)};
ulong kernel_stack[NR_TASKS] = {((ulong)(&init_task.stack)) + PAGE_SIZE + KERNEL_STACK_SIZE};
ulong kernel_stackbottom[NR_TASKS] = {((ulong)(&init_task.stack))};
ulong cur_kernel_stackbottom = 0;
ulong cur_kernel_stack = 0;
ulong old_kernel_stack = 0;

long user_stack[PAGE_SIZE >> 2];//2 page

void set_page_dir(pte_sv39 *dir_table)
{
    page_dir_table = dir_table;
    pte_common_addr_to_ppn((volatile pte_64model *)&page_root_table[3],(ulong)dir_table);
    pte_common_enable_entry((volatile pte_64model *)&page_root_table[3]);
    pte_common_enable_user((volatile pte_64model *)&page_root_table[3]);
    pte_refresh_tlb();
}

void mepc_recover()
{
    csr_define_common csr;

    csr.mepc.value = current -> tss.epc;
    csr_write(csr_mepc,csr);
    syslog_debug("mepc_recover","current = %p,epc = %p",current,current -> tss.epc);
}

int timer_interrupt(void *ctx)
{
    do_timer();
    return 0;
}

void interrupt_recover()
{
    csr_define_common csr;

    if(trap_from_interrupt == true)
    {
        timer[TIMER_DEVICE_0] -> channel[TIMER_CHANNEL_0].eoi = readl(&timer[TIMER_DEVICE_0] -> channel[TIMER_CHANNEL_0].eoi);
        readl(&timer[TIMER_DEVICE_0] -> channel[TIMER_CHANNEL_0].eoi);
        plic -> targets.target[core_get_coreid()].claim_complete = trap_info.int_num;
        plic -> targets.target[core_get_coreid()].priority_threshold = trap_info.int_threshold;
    }

    csr = csr_read(csr_mie);
    csr.mie.meie = 1;
    csr_write(csr_mie,csr);
    csr = csr_read(csr_mstatus);
    csr.mstatus.mie = 0;
    csr.mstatus.mpie = 1;
    csr_write(csr_mstatus,csr);
}

void output_stack_usage(ulong sp)
{
    printk("kernel stack usage:usage : %d,old_kernel_stack : %p,trap source : %s,cpl : %d\r\n",sp,old_kernel_stack,(trap_from_interrupt ? "interrupt" : "exception"),privilege_get_previous_level());
}

void kernel_stack_overflow(ulong sp)
{
    printk("kernel_stack_overflow!usage : %d,old_kernel_stack : %p,trap source : %s,cpl : %d\r\n",sp,old_kernel_stack,(trap_from_interrupt ? "interrupt" : "exception"),privilege_get_previous_level());
    while(1);
}

bool switch_to(int taskid)
{
    struct tss_struct *oldtss = &(current -> tss);
    struct tss_struct *tss = &(task[taskid] -> tss);
    trap_info_t oldtrapinfo;
    syslog_debug("switch_to","taskid = %d",taskid);
    stack_overflow_check();

    if(current != task[taskid])
    {
        //syslog_print("switch to %d\r\n",taskid);
        //syslog_print("cur kernel stack1 = %p\r\n",cur_kernel_stack);
        //syslog_print("switch to new task %d\r\n",taskid);
        syslog_debug("switch_to","switch to taskid = %d",taskid);
        /*syslog_print("oldepc = %p\r\n",trap_info.newepc);
        syslog_print("tss -> epc = %p\r\n",tss -> epc);*/
        oldtrapinfo = trap_info;
        oldtss -> epc = trap_info.newepc;
        current = task[taskid];
        old_kernel_stack = cur_kernel_stack;
        cur_kernel_stack = kernel_stack[taskid];
        cur_kernel_stackbottom = kernel_stackbottom[taskid];
        set_page_dir(current -> page_dir_table);
        switch_to_tss(oldtss,tss);
        //syslog_print("cur kernel stack2 = %p\r\n",cur_kernel_stack);
        interrupt_recover();
        trap_info = oldtrapinfo;
        //trap_info.newepc = tss -> epc;
        //syslog_print("newepc = %p\r\n",trap_info.newepc);

        if(trap_info.newepc < USER_START_ADDR)
        {
            pte_common_enable_user((volatile pte_64model *)&page_root_table[2]);
        }
        else
        {
            pte_common_disable_user((volatile pte_64model *)&page_root_table[2]);
        }

        //set_page_dir(task[taskid] -> page_dir_table);
        syslog_flagn(200);
        /*memcpy(oldtss -> regs,trap_info.regs,sizeof(trap_info.regs[0]) * 32);
        memcpy(oldtss -> fregs,trap_info.fregs,sizeof(trap_info.fregs[0]) * 32);
        oldtss -> epc = trap_info.epc;
        current = task[taskid];
        memcpy(trap_info.regs,tss -> regs,sizeof(trap_info.regs[0]) * 32);
        memcpy(trap_info.fregs,tss -> fregs,sizeof(trap_info.fregs[0]) * 32);
        set_page_dir(task[taskid] -> page_dir_table);
        trap_info.newepc = tss -> epc;*/
        return false;
    }

    return true;
}

//'schedule()' is the scheduler function
//Task 0 is the 'idle' task,which gets called when no other tasks can run.It can not be killed,and it cannot sleep.
//The 'state' information in task[0] is never used.
bool schedule()
{
    int i,next,c;
    struct task_struct **p;
    csr_define_common mstatus,mie;

    mstatus = csr_read(csr_mstatus);
    mie = csr_read(csr_mie);
    sysctl_disable_irq();
    
    //check alarm,wake up any interruptible tasks that have got a signal
    for(p = &LAST_TASK;p > &FIRST_TASK;--p)
    {
        if(*p)
        {
            syslog_info("schedule","task %d checking...,task struct addr = %p",p - &FIRST_TASK,*p);

            if(((*p) -> alarm) && ((*p) -> alarm < jiffies))
            {
                (*p) -> signal |= (1 << (SIGALRM - 1));
                (*p) -> alarm = 0;
            }

            if((((*p) -> signal) & (~(_BLOCKABLE * (*p) -> blocked))) && ((*p) -> state == TASK_INTERRUPTIBLE))
            {
                (*p) -> state = TASK_RUNNING;
            }
        }
    }

    //this is the scheduler proper
    while(1)
    {
        c = -1;
        next = 0;
        i = NR_TASKS;
        p = &task[NR_TASKS];

        while(--i)
        {
            if(!*--p)
            {
                continue;
            }

            if(((*p) -> state == TASK_RUNNING) && ((*p) -> counter > c))
            {
                c = (*p) -> counter;
                next = i;
            }
        }

        if(c)
        {
            break;
        }

        for(p = &LAST_TASK;p > &FIRST_TASK;--p)
        {
            if(*p)
            {
                (*p) -> counter = ((*p) -> counter >> 1) + (*p) -> priority;
            }
        }
    }

    bool r = switch_to(next);
    csr_write(csr_mie,mie);
    csr_write(csr_mstatus,mstatus);
    return r;
}

int64_t sys_pause()
{
    current -> state = TASK_INTERRUPTIBLE;
    schedule();
    return 0;
}

void sleep_on(struct task_struct **p)
{
    struct task_struct *tmp;

    if(!p)
    {
        return;
    }

    if(current == &(init_task.task))
    {
        panic("task[0] trying to sleep");
    }

    tmp = *p;
    *p = current;
    current -> state = TASK_UNINTERRUPTIBLE;
    
    if(schedule())
    {
        if(tmp)
        {
            tmp -> state = TASK_RUNNING;
        }
    }
}

void interruptible_sleep_on(struct task_struct **p)
{
    struct task_struct *tmp;

    if(!p)
    {
        return;
    }

    if(current == &(init_task.task))
    {
        panic("task[0] trying to sleep");
    }

    tmp = *p;
    *p = current;

    repeat:
        current -> state = TASK_INTERRUPTIBLE;
        
        if(schedule())
        {
            if((*p) && (*p != current))
            {
                (**p).state = TASK_RUNNING;
                goto repeat;
            }
        }

    *p = tmp;

    if(tmp)
    {
        tmp -> state = TASK_RUNNING;
    }
}

void wake_up(struct task_struct **p)
{
    if(p && (*p))
    {
        (**p).state = TASK_RUNNING;
    }
}

#define TIME_REQUESTS 64

static struct timer_list
{
    int64_t jiffies;
    void (*fn)();
    struct timer_list *next;
}timer_list[TIME_REQUESTS],*next_timer = NULL;

void add_timer(int64_t jiffies,void (*fn)())
{
    struct timer_list *p;

    if(!fn)
    {
        return;
    }

    sysctl_disable_irq();

    if(jiffies <= 0)
    {
        fn();
    }
    else
    {
        for(p = timer_list;p < timer_list + TIME_REQUESTS;p++)
        {
            if(!p -> fn)
            {
                break;
            }
        }

        if(p >= timer_list + TIME_REQUESTS)
        {
            panic("No more time requests free");
        }

        p -> fn = fn;
        p -> jiffies = jiffies;
        p -> next = next_timer;
        next_timer = p;

        while(p -> next && (p -> next -> jiffies < p -> jiffies))
        {
            p -> jiffies -= p -> next -> jiffies;
            fn = p -> fn;
            p -> next -> fn = fn;
            jiffies = p -> jiffies;
            p -> jiffies = p -> next -> jiffies;
            p -> next -> jiffies = jiffies;
            p = p -> next;
        }

        jiffies = p -> jiffies;

        while(p -> next)
        {
            p -> next -> jiffies -= jiffies;
            p = p -> next;
        }
    }

    sysctl_enable_irq();
}

void do_timer()
{
    privilege_level cpl = privilege_get_previous_level();
    syslog_debug("do_timer","cpl = %d",cpl);

    if(cpl == privilege_level_user)
    {
        current -> utime++;
    }
    else
    {
        syslog_debug("do_timer","mepc = %p",csr_read(csr_mepc).mepc.value);
        current -> stime++;
    }

    if(next_timer)
    {
        next_timer -> jiffies--;

        while(next_timer && next_timer -> jiffies <= 0)
        {
            void (*fn)();
            fn = next_timer -> fn;
            next_timer -> fn = NULL;
            next_timer = next_timer -> next;
            fn();
        }
    }

    syslog_debug("do_timer","counter = %d",current -> counter);

    if((--(current -> counter)) > 0)
    {
        return;
    }

    current -> counter = 0;

    if(cpl != privilege_level_user)
    {
        syslog_debug("do_timer","privilege is not user");
        return;
    }

    schedule();
}


//set alarm time(second)
int64_t sys_alarm(long seconds)
{
    int old = current -> alarm;

    if(old)
    {
        old = (old - jiffies) / HZ;
    }

    current -> alarm = (seconds > 0) ? (jiffies + HZ * seconds) : 0;
    return old;
}

//get current process id
int64_t sys_getpid()
{
    return current -> pid;
}

//get parent process id
int64_t sys_getppid()
{
    return current -> father;
}

//get user id
int64_t sys_getuid()
{
    return current -> uid;
}

//get effective uid
int64_t sys_geteuid()
{
    return current -> euid;
}

//get group id
int64_t sys_getgid()
{
    return current -> gid;
}

//decrease cpu priority
int64_t sys_nice(int64_t increment)
{
    if((current -> priority) - increment > 0)
    {
        current -> priority -= increment;
    }

    return 0;
}

void sched_init()
{
    int i;

    for(i = 1;i < NR_TASKS;i++)
    {
        task[i] = NULL;
    }

    struct task_struct *task = &init_task.task;
    memset(&init_task.task,0,sizeof(init_task.task));
    task -> state = TASK_RUNNING;
    task -> counter = 15;
    task -> priority = 15;
    task -> father = -1;
    task -> page_dir_table = init_task_page_dir_table;
    pte_common_init(init_task_page_dir_table,PAGE_DIR_TABLE_NUM);
    task -> code_base = 0xC0000000;
    task -> data_base = task -> code_base;
    task -> code_limit = 640 * 1024;
    task -> data_limit = 640 * 1024;
    regs_backup(task -> tss.regs,task -> tss.fregs);
    task -> tss.regs[reg_sp] = ((ulong)init_task.stack) + PAGE_SIZE;
    task -> tty = -1;
    cur_kernel_stack = kernel_stack[0];

    timer_init(TIMER_DEVICE_0);
    timer_set_interval(TIMER_DEVICE_0,TIMER_CHANNEL_0,10000000UL);
    timer_irq_register(TIMER_DEVICE_0,TIMER_CHANNEL_0,0,PLIC_NUM_PRIORITIES,timer_interrupt,NULL);
    timer_set_enable(TIMER_DEVICE_0,TIMER_CHANNEL_0,1);
}