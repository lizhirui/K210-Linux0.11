#include "common.h"

void pte_common_init(volatile pte_64model *pte,int itemnum)
{
    memset((void *)pte,0,sizeof(pte_64model) * itemnum);
}

void pte_common_set_accessibility(volatile pte_64model *pte,pte_accessibility accessibility)
{
    pte -> r = bool_to_bit(accessibility & (1 << (PTE_OFFSET_R - PTE_OFFSETBASE_ACCESSIBILITY)));
    pte -> w = bool_to_bit(accessibility & (1 << (PTE_OFFSET_W - PTE_OFFSETBASE_ACCESSIBILITY)));
    pte -> x = bool_to_bit(accessibility & (1 << (PTE_OFFSET_X - PTE_OFFSETBASE_ACCESSIBILITY)));
}

void pte_common_addr_to_ppn(volatile pte_64model *pte,ulong addr)
{
    pte -> ppn = addr >> PTE_ADDR_OFFSET_LENGTH;
}

ulong pte_common_ppn_to_addr(volatile pte_64model *pte)
{
    return pte -> ppn << PTE_ADDR_OFFSET_LENGTH;
}

void pte_common_enable_entry(volatile pte_64model *pte)
{
    syslog_info("pte_common_enable_entry","pte 0x%08X enabled",(ulong)pte);
    pte -> v = 1;
}

void pte_common_disable_entry(volatile pte_64model *pte)
{
    pte -> v = 0;
}

void pte_common_enable_user(volatile pte_64model *pte)
{
    pte -> u = 1;
}

void pte_common_disable_user(volatile pte_64model *pte)
{
    pte -> u = 0;
}

void pte_common_set_writeable(volatile pte_64model *pte)
{
    pte -> w = 1;
}

//Must be executed in machine mode
void pte_entry_sv39()
{
    syslog_info("pte_entry_sv39","entry sv39 page mode");
    csr_define_common csr = csr_read(csr_mstatus);
    csr.mstatus.vm = pte_mode_sv39;
    csr_write(csr_mstatus,csr);
}

//Must be executed in machine mode
void pte_entry_sv48()
{
    csr_define_common csr = csr_read(csr_mstatus);
    csr.mstatus.vm = pte_mode_sv48;
    csr_write(csr_mstatus,csr);
}

//Must be executed in machine mode
void pte_exit()
{
    csr_define_common csr = csr_read(csr_mstatus);
    csr.mstatus.vm = pte_mode_none;
    csr_write(csr_mstatus,csr);
}

void pte_refresh_tlb()
{
    asm volatile("sfence.vm");
}

//pte must be aligned to 4-byte
void pte_set_root_page_table(volatile pte_64model *pte)
{
    csr_define_common csr = csr_read(csr_sptbr);
    csr.sptbr.ppn = ((ulong)pte) >> PTE_ADDR_OFFSET_LENGTH;
    csr_write(csr_sptbr,csr);
    pte_refresh_tlb();
}