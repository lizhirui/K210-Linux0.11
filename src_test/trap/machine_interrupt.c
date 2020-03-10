#include "common.h"
#include "linux/sched.h"

extern trap_info_t trap_info;
extern bool trap_from_interrupt;

extern uintptr_t handle_irq_m_ext(uintptr_t cause,uintptr_t epc);
extern int timer_interrupt(void *ctx);
extern int machine_main();
extern void entry(int core_id);

uintptr_t machine_interrupt_handler(trap_interrupt cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32],uintptr_t csregs[2])
{
    csr_define_common *mstatus = (csr_define_common *)(&csregs[0]);
    csr_define_common *mie = (csr_define_common *)(&csregs[1]);
    trap_info_t old_trap_info = trap_info;
    bool old_trap_from_interrupt = trap_from_interrupt;

    syslog_info("machine_interrupt_handler","cause = %d,epc = %p",cause,epc);
    //printk("entry interrupt\r\n");
    trap_from_interrupt = true;
    trap_info.regs = regs;
    trap_info.fregs = fregs;
    trap_info.epc = epc;
    trap_info.newepc = epc;

	switch(cause)
    {
        case trap_interrupt_machine_external:
            handle_irq_m_ext((uintptr_t)cause,epc);
            //timer_interrupt(NULL);
            break;
    }
    
    syslog_info("machine_interrupt_handler","newepc = %p",trap_info.newepc);
    sysctl_disable_irq();
    ulong r = trap_info.newepc;
    trap_info = old_trap_info;
    trap_from_interrupt = old_trap_from_interrupt;

    /*if((trap_info.regs[reg_sp] < 0x80000000UL) || (trap_info.regs[reg_ra] < 0x80000000UL))
    {
        printk("user stack exception from interrupt:sp = %p,ra = %p\r\n",trap_info.regs[reg_sp],trap_info.regs[reg_ra]);
    }

    if((((r < ((ulong)entry)) || (r > ((ulong)machine_main)))) && (privilege_get_previous_level() == privilege_level_user))
    {
        printk("interrupt:out of range epc:%p\r\n",trap_info.newepc);
    }*/

    if(r < 0x80000000UL)
    {
        printk("interrupt:unknown epc:%p",trap_info.newepc);
        while(1);
    }
    
    //if((trap_info.regs[1] == 0x80004d46) || (trap_info.regs[6] == 0x1800) || (trap_info.regs[16] == 0x177C))
    /*if(r == 0x80004d88)
    {
        //ulong ra = 0;
        //asm volatile("mv %0,ra" : "=r"(ra));
        printk("i:epc=%p,r=%p\r\n",trap_info.epc,r);
        trap_listregs();
    }*/
    
    //printk("leave interrupt,cpl = %d\r\n",privilege_get_previous_level());
    return r;
}

void machine_interrupt_delegate_enable(trap_interrupt_multitype value)
{
    csr_define_common csr = csr_read(csr_mideleg);
    csr.value |= (ulong)value;
    csr_write(csr_mideleg,csr);
}

void machine_interrupt_delegate_disable(trap_interrupt_multitype value)
{
    csr_define_common csr = csr_read(csr_mideleg);
    csr.value &= ~((ulong)value);
    csr_write(csr_mideleg,csr);
}

void machine_interrupt_delegate_enable_all()
{
    csr_define_common csr = csr_read(csr_mideleg);
    csr.value = ~0UL;
    csr_write(csr_mideleg,csr);
}

void machine_interrupt_delegate_disable_all()
{
    csr_define_common csr = csr_read(csr_mideleg);
    csr.value = 0UL;
    csr_write(csr_mideleg,csr);
}