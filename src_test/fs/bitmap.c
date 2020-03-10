#include "common.h"
#include <linux/sched.h>
#include <linux/kernel.h>

#define BLOCK_SIZE_BITS 13

static void clear_block(ulong addr)
{
    for(ulong i = 0;i < BLOCK_SIZE;i += sizeof(ulong))
    {
        *((ulong *)(addr + i)) = 0x00;
    }
}

static ulong set_bit(ulong nr,ulong addr)
{
    addr += (nr >> 3);
    ulong bit = nr & 0x07;
    ulong old = ((*((ulong *)addr)) >> bit) & 0x01;
    *((ulong *)addr) = *((ulong *)addr) | (1 << bit);
    return old;
}

static ulong clear_bit(ulong nr,ulong addr)
{
    addr += (nr >> 3);
    ulong bit = nr & 0x07;
    ulong old = (~((*((ulong *)addr)) >> bit)) & 0x01;
    *((ulong *)addr) = *((ulong *)addr) & (~(1 << bit));
    return old;
}

static ulong find_first_zero(ulong addr)
{
    ulong i;

    for(i = 0;i < BLOCK_SIZE;i += sizeof(ulong))
    {
        ulong v = ~(*((ulong *)addr));

        if(v != 0)
        {
            for(ulong j = 0;j < (sizeof(ulong) << 3);j++)
            {
                if(v & 0x01)
                {
                    return (i << 3) + j;
                }

                v >>= 1;
            }
        }
    }

    return i << 3;
}

void free_block(int dev,int block)
{
    struct super_block *sb;
    struct buffer_head *bh;

    if(!(sb = get_super(dev)))
    {
        panic("trying to free block on nonexistent device");
    }

    if((block < sb -> s_firstdatazone) || (block >= (sb -> s_nzones)))
    {
        panic("trying to free block not in datazone");
    }

    bh = get_hash_table(dev,block);

    if(bh)
    {
        if(bh -> b_count > 1)
        {
            printk("trying to free block (%04x:%d), count=%d\n",dev,block,bh->b_count);
            brelse(bh);
            return;
        }

        bh -> b_dirt = 0;
        bh -> b_uptodate = 0;
        brelse(bh);
    }

    //calculate data block number(from no.1)
    block -= sb -> s_firstdatazone - 1;

    //bit address:1 block = 1024Bytes = 8192Bits
    if(clear_bit(block & (((1 << BLOCK_SIZE_BITS)) - 1),sb -> s_zmap[(block >> BLOCK_SIZE_BITS)] -> b_data))
    {
        printk("block (%04x:%d) ",dev,block + sb -> s_firstdatazone - 1);
        panic("free_block: bit already cleared");
    }

    sb -> s_zmap[block >> (BLOCK_SIZE_BITS)] -> b_dirt = 1;
}

int new_block(int dev)
{
    struct buffer_head *bh;
    struct super_block *sb;
    int i,j;

    if(!(sb = get_super(dev)))
    {
        panic("trying to get new block from nonexistent device");
    }

    j = 8192;

    for(i = 0;i < Z_MAP_SLOTS;i++)
    {
        if(bh = sb -> s_zmap[i])
        {
            if((j = find_first_zero(bh -> b_data)) < ((1 << BLOCK_SIZE_BITS)))
            {
                break;
            }
        }
    }

    if((i >= Z_MAP_SLOTS) || (!bh) || (j >= ((1 << BLOCK_SIZE_BITS))))
    {
        return 0;
    }

    if(set_bit(j,bh -> b_data))
    {
        panic("new_block: bit already set");
    }

    bh -> b_dirt = 1;
    //calculate data block number
    j += ((i << BLOCK_SIZE_BITS)) + sb -> s_firstdatazone - 1;

    if(j >= sb -> s_nzones)
    {
        return 0;
    }

    if(!(bh = getblk(dev,j)))
    {
        panic("new_block: cannot get block");
    }

    if(bh -> b_count != 1)
    {
        panic("new block: count is != 1");
    }

    clear_block(bh -> b_data);
    bh -> b_uptodate = 1;
    bh -> b_dirt = 1;
    brelse(bh);
    return j;
}

void free_inode(struct m_inode *inode)
{
    struct super_block *sb;
    struct buffer_head *bh;

    if(!inode)
    {
        return;
    }

    if(!inode -> i_dev)
    {
        memset(inode,0,sizeof(*inode));
        return;
    }

    if(inode -> i_count > 1)
    {
        printk("trying to free inode with count=%d\r\n",inode -> i_count);
        panic("free_inode");
    }

    if(inode -> i_nlinks)
    {
        panic("trying to free inode with links");
    }

    if(!(sb = get_super(inode -> i_dev)))
    {
        panic("trying to free inode on nonexistent device");
    }

    if((inode -> i_num < 1) || (inode -> i_num > sb -> s_ninodes))
    {
        panic("trying to free inode 0 or nonexistent inode");
    }

    if(!(bh = sb -> s_imap[(inode -> i_num >> BLOCK_SIZE_BITS)]))
    {
        panic("nonexistent imap in superblock");
    }

    if(clear_bit(inode -> i_num & ((1 << BLOCK_SIZE_BITS)),bh -> b_data))
    {
        printk("free_inode: bit already cleared.\r\n");
    }

    bh -> b_dirt = 1;
    memset(inode,0,sizeof(*inode));
}

struct m_inode *new_inode(int dev)
{
    struct m_inode *inode;
    struct super_block *sb;
    struct buffer_head *bh;
    int i,j;

    if(!(inode = get_empty_inode()))
    {
        return NULL;
    }

    if(!(sb = get_super(dev)))
    {
        panic("new_inode with unknown device");
    }

    j = (1 << BLOCK_SIZE_BITS);

    for(i = 0;i < I_MAP_SLOTS;i++)
    {
        if(bh = sb -> s_imap[i])
        {
            if((j = find_first_zero(bh -> b_data)) < ((1 << BLOCK_SIZE_BITS)))
            {
                break;
            }
        }
    }

    if((!bh) || (j >= ((1 << BLOCK_SIZE_BITS))) || ((j + ((i << BLOCK_SIZE_BITS))) > sb -> s_ninodes))
    {
        iput(inode);
        return NULL;
    }

    if(set_bit(j,bh -> b_data))
    {
        panic("new_inode: bit already set");
    }

    bh -> b_dirt = 1;
    inode -> i_count = 1;
    inode -> i_nlinks = 1;
    inode -> i_dev = dev;
    inode -> i_uid = current -> euid;
    inode -> i_gid = current -> egid;
    inode -> i_dirt = 1;
    inode -> i_num = j + ((i << BLOCK_SIZE_BITS));
    inode -> i_mtime = inode -> i_atime = inode -> i_ctime = CURRENT_TIME;
    return inode;
}