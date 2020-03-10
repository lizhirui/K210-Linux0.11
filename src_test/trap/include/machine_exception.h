#ifndef __MACHINE_EXCEPTION_H__
#define __MACHINE_EXCEPTION_H__

	uintptr_t machine_exception_handler(trap_exception cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32],uintptr_t csregs[2]);
    void machine_exception_delegate_enable(trap_exception_multitype value);
    void machine_exception_delegate_disable(trap_exception_multitype value);
    void machine_exception_delegate_enable_all();
    void machine_exception_delegate_disable_all();

#endif