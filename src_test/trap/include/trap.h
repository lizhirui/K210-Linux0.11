#ifndef __TRAP_H__
#define __TRAP_H__

    typedef enum trap_interrupt
    {
        trap_interrupt_xxx = 0xFFFFFFFFFFFFFFFFUL,
        trap_interrupt_user_software = 0,
        trap_interrupt_supervisor_software = 1,
        trap_interrupt_machine_software = 3,
        trap_interrupt_user_timer = 4,
        trap_interrupt_supervisor_timer = 5,
        trap_interrupt_machine_timer = 7,
        trap_interrupt_user_external = 8,
        trap_interrupt_supervisor_external = 9,
        trap_interrupt_machine_external = 11
    }trap_interrupt;

    typedef enum trap_exception
    {
        trap_exception_xxx = 0xFFFFFFFFFFFFFFFFUL,
        trap_exception_instruction_address_misaligned = 0,
        trap_exception_instruction_access_fault = 1,
        trap_exception_illegal_instruction = 2,
        trap_exception_breakpoint = 3,
        trap_exception_load_address_misaligned = 4,
        trap_exception_load_access_fault = 5,
        trap_exception_store_or_amo_address_misaligned = 6,
        trap_exception_store_or_amo_access_fault = 7,
        trap_exception_environment_call_from_u_mode = 8,
        trap_exception_environment_call_from_s_mode = 9,
        trap_exception_environment_call_from_m_mode = 11
    }trap_exception;

    typedef struct trap_info_t
    {
        uintptr_t epc;//read-only
        uintptr_t newepc;//default equal to epc
        uintptr_t *regs;//32 items
        uintptr_t *fregs;//32 items
        uint32_t int_num;
        uint32_t int_threshold;
    }trap_info_t;
    
    typedef ulong trap_interrupt_multitype;
    typedef ulong trap_exception_multitype;

    trap_interrupt_multitype trap_interrupt_generate_multitype(ulong num,...);
    trap_exception_multitype trap_exception_generate_multitype(ulong num,...);
    void trap_listregs();
    void trap_listuserstack();

    #define TRAP_BACKUPSTACKSIZE (70 * 8)
    #define TRAP_CUSTOMBACKUPSTACKSIZE (TRAP_BACKUPSTACKSIZE - (64 * 8))
    #define TRAP_CUSTOMBACKUPITEM (TRAP_CUSTOMBACKUPSTACKSIZE >> 3)

    #include "machine_exception.h"
	#include "machine_interrupt.h"
    #include "supervisor_exception.h"
    #include "supervisor_interrupt.h"

#endif