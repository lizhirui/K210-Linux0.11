#include "common.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/sched.h"
#include "signal.h"

volatile void do_exit(int code);

static inline volatile void oom()
{
    printk("out of memory\r\n");
    do_exit(SIGSEGV);
}

#define invalidate pte_refresh_tlb


#define MAP_NR(addr) (((addr) - LOW_MEM) >> PAGING_SHIFT)//Address maps to page id
#define USED 100//paging is used flag

#define ALIGN_TEST_4KB(x) ((x) & (PAGING_SIZE - 1))
#define ALIGN_TEST_2MB(x) ((x) & (PAGING_HIGH_LEVEL_SIZE - 1))

#define CODE_SPACE(x) 0

static ulong HIGH_MEMORY = 0;//Memory high address

static ulong inituseraddr = 0UL;
static ulong lastuseraddr = 0UL;
static ulong lastkerneladdr = 0UL;
static ulong traned = false;

extern trap_info_t trap_info;

void page_transform_start()
{
    traned = false;
}

bool page_transform_addr(ulong *addr)
{
    if((traned == false) || ((GET_PAGE_ID(*addr) != GET_PAGE_ID(lastkerneladdr))))
    {
        traned = true;
        lastuseraddr = *addr - lastkerneladdr + lastuseraddr;
        *addr = lastuseraddr;
        bool r = user_addr_to_kernel(&addr);
        lastkerneladdr = *addr;
        return r;
    }

    return true;
}

bool page_transform_ptr(void **ptr)
{
    return page_transform_addr((ulong *)ptr);
}

void mem_copy_from_kernel(ulong fromaddr,ulong toaddr,ulong size)
{
    char *from = (char *)fromaddr;
    char *to = (char *)toaddr;

    page_transform_start();

    while(size--)
    {
        page_transform_ptr(&to);
        *to = *from;
    }
}

void mem_copy_to_kernel(ulong fromaddr,ulong toaddr,ulong size)
{
    char *from = (char *)fromaddr;
    char *to = (char *)toaddr;

    page_transform_start();

    while(size--)
    {
        page_transform_ptr(&from);
        *to = *from;
    }
}

void copy_page(ulong from,ulong to)
{
    ulong i;

    for(i = 0;i < PAGING_SIZE;i += sizeof(ulong))
    {
        *(ulong *)to = *(ulong *)from;
    }
}

static uint8_t mem_map[PAGING_PAGES] = {0,};

//Get physical address of first(actually last) free page,and mark it used.If no free pages left,return 0
ulong get_free_page()
{
    ulong i = PAGING_PAGES;
    ulong addr;

    while(--i)
    {
        if(mem_map[i] == 0)
        {
            mem_map[i] = 1;
            addr = (i << PAGING_SHIFT) + LOW_MEM;
            memset((void *)addr,0,PAGING_SIZE);
            return addr;
        }
    }

    return 0;
}

//Get physical address of first(actually last) free page,and mark it used.If no free pages left,return 0
ulong get_free_pages(ulong pagenum)
{
    ulong i = PAGING_PAGES - 1;
    ulong j;
    bool exist = false;
    ulong addr;

    while(i > (pagenum - 1))
    {
        exist = false;

        for(j = i;j >= i - pagenum + 1;j--)
        {
            if(mem_map[j] != 0)
            {
                exist = true;
                i = j - 1;
                break;
            }
        }

        if(!exist)
        {
            for(j = i;j >= i - pagenum + 1;j--)
            {
                mem_map[j] = 1;
            }

            addr = ((i - pagenum + 1) << PAGING_SHIFT) + LOW_MEM;
            memset((void *)addr,0,PAGING_SIZE * pagenum);
            return addr;
        }
    }

    return 0;
}

//Free a page of memory at physical address 'addr'.Used by 'free_page_tables()'
void free_page(ulong addr)
{
    if(addr < LOW_MEM)
    {
        return;
    }

    if(addr >= HIGH_MEMORY)
    {
        panic("trying to free nonexistent page");
    }

    addr -= LOW_MEM;
    addr >>= PAGING_SHIFT;

    if(mem_map[addr]--)
    {
        return;
    }

    mem_map[addr] = 0;
    panic("trying to free free page");
}

void free_pages(ulong addr,ulong pagenum)
{
    ulong i;

    for(i = 0;i < pagenum;i++,addr += PAGE_SIZE)
    {
        free_page(addr);
    }
}

//This function frees a continuous block of page tables,as needed by 'exit()'.As does copy_page_tables(),this handles only 2Mb blocks
int free_page_tables(ulong from,volatile pte_sv39 *dir,ulong size)
{
    volatile pte_sv39 *pg_table;
    ulong nr;

    //if(ALIGN_TEST_2MB(from))
    if(ALIGN_TEST_4KB(from))
    {
        printk("addr = %p  ",from);
        panic("free_page_tables called with wrong alignment");
    }

    if(!from)
    {
        panic("Trying to free up swapper memory space");
    }

    size = GET_PAGE_DIR_SIZE(size);
    dir = &page_dir_table[GET_PAGE_DIR_ID(from)];

    for(;size-- > 0;dir++)
    {
        if(!dir -> v)
        {
            continue;
        }

        pg_table = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)dir);

        for(nr = 0;nr < PAGE_TABLE_ITEM_NUM;nr++)
        {
            if(pg_table -> v)
            {
                free_page(pte_common_ppn_to_addr((volatile pte_64model *)pg_table));
            }

            pte_common_init((volatile pte_64model *)pg_table,1);
            pg_table++;
        }

        free_page(pte_common_ppn_to_addr((volatile pte_64model *)dir));
        pte_common_init((volatile pte_64model *)dir,1);
    }

    invalidate();
}

//It copies a range of linear addresses by copying only the pages
//We don't copy any chunks of memory - addresses have to be divisibile by 2Mb(one page_directory entry),as this makes the functino easier.It's used only by fork anyway.
int copy_page_tables(ulong from,ulong to,volatile pte_sv39 *to_dir,ulong size)
{
    volatile pte_sv39 *from_page_table;
    volatile pte_sv39 *to_page_table;
    volatile pte_sv39 *from_dir;
    ulong pagebaseaddr;
    ulong nr;

    if(to_dir == NULL)
    {
        return 0;
    }

    if(ALIGN_TEST_2MB(from) || ALIGN_TEST_2MB(to))
    {
        panic("copy_page_tables called with wrong alignment");
    }

    from_dir = &page_dir_table[GET_PAGE_DIR_ID(from)];
    //to_dir = &page_dir_table[GET_PAGE_DIR_ID(to)];
    size = GET_PAGE_DIR_SIZE(size);

    for(;size-- > 0;from_dir++,to_dir++)
    {
        if(to_dir -> v)
        {
            panic("copy_page_tables: already exist");
        }

        if(!from_dir -> v)
        {
            continue;
        }

        from_page_table = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)from_dir);
        
        if(!(to_page_table = (volatile pte_sv39 *)get_free_page()))
        {
            return -1;
        }

        pte_common_addr_to_ppn((volatile pte_64model *)to_dir,(ulong)to_page_table);
        pte_common_set_accessibility((volatile pte_64model *)to_dir,pte_accessibility_pointer);
        pte_common_enable_entry((volatile pte_64model *)to_dir);

        nr = 512;

        for(;nr-- > 0;from_page_table++,to_page_table++)
        {
            if(!from_page_table -> v)
            {
                continue;
            }

            *to_page_table = *from_page_table;
            pte_common_set_accessibility((volatile pte_64model *)to_page_table,pte_accessibility_readexecute);
            pagebaseaddr = pte_common_ppn_to_addr((volatile pte_64model *)to_page_table);

            if(pagebaseaddr > LOW_MEM)
            {
                *from_page_table = *to_page_table;
                mem_map[(pagebaseaddr - LOW_MEM) >> PAGING_SHIFT]++;
            }
        }
    }

    invalidate();
    return 0;
}

//This function puts a page in memory at the wanted address
//It returns the physical address of the page gotten,0 if 
//out of memory(either when trying to access page-table or page)
ulong put_page(ulong page,ulong address)
{
    ulong tmp;
    volatile pte_sv39 *page_table;

    if(page < LOW_MEM || page >= HIGH_MEMORY)
    {
        printk("Trying to put page %p\n",page,address);
    }

    if(mem_map[(page - LOW_MEM) >> PAGING_SHIFT] != 1)
    {
        printk("mem_map disagrees with %p at %p\n",page,address);
    }

    page_table = &page_dir_table[GET_PAGE_DIR_ID(address)];
    //syslog_print("page_table address = %p\r\n",page_table);

    if(page_table -> v)
    {
        page_table = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)page_table);
    }
    else
    {
        if(!(tmp = get_free_page()))
        {
            return 0;
        }

        pte_common_init(tmp,PAGE_TABLE_ITEM_NUM);
        pte_common_addr_to_ppn((volatile pte_64model *)page_table,tmp);
        pte_common_set_accessibility((volatile pte_64model *)page_table,pte_accessibility_pointer);
        pte_common_enable_user((volatile pte_64model *)page_table);
        pte_common_enable_entry((volatile pte_64model *)page_table);
        page_table = (volatile pte_sv39 *)tmp;
    }

    //syslog_print("%d\r\n",(address >> PAGING_SHIFT) & (PAGE_TABLE_ITEM_NUM - 1));
    page_table = &page_table[GET_PAGE_ENTRY_ID(address)];
    pte_common_addr_to_ppn((volatile pte_64model *)page_table,page);
    pte_common_set_accessibility((volatile pte_64model *)page_table,pte_accessibility_all);
    pte_common_enable_user((volatile pte_64model *)page_table);
    pte_common_enable_entry((volatile pte_64model *)page_table);
    //no need for invalidate
    invalidate();
    return page;
}

//un_wp_page -- Un-Write Protect Page
void un_wp_page(volatile pte_sv39 *table_entry)
{
    ulong old_page;
    ulong new_page;

    old_page = pte_common_ppn_to_addr((volatile pte_64model *)table_entry);

    if((old_page) >= LOW_MEM && (mem_map[MAP_NR(old_page)] == 1))
    {
        pte_common_set_writeable((volatile pte_64model *)table_entry);
        invalidate();
        return;
    }

    if(!(new_page = get_free_page()))
    {
        oom();//Out of Memory
    }

    if(old_page >= LOW_MEM)
    {
        mem_map[MAP_NR(old_page)]--;
    }

    pte_common_addr_to_ppn((volatile pte_64model *)table_entry,new_page);
    pte_common_set_accessibility((volatile pte_64model *)table_entry,pte_accessibility_all);
    pte_common_enable_user((volatile pte_64model *)table_entry);
    pte_common_enable_entry((volatile pte_64model *)table_entry);
    invalidate();
    copy_page(old_page,new_page);
}

//This routine handles present pages,when users try to write to a shared page.
//It is done by copying the page to a new address and decrementing the shared-page counter for the old page.
//If it's in code space we exit with a segment error
void do_wp_page(ulong address)
{
    volatile pte_sv39 *dir = &page_dir_table[GET_PAGE_DIR_ID(address)];
    volatile pte_sv39 *entry = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)dir);
    un_wp_page(&entry[GET_PAGE_ENTRY_ID(address)]);
}

void write_verify(ulong address)
{
    volatile pte_sv39 *dir = &page_dir_table[GET_PAGE_DIR_ID(address)];

    if(dir -> v == 0)
    {
        return;
    }

    volatile pte_sv39 *page = ((volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)dir)) + GET_PAGE_ENTRY_ID(address);

    if((page -> v == 1) && (page -> w == 0))
    {
        un_wp_page(page);
    }
}

void get_empty_page(ulong address)
{
    ulong tmp;

    if(!(tmp = get_free_page()) || (!put_page(tmp,address)))
    {
        syslog_print("error\r\n");
        free_page(tmp);//0 is ok - ignored
        oom();
    }
}

//try_to_share() checks the page at address "address" in the task "p",
//to see if it exists,and if it is clean.If so,share it with the current task.
//NOTE! this assumes we have checked that p != current,and that they share the same executable
//static int try_to_share(ulong address,struct task_struct *p)

static int try_to_share(ulong address,struct task_struct *p)
{
    ulong from;
    ulong to;
    pte_sv39 *from_page;
    pte_sv39 *to_page;
    ulong phys_addr;

    from_page = &(p -> page_dir_table[GET_PAGE_DIR_ID(address)]);
    to_page = &(current -> page_dir_table[GET_PAGE_DIR_ID(address)]);

    if(!(from_page -> v))
    {
        return 0;
    }

    from_page = ((pte_sv39 *)pte_common_ppn_to_addr(from_page)) + GET_PAGE_ENTRY_ID(address);

    //is the page clean and present?
    if((!from_page -> v) || from_page -> d)
    {
        return 0;
    }

    phys_addr = pte_common_ppn_to_addr(from_page);

    if((phys_addr >= HIGH_MEMORY) || (phys_addr < LOW_MEM))
    {
        return 0;
    }

    if(!(to_page -> v))
    {
        if(to = get_free_page())
        {
            pte_common_addr_to_ppn(to_page,to);
            pte_common_set_accessibility(to_page,pte_accessibility_all);
            pte_common_enable_entry(to_page);
            pte_common_enable_user(to_page);
        }
        else
        {
            oom();
        }
    }

    to_page = ((pte_sv39 *)pte_common_ppn_to_addr(to_page)) + GET_PAGE_ENTRY_ID(address);

    if(to_page -> v)
    {
        panic("try_to_share: to_page already exists");
    }

    //share them:write-protect
    from_page -> w = 0;
    to_page -> w = 0;
    invalidate();
    phys_addr -= LOW_MEM;
    phys_addr >>= PAGING_SHIFT;
    mem_map[phys_addr]++;
    return 1;
}

//share_page() tries to find a process that could share a page with the current one.
//Address is the address of the wanted page relative to the current data space.
//We first check if it is at all feasible by checking executable -> i_count.
//It should be > 1 if there are other tasks sharing this inode.
static int share_page(ulong address)
{
    struct task_struct **p;

    if(!current -> executable)
    {
        return 0;
    }

    if(current -> executable -> i_count < 2)
    {
        return 0;
    }

    for(p = &LAST_TASK;p >= &FIRST_TASK;--p)
    {
        if(!*p)
        {
            continue;
        }

        if(current == *p)
        {
            continue;
        }

        if((*p) -> executable != current -> executable)
        {
            continue;
        }

        if(try_to_share(address,*p))
        {
            return 1;
        }
    }

    return 0;
}

//int first = 1;

void do_no_page(ulong address)
{
    int nr[4];
    ulong tmp;
    ulong page;
    int block,i;
    //syslog_print("address = %p,cause = %d,epc = %p\r\n",address,csr_read(csr_mcause).value,trap_info.epc);
    address &= ~(PAGE_SIZE - 1);
    /*sysctl_disable_irq();
    syslog_print("cpl = %d\r\n",privilege_get_previous_level());
    syslog_print("%d\r\n",page_dir_table[0].v);
    syslog_print("address = %p,cause = %d,epc = %p\r\n",address,csr_read(csr_mcause).value,trap_info.epc);
    if(first == 0)
    {
        syslog_print("no first check:%p\r\n",address);
        csr_define_common csr = csr_read(csr_sptbr);
        pte_sv39 *root = csr.sptbr.ppn << 12;
        root = &root[address >> 30];

        //syslog_print("ppn = %p\r\n",csr.sptbr.ppn);
        syslog_print("1:");

        if(!root -> v)
        {
            syslog_print("root item is disabled.\r\n");
        }

        syslog_print("2:");

        if(!root -> u)
        {
            syslog_print("root item isn't for user.\r\n");
        }

        pte_sv39 *dir = pte_common_ppn_to_addr(root);
        syslog_print("xpage address = %p\r\n",dir);
        dir = &dir[(address & 0x3FFFFFFFUL) >> 21];
        syslog_print("3:");

        if(!dir -> v)
        {
            syslog_print("dir item is disabled.\r\n");
        }

        syslog_print("4:");

        if(!dir -> u)
        {
            syslog_print("dir item isn't for user.\r\n");
        }

        pte_sv39 *entry = pte_common_ppn_to_addr(dir);
        entry = &entry[(address & 0x1FFFFFUL) >> 12];
        syslog_print("5:");

        if(!entry -> v)
        {
            syslog_print("entry item is disabled.\r\n");
        }

        syslog_print("6:");

        if(!entry -> u)
        {
            syslog_print("entry item isn't for user.\r\n");
        }

        ulong addr = pte_common_ppn_to_addr(entry);
        syslog_print("result is %p\r\n",addr);
        
        user_addr_to_kernel(&address);
        syslog_print("unknown error,new addr = %p\r\n",address);
        while(1);
    }
    else
    {
        first = 0;
    }*/

    //syslog_print("check,%p,%d,%p\r\n",address,current -> executable,current -> end_data);
    tmp = address;

    if((!current -> executable) || (tmp >= current -> end_data))
    {
        get_empty_page(address);
        return;
    }

    //syslog_print("share_page\r\n");

    if(share_page(tmp))
    {
        return;
    }

    //syslog_print("get_free_page\r\n");

    if(!(page = get_free_page()))
    {
        oom();
    }

    tmp = address - current -> start_code;

    //remember that 1 block is used for header
    block = 1 + tmp / BLOCK_SIZE;

    for(i = 0;i < 4;block++,i++)
    {
        nr[i] = bmap(current -> executable,block);
    }

    //syslog_print("bread_page\r\n");
    bread_page(page,current -> executable -> i_dev,nr);
    
    i = tmp + current -> start_code + 4096 - current -> end_data;
    //syslog_print("i = %d\r\n",i);
    tmp = page + 4096;

    while(i-- > 0)
    {
        tmp--;
        *(char *)tmp = 0;
    }

    //syslog_print("put_page\r\n");
    //syslog_print("dir %d,entry %d\r\n",GET_PAGE_DIR_ID(address),GET_PAGE_ENTRY_ID(address));
    if(put_page(page,address))
    {
        //syslog_print("%p -> %p\r\n",address,page);
        /*syslog_print("put_page_page_dir_table = %p\r\n",&page_dir_table[GET_PAGE_DIR_ID(address)]);
        syslog_print("after put_page:address = %p\r\n",address);
        extern uint64_t get_sp();
        extern ulong cur_kernel_stackbottom;
        syslog_print("sp = %p,kernel_bottom = %p\r\n",get_sp(),kernel_stackbottom[2],cur_kernel_stackbottom);*/
        //user_addr_to_kernel(&address);
        /*syslog_print("after transform:address = %p\r\n",address);
        extern trap_info_t trap_info;
        syslog_print("epc = %p\r\n",trap_info.newepc);*/
        /*syslog_print("page address = %p\r\n",page_dir_table);
        syslog_print("cpl = %d\r\n",privilege_get_previous_level());
        syslog_print("%d\r\n",page_dir_table[0].v);
        syslog_print("%p:%p\r\n",address,*(ulong *)address);*/
        return;
    }

    free_page(page);
    oom();
}

extern void machine_exception_store_or_amo_access_fault(ulong addr);

bool user_addr_to_kernel_write(ulong *addr)
{
    int first = 1;

    repeat:

    if(*addr < USER_START_ADDR)
    {
        return true;
    }

    if(page_dir_table == NULL)
    {
        panic("page_dir_table is null\r\n");
        return false;
    }

    if(*addr > USER_END_ADDR)
    {
        panic("addr > USER_END_ADDR\r\n");
        return false;
    }

    if(!page_dir_table[GET_PAGE_DIR_ID(*addr)].v)
    {
        if(first == 1)
        {
            first = 0;
            machine_exception_store_or_amo_access_fault(*addr);
            goto repeat;
        }

        syslog_print("*addr = %p\r\n",*addr);
        panic("page dir is not enabled\r\n");
        return false;
    }
    
    volatile pte_sv39 *pt = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)&page_dir_table[GET_PAGE_DIR_ID(*addr)]);

    if(!pt[GET_PAGE_ENTRY_ID(*addr)].v)
    {
        if(first == 1)
        {
            first = 0;
            machine_exception_store_or_amo_access_fault(*addr);
            goto repeat;
        }

        syslog_print("*addr = %p,epc = %p\r\n",*addr,trap_info.epc);
        panic("page entry is not enabled\r\n");
        return false;
    }

    *addr = pte_common_ppn_to_addr(&pt[GET_PAGE_ENTRY_ID(*addr)]) + ((*addr) & 0xFFF);
    return true;
}

bool user_addr_to_kernel(ulong *addr)
{
    int first = 1;

    repeat:

    if(*addr < USER_START_ADDR)
    {
        return true;
    }

    if(page_dir_table == NULL)
    {
        panic("page_dir_table is null\r\n");
        return false;
    }

    if(*addr > USER_END_ADDR)
    {
        panic("addr > USER_END_ADDR\r\n");
        return false;
    }

    if(!page_dir_table[GET_PAGE_DIR_ID(*addr)].v)
    {
        if(first == 1)
        {
            first = 0;
            do_no_page(*addr);
            goto repeat;
        }

        syslog_print("*addr = %p\r\n",*addr);
        panic("page dir is not enabled\r\n");
        return false;
    }
    
    volatile pte_sv39 *pt = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)&page_dir_table[GET_PAGE_DIR_ID(*addr)]);

    if(!pt[GET_PAGE_ENTRY_ID(*addr)].v)
    {
        if(first == 1)
        {
            first = 0;
            do_no_page(*addr);
            goto repeat;
        }

        syslog_print("*addr = %p,epc = %p\r\n",*addr,trap_info.epc);
        panic("page entry is not enabled\r\n");
        return false;
    }

    *addr = pte_common_ppn_to_addr(&pt[GET_PAGE_ENTRY_ID(*addr)]) + ((*addr) & 0xFFF);
    return true;
}

bool user_ptr_to_kernel(void **ptr)
{
    return user_addr_to_kernel((ulong *)ptr);
}

void mem_init(ulong start_mem,ulong end_mem)
{
    int i;

    HIGH_MEMORY = end_mem;

    for(i = 0;i < PAGING_PAGES;i++)
    {
        mem_map[i] = USED;
    }

    i = MAP_NR(start_mem);
    end_mem -= start_mem;
    end_mem >>= PAGING_SHIFT;

    while(end_mem-- > 0)
    {
        mem_map[i++] = 0;
    }
}

//for debug only
void calc_mem()
{
    int i,j,k,free = 0;
    volatile pte_sv39 *pg_tbl;

    for(i = 0;i < PAGING_PAGES;i++)
    {
        if(!mem_map[i])
        {
            free++;
        }
    }

    printk("%d pages free (of %d)\r\n",free,PAGING_PAGES);

    for(i = 0;i < PAGE_DIR_TABLE_NUM;i++)
    {
        if(page_dir_table[i].v == 1)
        {
            pg_tbl = (volatile pte_sv39 *)pte_common_ppn_to_addr((volatile pte_64model *)&page_dir_table[i]);

            for(j = k = 0;j < PAGE_TABLE_ITEM_NUM;j++)
            {
                if(pg_tbl[j].v == 1)
                {
                    k++;
                }
            }

            printk("Pg-dir[%d] uses %d pages\n",i,k);
        }
    }
}