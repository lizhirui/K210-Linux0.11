#include "common.h"

handle_irq_s_ext(uintptr_t cause,uintptr_t epc);

uintptr_t supervisor_interrupt_handler(trap_interrupt cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32])
{
    syslog_info("supervisor_interrupt_handler","cause = %d",cause);


	switch(cause)
    {
        case trap_interrupt_machine_external:
            return handle_irq_s_ext((uintptr_t)cause,epc);
    }

    return epc;
}

void supervisor_interrupt_delegate_enable(trap_interrupt_multitype value)
{
    csr_define_common csr = csr_read(csr_sideleg);
    csr.value |= (ulong)value;
    csr_write(csr_sideleg,csr);
}

void supervisor_interrupt_delegate_disable(trap_interrupt_multitype value)
{
    csr_define_common csr = csr_read(csr_sideleg);
    csr.value &= ~((ulong)value);
    csr_write(csr_sideleg,csr);
}

void supervisor_interrupt_delegate_enable_all()
{
    csr_define_common csr = csr_read(csr_sideleg);
    csr.value = ~0UL;
    csr_write(csr_sideleg,csr);
}

void supervisor_interrupt_delegate_disable_all()
{
    csr_define_common csr = csr_read(csr_sideleg);
    csr.value = 0UL;
    csr_write(csr_sideleg,csr);
}