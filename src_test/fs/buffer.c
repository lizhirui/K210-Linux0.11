#include "common.h"

#include "linux/config.h"
#include "linux/sched.h"
#include "linux/kernel.h"

extern ulong _buffer_start;
extern ulong _buffer_end;
struct buffer_head *start_buffer = (struct buffer_head *)&_buffer_start;
struct buffer_head *hash_table[NR_HASH];
static struct buffer_head *free_list;
static struct task_struct *buffer_wait = NULL;

int NR_BUFFERS = 0;

static inline void wait_on_buffer(struct buffer_head *bh)
{
    sysctl_disable_irq();

    while(bh -> b_lock)
    {
        sleep_on(&bh -> b_wait);
    }

    sysctl_enable_irq();
}

int64_t sys_sync()
{
    int i;
    struct buffer_head *bh;

    sync_inodes();
    bh = start_buffer;

    for(i = 0;i < NR_BUFFERS;i++,bh++)
    {
        wait_on_buffer(bh);

        if(bh -> b_dirt)
        {
            ll_rw_block(WRITE,bh);
        }
    }

    return 0;
}

int sync_dev(int dev)
{
    int i;
    struct buffer_head *bh;

    bh = start_buffer;

    for(i = 0;i < NR_BUFFERS;i++,bh++)
    {
        if(bh -> b_dev != dev)
        {
            continue;
        }

        wait_on_buffer(bh);

        if((bh -> b_dev == dev) && (bh -> b_dirt))
        {
            ll_rw_block(WRITE,bh);
        }
    }

    sync_inodes();

    bh = start_buffer;

    for(i = 0;i < NR_BUFFERS;i++,bh++)
    {
        if(bh -> b_dev != dev)
        {
            continue;
        }

        wait_on_buffer(bh);

        if((bh -> b_dev == dev) && (bh -> b_dirt))
        {
            ll_rw_block(WRITE,bh);
        }
    }

    return 0;
}

inline void invalidate_buffers(int dev)
{
    int i;
    struct buffer_head *bh;

    bh = start_buffer;

    for(i = 0;i < NR_BUFFERS;i++,bh++)
    {
        if(bh -> b_dev != dev)
        {
            continue;
        }

        wait_on_buffer(bh);

        if(bh -> b_dev == dev)
        {
            bh -> b_uptodate = bh -> b_dirt = 0;
        }
    }
}

#define _hashfn(dev,block) (((uint16_t)(dev ^ block)) % NR_HASH)
#define hash(dev,block) hash_table[_hashfn(dev,block)]

static inline void remove_from_queues(struct buffer_head *bh)
{
    //remove from hash-queue
    if(bh -> b_next)
    {
        bh -> b_next -> b_prev = bh -> b_prev;
    }

    if(bh -> b_prev)
    {
        bh -> b_prev -> b_next = bh -> b_next;
    }

    if(hash(bh -> b_dev,bh -> b_blocknr) == bh)
    {
        hash(bh -> b_dev,bh -> b_blocknr) = bh -> b_next;
    }

    //remove from free list
    if(!(bh -> b_prev_free) || (!(bh -> b_next_free)))
    {
        panic("Free block list corrupted");
    }

    bh -> b_prev_free -> b_next_free = bh -> b_next_free;
    bh -> b_next_free -> b_prev_free = bh -> b_prev_free;

    if(free_list == bh)
    {
        free_list = bh -> b_next_free;
    }
}

static inline void insert_into_queues(struct buffer_head *bh)
{
    //put at end of free list
    bh -> b_next_free = free_list;
    bh -> b_prev_free = free_list -> b_prev_free;
    free_list -> b_prev_free -> b_next_free = bh;
    free_list -> b_prev_free = bh;

    //put the buffer in new hash-queue if it has a device
    bh ->  b_prev = NULL;
    bh ->  b_next = NULL;

    if(!bh -> b_dev)
    {
        return;
    }

    bh -> b_next = hash(bh -> b_dev,bh -> b_blocknr);
    hash(bh -> b_dev,bh -> b_blocknr) = bh;

    if(bh -> b_next)
    {
        bh -> b_next -> b_prev = bh;
    }
}

static struct buffer_head *find_buffer(int dev,int block)
{
    struct buffer_head *tmp;

    for(tmp = hash(dev,block);tmp != NULL;tmp = tmp -> b_next)
    {
        if((tmp -> b_dev == dev) && (tmp -> b_blocknr == block))
        {
            return tmp;
        }
    }

    return NULL;
}

struct buffer_head *get_hash_table(int dev,int block)
{
    struct buffer_head *bh;

    while(1)
    {
        if(!(bh = find_buffer(dev,block)))
        {
            return NULL;
        }

        bh -> b_count++;
        wait_on_buffer(bh);

        if((bh -> b_dev == dev) && (bh -> b_blocknr == block))
        {
            return bh;
        }

        bh -> b_count--;
    }
}

#define BADNESS(bh) ((((bh) -> b_dirt) << 1) + (bh) -> b_lock)

//this is getblk,and it isn't very clear,again to hinder race-conditions.
//Most of the code is seldom used,(ie repeating),
//so it should be much more efficient that it looks
struct buffer_head *getblk(int dev,int block)
{
    struct buffer_head *tmp,*bh;

    repeat:
        if(bh = get_hash_table(dev,block))
        {
            return bh;
        }

        tmp = free_list;

        do
        {
            if(tmp -> b_count)
            {
                continue;
            }

            if((!bh) || (BADNESS(tmp) < BADNESS(bh)))
            {
                bh = tmp;

                if(!BADNESS(tmp))
                {
                    break;
                }
            }
            //repeat until we find something good
        }while((tmp = tmp -> b_next_free) != free_list);

        if(!bh)
        {
            sleep_on(&buffer_wait);
            goto repeat;
        }

        wait_on_buffer(bh);

        if(bh -> b_count)
        {
            goto repeat;
        }

        while(bh -> b_dirt)
        {
            sync_dev(bh -> b_dev);
            wait_on_buffer(bh);

            if(bh -> b_count)
            {
                goto repeat;
            }
        }

        //NOTE!!While we slept waiting for this block,somebody else might already have added "this" block to the cache,check it
        if(find_buffer(dev,block))
        {
            goto repeat;
        }

    //OK,FINALLY we know that this buffer is the only one of it's kind,
    //and that it's unused(b_count = 0),unlocked(b_lock = 0),and clean
    bh -> b_count = 1;
    bh -> b_dirt = 0;
    bh -> b_uptodate = 0;
    remove_from_queues(bh);
    bh -> b_dev = dev;
    bh -> b_blocknr = block;
    insert_into_queues(bh);
    return bh;
}

void brelse(struct buffer_head *buf)
{
    if(!buf)
    {
        return;
    }

    wait_on_buffer(buf);

    if(!(buf -> b_count--))
    {
        panic("Trying to free free buffer");
    }

    wake_up(&buffer_wait);
}

//bread() reads a specified block and returns the buffer that contains it.It returns NULL if the block was unreadable
struct buffer_head *bread(int dev,int block)
{
    struct buffer_head *bh;
    
    if(!(bh = getblk(dev,block)))
    {
        panic("bread: getblk returned NULL\r\n");
    }

    if(bh -> b_uptodate)
    {
        return bh;
    }
    
    ll_rw_block(READ,bh);
    wait_on_buffer(bh);

    if(bh -> b_uptodate)
    {
        return bh;
    }

    brelse(bh);
    return NULL;
}

void COPYBLK(ulong from,ulong to)
{
    ulong i;

    for(i = 0;i < BLOCK_SIZE;i += sizeof(ulong))
    {
        *((ulong *)(to + i)) = *((ulong *)(from + i));
    }
}

//bread_page reads four buffers into memory at the desired address.
//It's a function of its own,as there is some speed to be got by reading
//them all at the same time,not waiting for one to be read,and then another etc.
//only used by function "do_no_page" in "mm/memory.c"
void bread_page(ulong address,int dev,int b[4])
{
    struct buffer_head *bh[4];
    int i;

    for(i = 0;i < 4;i++)
    {
        if(b[i])
        {
            if(bh[i] = getblk(dev,b[i]))
            {
                if(!bh[i] -> b_uptodate)
                {
                    ll_rw_block(READ,bh[i]);
                }
            }
        }
        else
        {
            bh[i] = NULL;
        }
    }

    for(i = 0;i < 4;i++,address += BLOCK_SIZE)
    {
        if(bh[i])
        {
            wait_on_buffer(bh[i]);

            if(bh[i] -> b_uptodate)
            {
                COPYBLK((ulong)bh[i] -> b_data,address);
            }
            
            brelse(bh[i]);
        }
    }

    //syslog_print("execute ok!\r\n");
}

//OK,breada can be used as bread,but additionally to mark other blocks for reading as well.
//End the argument list with a negative number.
struct buffer_head *breada(int dev,int first,...)
{
    va_list args;
    struct buffer_head *bh,*tmp;

    va_start(args,first);

    if(!(bh = getblk(dev,first)))
    {
        panic("bread:getblk returned NULl\r\n");
    }

    if(!bh -> b_uptodate)
    {
        ll_rw_block(READ,bh);
    }

    while((first = va_arg(args,int)) >= 0)
    {
        tmp = getblk(dev,first);

        if(tmp)
        {
            if(!tmp -> b_uptodate)
            {
                ll_rw_block(READA,tmp);
            }

            tmp -> b_count--;//这里为何不使用brelse释放
        }
    }

    va_end(args);
    wait_on_buffer(bh);

    if(bh -> b_uptodate)
    {
        return bh;
    }

    brelse(bh);
    return NULL;
}

void buffer_init(ulong buffer_end)
{
    struct buffer_head *h = start_buffer;
    uint8_t *b;
    int i;

    b = (void *)buffer_end;

    while((b -= BLOCK_SIZE) >= ((uint8_t *)(h + 1)))
    {
        h -> b_dev = 0;
        h -> b_dirt = 0;
        h -> b_count = 0;
        h -> b_lock = 0;
        h -> b_uptodate = 0;
        h -> b_wait = NULL;
        h -> b_next = NULL;
        h -> b_prev = NULL;
        h -> b_data = (char *)b;
        h -> b_prev_free = h - 1;
        h -> b_next_free = h + 1;
        h++;
        NR_BUFFERS++;
    }

    h--;
    free_list = start_buffer;
    free_list -> b_prev_free = h;
    h -> b_next_free = free_list;

    for(i = 0;i < NR_HASH;i++)
    {
        hash_table[i] = NULL;
    }
}