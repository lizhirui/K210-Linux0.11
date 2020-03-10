#include "common.h"
#include "drivers.h"
#include "entry.h"

extern volatile uint64_t g_wake_up[2];

core_instance_t core1_instance;

extern int machine_main();

void thread_entry(int core_id)
{
    while (!atomic_read(&g_wake_up[core_id]));
}

void core_enable(int core_id)
{
    clint_ipi_send(core_id);
    atomic_set(&g_wake_up[core_id], 1);
}

int register_core1(core_function func, void* ctx)
{
    if (func == NULL)
        return -1;
    core1_instance.callback = func;
    core1_instance.ctx = ctx;
    core_enable(1);
    return 0;
}

void init_bss()
{
    extern unsigned int _bss;
    extern unsigned int _ebss;
    unsigned int *dst;
    dst = &_bss;

    while(dst < &_ebss)
    {
        *dst++ = 0;
    }
}

void entry(int core_id)
{
    if(core_id == 0)
    {
        sysctl_pll_set_freq(SYSCTL_PLL0,800000000U);
        init_bss();
        fpioa_set_function(4,FUNC_UART3_RX);
        fpioa_set_function(5,FUNC_UART3_TX);
        uart_debug_init(UART_DEBUG_DEVICE);
        /*fpioa_set_function(31,FUNC_JTAG_TDI);
        fpioa_set_function(30,FUNC_JTAG_TDO);
        fpioa_set_function(33,FUNC_JTAG_TMS);
        fpioa_set_function(32,FUNC_JTAG_TCLK);*/
        sysctl_set_power_mode(SYSCTL_POWER_BANK0,SYSCTL_POWER_V33);
        //fpioa_set_io_pull(30,FPIOA_PULL_UP);
        fpioa_init();
        sysctl_get_reset_status();
        sysctl_disable_irq();
        core_init();
        plic_init();        
        sleep(3);
    }

    if(core_id == 0)
    {
        core1_instance.callback = NULL;
        core1_instance.ctx = NULL;
        machine_main();
    }
    else
    {
        //plic_init();
        //sysctl_enable_irq();
        sysctl_disable_irq();
        thread_entry(core_id);

        if(core1_instance.callback == NULL)
        {
            while(1)
            {
                //asm volatile("wfi");
            }
        }
        else
        {
            core1_instance.callback(core1_instance.ctx);
        }
    }
}