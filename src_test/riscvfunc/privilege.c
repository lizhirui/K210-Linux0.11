#include "common.h"

//static privilege_level privilege_curvalue = privilege_level_machine;

const char *privilege_getstring(privilege_level level)
{
    switch(level)
    {
        case privilege_level_machine:
            return "machine";

        case privilege_level_supervisor:
            return "supervisor";

        case privilege_level_user:
            return "user";

        default:
            return "illegal";
    }
}

void privilege_init(privilege_level cur_privilege_level)
{
    core_get_info() -> privilege_level = cur_privilege_level;
    syslog_info("privilege_init","current privilege level is initialized to %s",privilege_getstring(cur_privilege_level));

    if(core_get_info() -> privilege_level != cur_privilege_level)
    {
        syslog_error("error","%d",core_get_info());
    }
}

void privilege_trap_entry(privilege_level cur_privilege_level,ulong cause,ulong epc,ulong badaddr)
{
    core_info_t *core_info = core_get_info();

    syslog_info("privilege_trap_entry","old privilege level is %s,new privilege level is %s,cause = %p,epc = %p,badaddr = %p",privilege_getstring(core_info -> privilege_level),privilege_getstring(cur_privilege_level),cause,epc,badaddr);
    core_info -> privilege_level = cur_privilege_level;
}

privilege_level privilege_get_previous_level()
{
    core_info_t *core_info = core_get_info();

    switch(core_info -> privilege_level)
    {
        case privilege_level_machine:
            return csr_read(csr_mstatus).mstatus.mpp;

        case privilege_level_supervisor:
            return csr_read(csr_sstatus).sstatus.spp;

        default:
            return privilege_level_user;
    }
}

ulong privilege_trap_exit(ulong epc)
{
    core_info_t *core_info = core_get_info();
    syslog_info("privilege_trap_exit","old privilege level is %s,new privilege level is %s,mepc = %p",privilege_getstring(core_info -> privilege_level),privilege_getstring(privilege_get_previous_level()),csr_read(csr_mepc).value);
    core_info -> privilege_level = privilege_get_previous_level();
    return epc;
}
    
void privilege_to_supervisor(privilege_targetfunc targetfunc)
{
    core_info_t *core_info = core_get_info();
    csr_define_common csrv_mstatus,csrv_mepc;

    if(core_info -> privilege_level != privilege_level_machine)
    {
        syslog_error("privilege_to_supervisor","current privilege level is %s,try to transfer to supervisor",privilege_getstring(core_info -> privilege_level));
        return;
    }

    syslog_info("privilege_to_supervisor","old privilege level is %s,new privilege level is supervisor",privilege_getstring(core_info -> privilege_level));
    csrv_mstatus = csr_read(csr_mstatus);
    csrv_mepc = csr_read(csr_mepc);
    core_info -> privilege_level = privilege_level_supervisor;
    csrv_mstatus.mstatus.mpp = privilege_level_supervisor;
    csrv_mepc.mepc.value = (ulong)targetfunc;
    csr_write(csr_mstatus,csrv_mstatus);
    csr_write(csr_mepc,csrv_mepc);
    asm volatile("mret");
}

void privilege_to_user(privilege_targetfunc targetfunc)
{
    core_info_t *core_info = core_get_info();
    csr_define_common csrv_status,csrv_epc;

    switch(core_info -> privilege_level)
    {
        case privilege_level_machine:
            syslog_info("privilege_to_supervisor","old privilege level is %s,new privilege level is user",privilege_getstring(core_info -> privilege_level));
            core_info -> privilege_level = privilege_level_user;
            csrv_status = csr_read(csr_mstatus);
            csrv_epc = csr_read(csr_mepc);
            csrv_status.mstatus.mpp = privilege_level_user;
            csrv_epc.mepc.value = (ulong)targetfunc;
            csr_write(csr_mstatus,csrv_status);
            csr_write(csr_mepc,csrv_epc);
            asm volatile("mret");
            break;

        case privilege_level_supervisor:
            syslog_info("privilege_to_supervisor","old privilege level is %s,new privilege level is user",privilege_getstring(core_info -> privilege_level));
            core_info -> privilege_level = privilege_level_user;
            csrv_status = csr_read(csr_sstatus);
            csrv_epc = csr_read(csr_sepc);
            csrv_status.sstatus.spp = privilege_level_user;
            csrv_epc.sepc.value = (ulong)targetfunc;
            csr_write(csr_sstatus,csrv_status);
            csr_write(csr_sepc,csrv_epc);
            asm volatile("sret");
            break;

        default:
            syslog_error("privilege_to_user","current privilege level is %s,try to transfer to user",privilege_getstring(core_info -> privilege_level));
            break;
    }
}

privilege_level privilege_get_cur_level()
{
    return core_get_readonly_info() -> privilege_level;
}