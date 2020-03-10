#ifndef __CORE_H__
#define __CORE_H__

    #define CORE_NUM 2UL

    typedef struct core_info_t
    {
        ulong need_mie_set;
        ulong core_id;
        privilege_level privilege_level;
    }core_info_t;

    void core_init();
    core_info_t *core_get_info();
    const core_info_t *core_get_readonly_info();
    core_info_t core_get_copy_info();
    ulong core_get_coreid();
    ulong core_get_time();
    ulong core_get_cycle();

#endif