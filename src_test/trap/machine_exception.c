#include "common.h"
#include "linux/mm.h"
#include "linux/kernel.h"

extern int64_t system_call(ulong a0,ulong a1,ulong a2,ulong a3);
void do_wp_page(ulong address);
void do_no_page(ulong address);
//void machine_exception_handler_exit(ulong retvalue);
void interrupt_recover();
extern trap_info_t trap_info;
extern bool trap_from_interrupt;
extern int machine_main();
extern void entry(int core_id);

void machine_syscall(uintptr_t regs[32],uintptr_t fregs[32])
{
    csr_define_common csr;
    syslog_info("machine_syscall","func = %p",regs[10]);

    switch(regs[10])
    {
        case 0:
            csr = csr_read(csr_mie);
            csr.mie.meie = 1;
            csr_write(csr_mie,csr);
            syslog_info("machine_syscall","mie = 1");
            break;
    }
}

ulong xorsum = 0;
ulong first = 1;
uint8_t page_dir_bak[4096];

ulong GetPageDirSum()
{
    ulong sum = 0;

    if(page_dir_table == NULL)
    {
        syslog_print("page dir table is null!\r\n");
        while(1);
    }

    for(ulong i = 0;i < PAGE_SIZE;i += sizeof(xorsum))
    {
        sum += *((ulong *)(((ulong)page_dir_table) + i));
    }

    return sum;
}

void PrintPageDir()
{
    syslog_dump_memory(page_dir_table,2048);   
    syslog_dump_memory(page_dir_bak,2048);
}

void CheckXorSum()
{
    ulong curvalue;

    if(first == 1)
    {
        first = 0;
        return;
    }

    if(xorsum != (curvalue = GetPageDirSum()))
    {
        syslog_print("page xorsum is wrong,xorsum = %p,curvalue = %p!\r\n",xorsum,curvalue);
        PrintPageDir();
        while(1);
    }
}

void UpdateXorSum()
{
    xorsum = GetPageDirSum();
    memcpy(page_dir_bak,page_dir_table,4096);
}

void machine_exception_store_or_amo_access_fault(ulong addr)
{
    bool nopage = false;

    if((addr > USER_END_ADDR) || (addr < USER_START_ADDR))
    {
        nopage = true;
    }
    else if(page_dir_table == NULL)
    {
        nopage = true;
    }
    else if(!page_dir_table[GET_PAGE_DIR_ID(addr)].v)
    {
        nopage = true;
    }
    else if(!((volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)&page_dir_table[GET_PAGE_DIR_ID(addr)]))[GET_PAGE_ENTRY_ID(addr)].v)
    {
        nopage = true;
    }

    if(nopage)
    {
        do_no_page(addr);
    }
    else
    {
        do_wp_page(addr);
    }
}

uintptr_t machine_exception_handler(trap_exception cause,uintptr_t epc,uintptr_t regs[32],uintptr_t fregs[32],uintptr_t csregs[2])
{
    csr_define_common *mstatus = (csr_define_common *)(&csregs[0]);
    csr_define_common *mie = (csr_define_common *)(&csregs[1]);
    trap_info_t old_trap_info = trap_info;
    bool old_trap_from_interrupt = trap_from_interrupt;

    trap_from_interrupt = false;
    trap_info.regs = regs;
    trap_info.fregs = fregs;
    trap_info.epc = epc;
    trap_info.newepc = epc + 4;
    //trap_info.switch_to_recoveryaddr = machine_exception_handler_exit;
    //trap_listregs();
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
            machine_syscall(regs,fregs);
            break;

        case trap_exception_environment_call_from_m_mode:
            errorstr = "environment call from m-mode";
            break;

        default:
            errorstr = "unknown exception";
            break;
    }

    syslog_error("machine_exception_handler","exception %d,%s",cause,errorstr);

    if((cause < trap_exception_environment_call_from_u_mode || cause > trap_exception_environment_call_from_m_mode) && (((cause != trap_exception_load_access_fault) && (cause != trap_exception_store_or_amo_access_fault) && (cause != trap_exception_instruction_access_fault)) || (privilege_get_previous_level() == privilege_level_machine) || (csr_read(csr_mbadaddr).value < 0xC0000000UL)))
    {
        sysctl_disable_irq();
        syslog_print("exception %d,%s,epc = %p,badaddr = %p,cpl = %d\r\n",cause,errorstr,epc,csr_read(csr_mbadaddr).value,privilege_get_previous_level());
        trap_listregs();
        trap_listuserstack();
        while(1);
    }

    switch(cause)
    {
        case trap_exception_instruction_address_misaligned:
            break;

        case trap_exception_instruction_access_fault:
            trap_info.newepc = epc;
            //CheckXorSum();
            do_no_page(csr_read(csr_mbadaddr).mbadaddr.value);
            //UpdateXorSum();
            break;

        case trap_exception_illegal_instruction:
            break;

        case trap_exception_breakpoint:
            break;

        case trap_exception_load_address_misaligned:
            break;

        case trap_exception_load_access_fault:
            trap_info.newepc = epc;
            //CheckXorSum();
            do_no_page(csr_read(csr_mbadaddr).mbadaddr.value);
            //UpdateXorSum();
            break;

        case trap_exception_store_or_amo_address_misaligned:
            break;

        case trap_exception_store_or_amo_access_fault:
            trap_info.newepc = epc;
            //CheckXorSum();
            machine_exception_store_or_amo_access_fault(csr_read(csr_mbadaddr).mbadaddr.value);
            //syslog_print("access fault\r\n");
            //UpdateXorSum();
            break;

        case trap_exception_environment_call_from_u_mode:
            syslog_info("machine_exception","syscall id = %d",regs[reg_a0]);
            syslog_info("machine_exception","arg1 = %p,arg2 = %p,arg3 = %p",regs[reg_a1],regs[reg_a2],regs[reg_a3]);
            //printk("syscall id = %d\r\n",regs[reg_a0]);
            //while(1);

            /*if(regs[reg_a7] == 64)
            {
                ulong addr = regs[reg_a1];

                if(addr >= 0xC0000000UL)
                {
                    addr = 0xC00100C0;
                    user_addr_to_kernel(&addr);
                    syslog_print("sys_write:%s,curaddr = %p\r\n",addr,regs[reg_a1]);
                    syslog_dump_memory(addr,40);
                }
            }*/

            /*if(regs[reg_a7] == 64)
            {
                ulong addr = regs[reg_a1];
                user_addr_to_kernel(&addr);
                syslog_print("addr = %p,%p\n",regs[reg_a1],addr);
            }*/
            
            sysctl_enable_irq();
            regs[reg_a0] = system_call(regs[reg_a7],regs[reg_a0],regs[reg_a1],regs[reg_a2]);

            if(regs[reg_a7] == 1)
            {
                //syslog_print("return to %p\r\n",trap_info.newepc);
            }

            /*if(regs[reg_a7] == 64)
            {
                ulong addr = regs[reg_a1];

                if(addr >= 0xC0000000UL)
                {
                    addr = 0xC00100C0;
                    user_addr_to_kernel(&addr);
                    syslog_print("sys_write:%s\r\n",addr);
                    syslog_dump_memory(addr,40);
                }
            }*/
            //syslog_info("machine_exception","newepc = %p,ra = %p,s0 = %p",trap_info.newepc,regs[reg_ra],regs[reg_s0]);
            break;

        case trap_exception_environment_call_from_s_mode:
            machine_syscall(regs,fregs);
            break;

        case trap_exception_environment_call_from_m_mode:
            break;

        default:
            break;
    }

    if((cause >= trap_exception_environment_call_from_u_mode && cause <= trap_exception_environment_call_from_m_mode) || (cause == trap_exception_load_access_fault) || (cause == trap_exception_store_or_amo_access_fault) || (cause == trap_exception_instruction_access_fault))
    {
        //trap_listregs();
        sysctl_disable_irq();
        ulong r = trap_info.newepc;
        trap_info = old_trap_info;
        trap_from_interrupt = old_trap_from_interrupt;

        /*if((trap_info.regs[reg_sp] < 0x80000000UL) || (trap_info.regs[reg_ra] < 0x80000000UL))
        {
            printk("user stack exception from exception:sp = %p,ra = %p\r\n",trap_info.regs[reg_sp],trap_info.regs[reg_ra]);
        }

        if((*((ulong *)(r - 4UL)) != 0x00000073UL) && (privilege_get_previous_level() == privilege_level_user))
        {
            printk("exception:out of range epc:%p\r\n",r);
        }*/

        /*if(r < 0x80000000UL)
        {
            printk("exception:unknown epc:%p",trap_info.newepc);
            while(1);
        }*/

        //printk("leave exception,cpl = %d\r\n",privilege_get_previous_level());
        return r;
    }

    syslog_info("machine_exception_handler","infinite loop");
    //trap_listregs();
	while(1);
}

void machine_exception_delegate_enable(trap_exception_multitype value)
{
    csr_define_common csr = csr_read(csr_medeleg);
    csr.value |= (ulong)value;
    csr_write(csr_medeleg,csr);
}

void machine_exception_delegate_disable(trap_exception_multitype value)
{
    csr_define_common csr = csr_read(csr_medeleg);
    csr.value &= ~((ulong)value);
    csr_write(csr_medeleg,csr);
}

void machine_exception_delegate_enable_all()
{
    csr_define_common csr = csr_read(csr_medeleg);
    csr.value = ~0UL;
    csr_write(csr_medeleg,csr);
}

void machine_exception_delegate_disable_all()
{
    csr_define_common csr = csr_read(csr_medeleg);
    csr.value = 0UL;
    csr_write(csr_medeleg,csr);
}