#include "common.h"
#include <linux/mm.h>

void mem_init(ulong start_mem,ulong end_mem);

void timer_test_interrupt_handler(void *ctx)
{
    syslog_info("timer_test_interrupt_handler","");
}

void supervisor_main()
{
    mem_init(0x80000000UL,0x80600000UL);
    timer_init(TIMER_DEVICE_0);
    size_t t = timer_set_interval(TIMER_DEVICE_0,TIMER_CHANNEL_0,1000000000UL);
    timer_irq_register(TIMER_DEVICE_0,TIMER_CHANNEL_0,0,PLIC_NUM_PRIORITIES,timer_test_interrupt_handler,NULL);
    timer_set_enable(TIMER_DEVICE_0,TIMER_CHANNEL_0,1);
    
    syslog_info("timer_interval","%d",t);

    while(1)
    {
        volatile ulong aa = 0xABCDEF9012345678;
        //volatile ulong *bb = (volatile ulong *)(((ulong)aa) | 0x100000000UL);
        //syslog_debug("aa,bb","aa =  %d,bb = %d",aa,*bb);
        uint *page = get_free_page();
        syslog_info("apply new page","%p",page);
        *page = 12345;
        syslog_info("read page value","%d",*page);
        free_page(page);
        syslog_warning("hello","%d",1);
        syslog_error("hi","%d",2);
        syslog_info("mode:","%s",privilege_getstring(privilege_get_cur_level()));
        syslog_info("size","%d",sizeof(trap_interrupt));
        pmsleep(1000);
    }
}