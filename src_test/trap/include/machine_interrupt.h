#ifndef __MACHINE_INTERRUPT_H__
#define __MACHINE_INTERRUPT_H__

    uintptr_t machine_interrupt_handler(trap_interrupt cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32],uintptr_t csregs[2]);
    void machine_interrupt_delegate_enable(trap_interrupt_multitype value);
    void machine_interrupt_delegate_disable(trap_interrupt_multitype value);
    void machine_interrupt_delegate_enable_all();
    void machine_interrupt_delegate_disable_all();

#endif