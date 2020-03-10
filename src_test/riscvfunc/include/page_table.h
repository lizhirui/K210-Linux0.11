#ifndef __PAGE_TABLE_H__
#define __PAGE_TABLE_H__

    #define PAGE_ROOT_TABLE_NUM 8UL
    #define PAGE_DIR_TABLE_NUM 512UL * 4UL
    //#define PAGE_TABLE_NUM 8
    #define PAGE_TABLE_ITEM_NUM 512UL

    extern volatile pte_sv39 page_root_table[PAGE_ROOT_TABLE_NUM];
    extern volatile pte_sv39 *page_dir_table;
    //extern volatile pte_sv39 page_table[PAGE_TABLE_NUM][PAGE_TABLE_ITEM_NUM];

#endif