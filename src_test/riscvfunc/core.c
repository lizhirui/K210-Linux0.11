#include "common.h"

core_info_t core_info[CORE_NUM];

#define CORE_GET_REGISTER_TEMPLATE(registername) \
    switch(privilege_get_cur_level()) \
    { \
        case privilege_level_machine: \
            return csr_read(csr_m##registername).m##registername.value; \
            \
        case privilege_level_supervisor: \
            return csr_read(csr_s##registername).s##registername.value; \
            \
        default: \
            return 0UL; \
    }

#define CORE_GET_PERFORMANCE_REGISTER_TEMPLATE(registername) \
    switch(privilege_get_cur_level()) \
    { \
        case privilege_level_machine: \
            return csr_read(csr_m##registername).m##registername.value; \
            \
        case privilege_level_supervisor: \
            return csr_read(csr_##registername).registername.value; \
            \
        default: \
            return 0UL; \
    }

//Only can be executed when machine mode
void core_init()
{
    ulong core_id = csr_read(csr_mhartid).mhartid.hart_id;
    csr_define_common csr;

    if(core_id >= CORE_NUM)
    {
        syslog_error("core_init","core id %d is illegal",core_id);
        return;
    }

    csr.value = (ulong)&core_info[core_id];
    csr_write(csr_mscratch,csr);
    csr_write(csr_sscratch,csr);
    csr = csr_read(csr_mscounteren);
    csr.mscounteren.CY = 1;
    csr.mscounteren.TM = 1;
    csr.mscounteren.IR = 1;
    csr_write(csr_mscounteren,csr);
    csr = csr_read(csr_mucounteren);
    csr.mucounteren.CY = 1;
    csr.mucounteren.TM = 1;
    csr.mucounteren.IR = 1;
    csr_write(csr_mucounteren,csr);
    core_info[core_id].core_id = core_id;
    privilege_init(privilege_level_machine);
}

core_info_t *core_get_info()
{
    csr_define_common csr;
    csr = csr_read(csr_sscratch);
    return (core_info_t *)csr.sscratch.value;
}

const core_info_t *core_get_readonly_info()
{
    return (const core_info_t *)core_get_info();
}

core_info_t core_get_copy_info()
{
    return *(core_get_info());
}

ulong core_get_coreid()
{
    return core_get_info() -> core_id;
}

ulong core_get_time()
{
    return csr_read(csr_time).time.value;
}

ulong core_get_cycle()
{
    CORE_GET_PERFORMANCE_REGISTER_TEMPLATE(cycle);
}