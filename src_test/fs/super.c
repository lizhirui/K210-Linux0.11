#include "common.h"

#include <linux/config.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <errno.h>
#include <sys/stat.h>

#define BLOCK_SIZE_BITS 13

int sync_dev(int dev);

void wait_for_keypress();

static ulong set_bit(ulong bitnr,ulong addr)
{
	addr += (bitnr >> 3);
    ulong bit = bitnr & 0x07;
    ulong old = ((*((uint8_t *)addr)) >> bit) & 0x01;
    return old;
}

struct super_block super_block[NR_SUPER];

int ROOT_DEV = 0;

static void lock_super(struct super_block *sb)
{
    sysctl_disable_irq();
    
    while(sb -> s_lock)
    {
        sleep_on(&(sb -> s_wait));
    }

    sb -> s_lock = 1;
    sysctl_enable_irq();
}

static void free_super(struct super_block *sb)
{
    sysctl_disable_irq();
    sb -> s_lock = 0;
    wake_up(&(sb -> s_wait));
    sysctl_enable_irq();
}

static void wait_on_super(struct super_block *sb)
{
    sysctl_disable_irq();
    
    while(sb -> s_lock)
    {
        sleep_on(&(sb -> s_wait));
    }

    sysctl_enable_irq();
}

struct super_block *get_super(int dev)
{
    struct super_block *s;

    if(!dev)
    {
        return NULL;
    }

    s = 0 + super_block;

    while(s < (NR_SUPER + super_block))
    {
        if(s -> s_dev == dev)
        {
            wait_on_super(s);

            if(s -> s_dev == dev)
            {
                return s;
            }

            s = 0 + super_block;
        }
        else
        {
            s++;
        }
    }

    return NULL;
}

void put_super(int dev)
{
    struct super_block *sb;
    struct m_inode *inode;
    int i;

    if(dev == ROOT_DEV)
    {
        printk("root diskette changed: prepare for armageddon\r\n");
        return;
    }

    if(!(sb = get_super(dev)))
    {
        return;
    }

    if(sb -> s_imount)
    {
        printk("Mounted disk changed - tssk,tssk\r\n");
        return;
    }

    lock_super(sb);
    sb -> s_dev = 0;

    for(i = 0;i < I_MAP_SLOTS;i++)
    {
        brelse(sb -> s_imap[i]);
    }

    for(i = 0;i < Z_MAP_SLOTS;i++)
    {
        brelse(sb -> s_zmap[i]);
    }

    free_super(sb);
    return;
}

static struct super_block *read_super(int dev)
{
    struct super_block *s;
    struct buffer_head *bh;
    int i,block;

    if(!dev)
    {
        return NULL;
    }

    if(s = get_super(dev))
    {
        return s;
    }
    
    for(s = 0 + super_block;;s++)
    {
        if(s >= (NR_SUPER + super_block))
        {
            return NULL;
        }

        if(!s -> s_dev)
        {
            break;
        }
    }

    s -> s_dev = dev;
    s -> s_isup = NULL;
    s -> s_imount = NULL;
    s -> s_time = 0;
    s -> s_rd_only = 0;
    s -> s_dirt = 0;

    lock_super(s);
    
    if(!(bh = bread(dev,1)))
    {
        s -> s_dev = 0;
        free_super(s);
        return NULL;
    }

    *((struct d_super_block *)s) = *((struct d_super_block *)bh -> b_data);
    brelse(bh);

    if(s -> s_magic != SUPER_MAGIC)
    {
        s -> s_dev = 0;
        free_super(s);
        return NULL;
    }

    for(i = 0;i < I_MAP_SLOTS;i++)
    {
        s -> s_imap[i] = NULL;
    }

    for(i = 0;i < Z_MAP_SLOTS;i++)
    {
        s -> s_zmap[i] = NULL;
    }

    block = 2;
    
    for(i = 0;i < s -> s_imap_blocks;i++)
    {
        if(s -> s_imap[i] = bread(dev,block))
        {
            block++;
        }
        else
        {
            break;
        }
    }

    for(i = 0;i < s -> s_zmap_blocks;i++)
    {
        if(s -> s_zmap[i] = bread(dev,block))
        {
            block++;
        }
        else
        {
            break;
        }
    }

    if(block != (2 + s -> s_imap_blocks + s -> s_zmap_blocks))
    {
        for(i = 0;i < I_MAP_SLOTS;i++)
        {
            brelse(s -> s_imap[i]);
        }

        for(i = 0;i < Z_MAP_SLOTS;i++)
        {
            brelse(s -> s_zmap[i]);
        }

        s -> s_dev = 0;
        free_super(s);
        return NULL;
    }

    s -> s_imap[0] -> b_data[0] |= 1;
    s -> s_zmap[0] -> b_data[0] |= 1;
    free_super(s);
    return s;
}

int64_t sys_umount(char *dev_name)
{
    struct m_inode *inode;
    struct super_block *sb;
    int dev;

    if(!(inode = namei(dev_name)))
    {
        return -ENOENT;
    }

    dev = inode -> i_zone[0];

    if(!S_ISBLK(inode -> i_mode))
    {
        iput(inode);
        return -ENOTBLK;
    }

    iput(inode);

    if(dev == ROOT_DEV)
    {
        return -EBUSY;
    }

    if(!(sb = get_super(dev)) || !(sb -> s_imount))
    {
        return -ENOENT;
    }

    if(!sb -> s_imount -> i_mount)
    {
        printk("Mounted inode has i_mount = 0\r\n");
    }

    for(inode = inode_table + 0;inode < inode_table + NR_INODE;inode++)
    {
        if((inode -> i_dev == dev) && (inode -> i_count))
        {
            return -EBUSY;
        }
    }

    sb -> s_imount -> i_mount = 0;
    iput(sb -> s_imount);
    sb -> s_imount = NULL;
    iput(sb -> s_isup);
    put_super(dev);
    sync_dev(dev);
    return 0;
}

int64_t sys_mount(char *dev_name,char *dir_name,int rw_flag)
{
    struct m_inode *dev_i,*dir_i;
    struct super_block *sb;
    int dev;

    if(!(dev_i = namei(dev_name)))
    {
        return -ENOENT;
    }

    dev = dev_i -> i_zone[0];

    if(!S_ISBLK(dev_i -> i_mount))
    {
        iput(dev_i);
        return -EPERM;
    }

    iput(dev_i);

    if(!(dir_i = namei(dir_name)))
    {
        return -ENOENT;
    }

    if((dir_i -> i_count != 1) || (dir_i -> i_num = ROOT_INO))
    {
        iput(dir_i);
        return -EBUSY;
    }

    if(!S_ISDIR(dir_i -> i_mode))
    {
        iput(dir_i);
        return -EPERM;
    }

    if(!(sb = read_super(dev)))
    {
        iput(dir_i);
        return -EBUSY;
    }

    if(sb -> s_imount)
    {
        iput(dir_i);
        return -EBUSY;
    }

    if(dir_i -> i_mount)
    {
        iput(dir_i);
        return -EPERM;
    }

    sb -> s_imount = dir_i;
    dir_i -> i_mount = 1;
    dir_i -> i_dirt = 1;
    return 0;
}

void mount_root()
{
    int i,free;
    struct super_block *p;
    struct m_inode *mi;

    if(32 != sizeof(struct d_inode))
    {
        panic("bad i-node size");
    }

    for(i = 0;i < NR_FILE;i++)
    {
        file_table[i].f_count = 0; 
    }

    for(p = &super_block[0];p < &super_block[NR_SUPER];p++)
    {
        p -> s_dev = 0;
        p -> s_lock = 0;
        p -> s_wait = NULL;
    }

    if(!(p = read_super(ROOT_DEV)))
    {
        panic("Unable to mount root");
    }

    syslog_print("mount_root:read_super ok\r\n");

    if(!(mi = iget(ROOT_DEV,ROOT_INO)))
    {
        panic("Unable to read root i-node");
    }

    syslog_print("mount_root:iget ok\r\n");

    mi -> i_count += 3;
    p -> s_isup = p -> s_imount = mi;
    current -> pwd = mi;
    current -> root = mi;
    free = 0;
    i = p -> s_nzones;

    while(--i > 0)
    {
        if(!set_bit(i & ((1 << BLOCK_SIZE_BITS) - 1),(ulong)(p -> s_zmap[(i >> BLOCK_SIZE_BITS)] -> b_data)))
        {
            free++;
        }
    }

    printk("%d/%d free blocks\r\n",free,p -> s_nzones);
    free = 0;
    i = p -> s_ninodes + 1;

    while(--i >= 0)
    {
        if(!set_bit(i & ((1 << BLOCK_SIZE_BITS) - 1),(ulong)(p -> s_imap[(i >> BLOCK_SIZE_BITS)] -> b_data)))
        {
            free++;
        }
    }

    printk("%d/%d free inodes\r\n",free,p -> s_ninodes);
}