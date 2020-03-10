#ifndef __PAGE_TABLE_ENTRY_H__
#define __PAGE_TABLE_ENTRY_H__

    //Page Table Entry for Sv39
    typedef struct pte_sv39
    {
        ulong v : 1;//It indicates whether the PTE is valid
        ulong r : 1;//It indicates whether the page can be read
        ulong w : 1;//It indicates whether the page can be written
        ulong x : 1;//It indicates whether the page can be executed
        ulong u : 1;//It indicates whether the page is accessible to user mode,supervisor mode software can access pages with U = 1 only when PUM in the sstatus register = 0(normally 1)
        ulong g : 1;//It designates a global mapping
        ulong a : 1;//Accessed Status
        ulong d : 1;//Dirty Status
        ulong : 2;
        ulong ppn0 : 9;//Physical Page Number 0
        ulong ppn1 : 9;//Physical Page Number 1
        ulong ppn2 : 20;//Physical Page Number 2
        ulong : 16;
    }ALIGN4BYTE pte_sv39;

    //Page Table Entry for Sv48
    typedef struct pte_sv48
    {
        ulong v : 1;//It indicates whether the PTE is valid
        ulong r : 1;//It indicates whether the page can be read
        ulong w : 1;//It indicates whether the page can be written
        ulong x : 1;//It indicates whether the page can be executed
        ulong u : 1;//It indicates whether the page is accessible to user mode,supervisor mode software can access pages with U = 1 only when PUM in the sstatus register = 0(normally 1)
        ulong g : 1;//It designates a global mapping
        ulong a : 1;//Accessed Status
        ulong d : 1;//Dirty Status
        ulong : 2;
        ulong ppn0 : 9;//Physical Page Number 0
        ulong ppn1 : 9;//Physical Page Number 1
        ulong ppn2 : 9;//Physical Page Number 2
        ulong ppn3 : 11;//Physical Page Number 3
        ulong : 16;
    }ALIGN4BYTE pte_sv48;

    //Page Table Entry 64-Bit Model
    typedef struct pte_64model
    {
        ulong v : 1;//It indicates whether the PTE is valid
        ulong r : 1;//It indicates whether the page can be read
        ulong w : 1;//It indicates whether the page can be written
        ulong x : 1;//It indicates whether the page can be executed
        ulong u : 1;//It indicates whether the page is accessible to user mode,supervisor mode software can access pages with U = 1 only when PUM in the sstatus register = 0(normally 1)
        ulong g : 1;//It designates a global mapping
        ulong a : 1;//Accessed Status
        ulong d : 1;//Dirty Status
        ulong : 2;
        ulong ppn : 38;//Physical Page Number
        ulong : 16;
    }ALIGN4BYTE pte_64model;

    //Page Table Entry Accessibility
    typedef enum pte_accessibility
    {
        pte_accessibility_pointer = 0,
        pte_accessibllity_readonly = 1,
        pte_accessibility_readwrite = 3,
        pte_accessibility_executeonly = 4,
        pte_accessibility_readexecute = 5,
        pte_accessibility_all = 7
    }pte_accessibility;

    #define PTE_OFFSET_V 0
    #define PTE_OFFSET_R 1
    #define PTE_OFFSET_W 2
    #define PTE_OFFSET_X 3
    #define PTE_OFFSET_U 4
    #define PTE_OFFSET_G 5
    #define PTE_OFFSET_A 6
    #define PTE_OFFSET_D 7
    #define PTE_OFFSET_PPN 10

    #define PTE_OFFSETBASE_ACCESSIBILITY 1

    #define PTE_ADDR_OFFSET_LENGTH 12

    typedef enum pte_mode
    {
        pte_mode_none = 0,
        pte_mode_sv39 = 9,
        pte_mode_sv48 = 10
    }pte_mode;

    void pte_common_init(volatile pte_64model *pte,int itemnum);
    void pte_common_set_accessibility(volatile pte_64model *pte,pte_accessibility accessibility);
    void pte_common_addr_to_ppn(volatile pte_64model *pte,ulong addr);
    ulong pte_common_ppn_to_addr(volatile pte_64model *pte);
    void pte_common_enable_entry(volatile pte_64model *pte);
    void pte_common_disable_entry(volatile pte_64model *pte);
    void pte_common_enable_user(volatile pte_64model *pte);
    void pte_common_disable_user(volatile pte_64model *pte);
    void pte_common_set_writeable(volatile pte_64model *pte);
    void pte_entry_sv39();
    void pte_entry_sv48();
    void pte_exit();
    void pte_refresh_tlb();
    void pte_set_root_page_table(volatile pte_64model *pte);

#endif