#ifndef __SUPERVISOR_INTERRUPT_H__
#define __SUPERVISOR_INTERRUPT_H__

    uintptr_t supervisor_interrupt_handler(trap_interrupt cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32]);
    void supervisor_interrupt_delegate_enable(trap_interrupt_multitype value);
    void supervisor_interrupt_delegate_disable(trap_interrupt_multitype value);
    void supervisor_interrupt_delegate_enable_all();
    void supervisor_interrupt_delegate_disable_all();

#endif