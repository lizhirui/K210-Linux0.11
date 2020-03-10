#include "common.h"
#include "linux/sched.h"

volatile pte_sv39 page_root_table[PAGE_ROOT_TABLE_NUM] ALIGN4K;
volatile pte_sv39 *page_dir_table;
//volatile pte_sv39 page_table[PAGE_TABLE_NUM][PAGE_TABLE_ITEM_NUM] ALIGN4K;