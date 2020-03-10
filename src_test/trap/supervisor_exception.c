#include "common.h"

uintptr_t supervisor_exception_handler(trap_exception cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32])
{
    char *errorstr = "";

    switch(cause)
    {
        case trap_exception_instruction_address_misaligned:
            errorstr = "instruction address misaligned";
            break;

        case trap_exception_instruction_access_fault:
            errorstr = "instruction access fault";
            break;

        case trap_exception_illegal_instruction:
            errorstr = "illegal instruction";
            break;

        case trap_exception_breakpoint:
            errorstr = "breakpoint";
            break;

        case trap_exception_load_address_misaligned:
            errorstr = "load address misaligned";
            break;

        case trap_exception_load_access_fault:
            errorstr = "load access fault";
            break;

        case trap_exception_store_or_amo_address_misaligned:
            errorstr = "store/amo address misaligned";
            break;

        case trap_exception_store_or_amo_access_fault:
            errorstr = "store/amo access fault";
            break;

        case trap_exception_environment_call_from_u_mode:
            errorstr = "environment call from u-mode";
            break;

        case trap_exception_environment_call_from_s_mode:
            errorstr = "environment call from s-mode";
            break;

        case trap_exception_environment_call_from_m_mode:
            errorstr = "environment call from m-mode";
            break;

        default:
            errorstr = "unknown exception";
            break;
    }

    syslog_debug("supervisor_exception_handler","exception %d,%s",cause,errorstr);

    if(cause >= trap_exception_environment_call_from_u_mode && cause <= trap_exception_environment_call_from_m_mode)
    {
        return epc;
    }

	while(1);
}

void supervisor_exception_delegate_enable(trap_exception_multitype value)
{
    csr_define_common csr = csr_read(csr_sedeleg);
    csr.value |= (ulong)value;
    csr_write(csr_sedeleg,csr);
}

void supervisor_exception_delegate_disable(trap_exception_multitype value)
{
    csr_define_common csr = csr_read(csr_sedeleg);
    csr.value &= ~((ulong)value);
    csr_write(csr_sedeleg,csr);
}

void supervisor_exception_delegate_enable_all()
{
    csr_define_common csr = csr_read(csr_sedeleg);
    csr.value = ~0UL;
    csr_write(csr_sedeleg,csr);
}

void supervisor_exception_delegate_disable_all()
{
    csr_define_common csr = csr_read(csr_sedeleg);
    csr.value = 0UL;
    csr_write(csr_sedeleg,csr);
}