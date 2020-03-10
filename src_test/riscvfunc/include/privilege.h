#ifndef __PRIVILEGE_H__
#define __PRIVILEGE_H__

    typedef enum privilege_level
    {
        privilege_level_user = 0,
        privilege_level_supervisor = 1,
        privilege_level_machine = 3
    }privilege_level;

    typedef void (*privilege_targetfunc)();

    const char *privilege_getstring(privilege_level level);
    void privilege_init(privilege_level cur_privilege_level);
    void privilege_trap_entry(privilege_level cur_privilege_level,ulong cause,ulong epc,ulong badaddr);
    privilege_level privilege_get_previous_level();
    ulong privilege_trap_exit(ulong epc);
    void privilege_to_supervisor(privilege_targetfunc targetfunc);
    void privilege_to_user(privilege_targetfunc targetfunc);
    privilege_level privilege_get_cur_level();
    

#endif