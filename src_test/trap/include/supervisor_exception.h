#ifndef __SUPERVISOR_EXCEPTION_H__
#define __SUPERVISOR_EXCEPTION_H__

	uintptr_t supervisor_exception_handler(trap_exception cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32]);
    void supervisor_exception_delegate_enable(trap_exception_multitype value);
    void supervisor_exception_delegate_disable(trap_exception_multitype value);
    void supervisor_exception_delegate_enable_all();
    void supervisor_exception_delegate_disable_all();

#endif
