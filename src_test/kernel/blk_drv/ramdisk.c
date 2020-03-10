#include "common.h"

#include "linux/config.h"
#include "linux/sched.h"

#include "linux/fs.h"
#include "linux/kernel.h"

#define MAJOR_NR MAJOR_NR_RAMDISK
#include "blk.h"

char *rd_start;
ulong rd_length = 0;

void do_rd_request()
{
    ulong len;
    char *addr;

    INIT_REQUEST;
    addr = rd_start + (CURRENT -> sector << 9);
    len = CURRENT -> nr_sectors << 9;

    if((MINOR(CURRENT -> dev) != MAJOR_NR) || ((addr + len) > (rd_start + rd_length)))
    {
        end_request(0);
        goto repeat;
    }

    if(CURRENT -> cmd == WRITE)
    {
        memcpy(addr,CURRENT -> buffer,len);
    }
    else if(CURRENT -> cmd == READ)
    {
        memcpy(CURRENT -> buffer,addr,len);
    }
    else
    {
        panic("unknown ramdisk-command");
    }

    end_request(1);
    goto repeat;
}

ulong rd_init(ulong mem_start,ulong length)
{
    ulong i;

    blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;
    rd_start = (char *)mem_start;
    rd_length = length;
    //don't zero ramdisk because this is copied by bootloader from external flash
    return length;
}

void rd_load()
{
    struct buffer_head *bh;
    struct super_block *s;

    if(!rd_length)
    {
        return;
    }

    printk("Ram disk: %d bytes,starting at %p...",rd_length,rd_start);

    s = (struct super_block *)(rd_start + 1024);

    if(s -> s_magic != SUPER_MAGIC)
    {
        panic("No root filesystem!");
    }

    printk("done\r\n");
    //ROOT_DEV = 0x0101;
}