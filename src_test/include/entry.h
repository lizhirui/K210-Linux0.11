#ifndef __ENTRY_H__
#define __ENTRY_H__
    
    typedef int (*core_function)(void* ctx);

    typedef struct _core_instance_t
    {
        core_function callback;
        void* ctx;
    } core_instance_t;

    int register_core1(core_function func, void* ctx);

	void entry();
#endif