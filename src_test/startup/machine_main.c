#include "common.h"
#define __LIBRARY__
#include "unistd.h"
#include "time.h"
#include "linux/tty.h"
#include "linux/sched.h"
#include "stddef.h"
#include "stdarg.h"
#include "linux/fs.h"
#include <fcntl.h>

static char printbuf[1024];

static inline _syscall0(int64_t,fork);
static inline _syscall0(int64_t,setup);
static inline _syscall0(int64_t,pause);
//static inline _syscall1(int64_t,write,char *,buf);
static inline _syscall3(int64_t,write,int,fd,const char *,buf,off_t,count);

static inline _syscall3(int64_t,open,const char *,filename,int,flag,int,mode);
static inline _syscall1(int64_t,dup,unsigned int,fildes);
static inline _syscall1(int64_t,exit,int,exit_code);
static inline _syscall3(int64_t,execve,const char *,file,char **,argv,char **,envp);
static inline _syscall3(int64_t,waitpid,pid_t,pid,uint *,stat_addr,int,options);
static inline _syscall0(int64_t,sync);
static inline _syscall0(pid_t,setsid);
static inline _syscall1(int64_t,close,int,fd);
static inline _syscall1(int64_t,debug,ulong,p);

void supervisor_main();
void mem_init(ulong start_mem,ulong end_mem);
extern void blk_dev_init();
extern void chr_dev_init();
extern void rd_init(ulong mem_start,ulong length);
extern void rd_load();
extern int64_t kernel_mktime(struct tm *tm);

extern ulong _buffer_start;
extern ulong _buffer_end;

void machine_timer_test_interrupt_handler(void *ctx)
{
    syslog_info("machine_timer_test_interrupt_handler","");
}

void machine_test()
{
    csr_define_common csr;
    timer_init(TIMER_DEVICE_0);
    size_t t = timer_set_interval(TIMER_DEVICE_0,TIMER_CHANNEL_0,1000000000UL);
    timer_irq_register(TIMER_DEVICE_0,TIMER_CHANNEL_0,0,PLIC_NUM_PRIORITIES,machine_timer_test_interrupt_handler,NULL);
    timer_set_enable(TIMER_DEVICE_0,TIMER_CHANNEL_0,1);
    csr = csr_read(csr_sstatus);
    csr.sstatus.sie = 0;
    csr.sstatus.spie = 0;
    csr_write(csr_sstatus,csr);

    while(1)
    {
        syslog_info("here is machine loop","");
        pmsleep(1000);
    }
}

static void time_init()
{
    struct tm time;

    time.tm_year = 2019;
    time.tm_mon = 11;
    time.tm_mday = 20;
    time.tm_hour = 22;
    time.tm_min = 51;
    time.tm_sec = 12;
    startup_time = kernel_mktime(&time);
}

/*void init3()
{
    while(1)
    {
        usersyscall_write("init3\r\n");
        usersyscall_write("hhhhhh\r\n");
    }
}

void init2()
{
    if(!usersyscall_fork())
    {
        init3();
    }

    while(1)
    {
        usersyscall_write("init2\r\n");
        usersyscall_write("hello\r\n");
    }
}

void init()
{
    int pid,i;

    usersyscall_setup();
    
}*/

static int printf(const char *fmt, ...)
{
	va_list args;
	int i = 0;

	va_start(args,fmt);
	usersyscall_write(1,printbuf,i = vsprintf(printbuf,fmt,args));
	va_end(args);
	return i;
}

static char * argv_rc[] = { "/bin/sh", NULL };
static char * envp_rc[] = { "HOME=/", NULL };

static char * argv[] = { "-/bin/sh",NULL };
static char * envp[] = { "HOME=/usr/root", NULL };

pid_t wait(int *wait_stat)
{
	return usersyscall_waitpid(-1,wait_stat,0);
}

void init()
{
    int pid,i;
    
    usersyscall_setup();
    usersyscall_setsid();
    usersyscall_open("/dev/tty0",O_RDWR,0);
    usersyscall_dup(0); 
    usersyscall_dup(0);  
    printf("%d buffers = %d bytes buffer space\r\n",NR_BUFFERS,NR_BUFFERS * BLOCK_SIZE);
    printf("Free mem: %d bytes\r\n",0x600000);

    if(!(pid = usersyscall_fork()))
    {
        usersyscall_close(0);

        if(usersyscall_open("/etc/rc",O_RDONLY,0))
        {
            usersyscall_exit(1); 
        }
        
        usersyscall_execve("/bin/sh",argv_rc,envp_rc);
        usersyscall_exit(2);
    }

    if(pid > 0)
    {
        while(pid != wait(&i));
    }

    while(1)
    {
        if((pid = usersyscall_fork()) < 0)
        {
            printf("Fork failed in init\r\n");
            continue;
        }

        if(!pid)
        {
            usersyscall_close(0);
            usersyscall_close(1);
            usersyscall_close(2);
            usersyscall_setsid();
            usersyscall_open("/dev/tty0",O_RDWR,0);
            usersyscall_dup(0);
            usersyscall_dup(0);
            ulong x = usersyscall_execve("/bin/sh",argv,envp);
            usersyscall_debug(errno);
            usersyscall_exit(x);
        }
        
        while(1)
        {
            if(pid == wait(&i))
            {
                break;
            }
        }

        printf("\r\nchild %d died with code %04x\r\n",pid,i);
        usersyscall_sync();
        while(1);
    }

    usersyscall_exit(0);
}

void user_main()
{   
    if(!usersyscall_fork())
    {
        init();
    }

    /*for(ulong i = 0;i < 8192;i += 8)
    {
        *((volatile ulong *)(0xC0000000)) = *((volatile ulong *)(0xC0000000 + i));
        *((volatile ulong *)(0xC0000000 + i)) = 15;
    }*/

    //while(1);
    
    while(1)
    {
        usersyscall_pause();
    }
}

int64_t sys_setup()
{
    syslog_print("here!\r\n");
    rd_load();
    syslog_print("rd_load ok\r\n");
    mount_root();
    syslog_print("mount_root ok\r\n");
    return 0;
}

int machine_main()
{
    uint32_t freq = otp_read_serial();
    ulong xx = 0x90ABCDEF12345678;
    csr_define_common csr;
    ulong i;
    //csr = csr_asm_read(0x000);
    
    /*while(1)
    {
        syslog_dump_memory(&xx,8);
        syslog_info("main","page table address:0x%08X",page_root_table);
        syslog_info("main","2 of page table address:0x%08X",page_root_table + 2);
    }*/

    pte_common_init((volatile pte_64model *)page_root_table,PAGE_ROOT_TABLE_NUM);

    //Page root table init
    /*pte_common_addr_to_ppn((volatile pte_64model *)&page_root_table[0],0x00000000UL);
    pte_common_set_accessibility((volatile pte_64model *)&page_root_table[0],pte_accessibility_all);
    pte_common_enable_entry((volatile pte_64model *)&page_root_table[0]);
    pte_common_enable_user((volatile pte_64model *)&page_root_table[0]);
    pte_common_addr_to_ppn((volatile pte_64model *)&page_root_table[1],0x40000000UL);
    pte_common_set_accessibility((volatile pte_64model *)&page_root_table[1],pte_accessibility_all);
    pte_common_enable_entry((volatile pte_64model *)&page_root_table[1]);
    pte_common_enable_user((volatile pte_64model *)&page_root_table[1]);*/
    pte_common_addr_to_ppn((volatile pte_64model *)&page_root_table[2],0x80000000UL);
    pte_common_set_accessibility((volatile pte_64model *)&page_root_table[2],pte_accessibility_all);
    pte_common_enable_entry((volatile pte_64model *)&page_root_table[2]);
    pte_common_enable_user((volatile pte_64model *)&page_root_table[2]);

    for(i = 3;i < PAGE_ROOT_TABLE_NUM;i++)
    {
        pte_common_set_accessibility((volatile pte_64model *)&page_root_table[i],pte_accessibility_pointer);
        pte_common_enable_entry((volatile pte_64model *)&page_root_table[i]);
    }

    //Page dir table init
    //pte_common_init((volatile pte_64model *)page_dir_table,PAGE_DIR_TABLE_NUM);

    /*for(i = 0;i < PAGE_TABLE_NUM;i++)
    {
        pte_common_init((volatile pte_64model *)page_table[i],PAGE_TABLE_ITEM_NUM);
        pte_common_addr_to_ppn((volatile pte_64model *)&page_dir_table[i],(ulong)page_table[i]);
        pte_common_set_accessibility((volatile pte_64model *)&page_dir_table[i],pte_accessibility_pointer);
        pte_common_enable_entry((volatile pte_64model *)&page_dir_table[i]);
    }*/

    pte_common_addr_to_ppn(&page_root_table[3],page_dir_table);
    pte_common_enable_user(&page_root_table[3]);

    pte_set_root_page_table((volatile pte_64model *)page_root_table);
    pte_entry_sv39();
    syslog_dump_memory((void *)page_root_table,sizeof(page_root_table));
    //syslog_dump_otp(0x0000,16384);
    /*csr = csr_read(csr_medeleg);
    csr.medeleg.load_access_fault = 1;
    csr_write(csr_medeleg,csr);*/
    /*csr = csr_read(csr_mstatus);
    csr.mstatus.sie = 1;
    csr_write(csr_mstatus,csr);
    csr = csr_read(csr_sstatus);
    csr.sstatus.pum = 0;//enable supervisor access user page
    csr.sstatus.sie = 1;
    csr_write(csr_sstatus,csr);*/
    //machine_exception_delegate_enable_all();
    //machine_exception_delegate_disable(trap_exception_generate_multitype(2,trap_exception_environment_call_from_m_mode,trap_exception_environment_call_from_s_mode));
    //machine_interrupt_delegate_enable_all();
    sysctl_disable_irq();
    //syslog_debug("machine_main","mideleg = %p",csr_read(csr_mideleg));
    //privilege_to_supervisor((privilege_targetfunc)supervisor_main);
    
    /*csr = csr_read(csr_mstatus);
    csr.mstatus.mie = 0;
    csr.mstatus.mpie = 0;
    csr.mstatus.sie = 0;
    csr.mstatus.spie = 0;
    //csr.mstatus.mpp = 1;
    csr_write(csr_mstatus,csr);
    csr = csr_read(csr_sstatus);
    csr.sstatus.sie = 0;
    csr.sstatus.spie = 0;
    csr_write(csr_sstatus,csr);*/
    /*csr = csr_read(csr_mepc);
    csr.mepc.value = machine_test;
    csr_write(csr_mepc,csr);
    asm volatile("mret");*/
    //privilege_to_supervisor((privilege_targetfunc)machine_test);

    /*while(1)
    {
        syslog_info("here is machine loop","");
        msleep(1000);
    }*/
    syslog_print("sizeof(struct task_struct) = %d\r\n",sizeof(struct task_struct));
    ROOT_DEV = 0x0101;
    syslog_print("buffer_start = %p,buffer_end = %p\r\n",&_buffer_start,&_buffer_end);
    rd_init(0x8007D000UL,0x5A000UL);
    syslog_print("rd_init ok\r\n");
    mem_init(0x80100000UL,0x80600000UL);
    syslog_print("mem_init ok\r\n");
    blk_dev_init();
    syslog_print("blk_dev_init ok\r\n");
    chr_dev_init();
    syslog_print("chr_dev_init ok\r\n");
    tty_init();
    syslog_print("tty_init ok\r\n");
    time_init();
    syslog_print("time_init ok\r\n");
    sched_init();
    syslog_print("sched_init ok\r\n");
    buffer_init(&_buffer_end);
    syslog_print("buffer_init ok\r\n");

    /*page_dir_table = get_free_page();
    pte_common_addr_to_ppn((volatile pte_64model *)&page_root_table[3],(ulong)page_dir_table);
    pte_common_enable_entry((volatile pte_64model *)&page_root_table[3]);
    pte_common_enable_user((volatile pte_64model *)&page_root_table[3]);
    pte_refresh_tlb();*/

    sysctl_enable_irq();
    
    privilege_to_user(user_main);

    /*while(1)
    {
        volatile ulong aa = 0xABCDEF9012345678;
        /*volatile ulong *bb = (volatile ulong *)(((ulong)aa) | 0x100000000UL);
        syslog_debug("aa,bb","aa =  %d,bb = %d",aa,*bb);*/
        /*uint *page = get_free_page();
        syslog_info("apply new page","%p",page);
        *page = 12345;
        syslog_info("read page value","%d",*page);
        free_page(page);
        syslog_warning("hello","%d",1);
        syslog_error("hi","%d",2);
        syslog_info("mode:","%s",privilege_getstring(privilege_get_cur_level()));
        syslog_info("size","%d",sizeof(trap_interrupt));
        msleep(1000);
    }*/

    return 0;
}