#ifndef _MM_H
#define _MM_H

    #define PAGE_SIZE 4096UL

    #define LOW_MEM 0x80100000UL//Memory low address
    #define PAGING_SIZE 4096UL//Must be divided sizeof(ulong)
    #define PAGING_SHIFT 12
    #define PAGING_HIGH_LEVEL_SHIFT 21
    #define PAGING_HIGH_LEVEL_SIZE (2UL * 1024UL * 1024UL)
    #define PAGING_MEMORY (15UL * 1024UL * 1024UL)//Paging memory 15MB
    #define PAGING_PAGES (PAGING_MEMORY >> PAGING_SHIFT)//Physics paging count
    #define GET_PAGE_DIR_ID(x) (((x) & 0x3FFFFFFFUL) >> PAGING_HIGH_LEVEL_SHIFT)
    #define GET_PAGE_DIR_SIZE(x) (((x) + PAGING_HIGH_LEVEL_SIZE - 1) >> PAGING_HIGH_LEVEL_SHIFT)
    #define GET_PAGE_ENTRY_ID(x) (((x) & (PAGING_HIGH_LEVEL_SIZE - 1)) >> PAGING_SHIFT)
    #define GET_PAGE_ID(x) (((x) & 0x3FFFFFFFUL) >> PAGING_SHIFT)

    #define USER_START_ADDR 0xC0000000UL
    #define USER_END_ADDR (USER_START_ADDR + PAGE_DIR_TABLE_NUM * PAGE_TABLE_ITEM_NUM * PAGE_SIZE - 1UL)

    extern ulong get_free_page(void);
    extern ulong get_free_pages(ulong pagenum);
    extern ulong put_page(ulong page,ulong address);
    extern void free_page(ulong addr);
    extern void free_pages(ulong addr,ulong pagenum);
    bool user_addr_to_kernel(ulong *addr);
    bool user_ptr_to_kernel(void **ptr);
    bool user_addr_to_kernel_write(ulong *addr);
    void page_transform_start();
    bool page_transform_addr(ulong *addr);
    bool page_transform_ptr(void **ptr);
    void mem_copy_from_kernel(ulong fromaddr,ulong toaddr,ulong size);
    void mem_copy_to_kernel(ulong fromaddr,ulong toaddr,ulong size);

#endif
