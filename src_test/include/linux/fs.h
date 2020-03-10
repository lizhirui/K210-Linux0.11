/*
 * This file has definitions for some important file table
 * structures etc.
 */

#ifndef __FS_H__
#define __FS_H__

    #include <sys/types.h>

    /* devices are as follows: (same as minix, so we can use the minix
     * file system. These are major numbers.)
     *
     * 0 - unused (nodev)
     * 1 - /dev/mem
     * 2 - /dev/fd(actually unused for riscv)
     * 3 - /dev/hd(actually unused for riscv)
     * 4 - /dev/ttyx
     * 5 - /dev/tty(actually unused for riscv)
     * 6 - /dev/lp(actually unused for riscv)
     * 7 - unnamed pipes
     */

    #define IS_SEEKABLE(x) ((x)>=1 && (x)<=3)

    #define READ 0
    #define WRITE 1
    #define READA 2		/* read-ahead - don't pause */
    #define WRITEA 3	/* "write-ahead" - silly, but somewhat useful */

    void buffer_init(ulong buffer_end);

    #define MAJOR(a) (((unsigned)(a))>>8)
    #define MINOR(a) ((a)&0xff)

    #define NAME_LEN 14
    #define ROOT_INO 1

    #define I_MAP_SLOTS 8
    #define Z_MAP_SLOTS 8
    #define SUPER_MAGIC 0x137F

    #define NR_OPEN 20
    #define NR_INODE 32
    #define NR_FILE 64
    #define NR_SUPER 8
    #define NR_HASH 307
    #define NR_BUFFERS nr_buffers
    #define BLOCK_SIZE 1024
    #define BLOCK_SIZE_BITS 10
    #ifndef NULL
    #define NULL ((void *) 0)
    #endif

    #define INODES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct d_inode)))
    #define DIR_ENTRIES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct dir_entry)))

    #define PIPE_HEAD(inode) ((inode).i_zone[0])
    #define PIPE_TAIL(inode) ((inode).i_zone[1])
    #define PIPE_SIZE(inode) ((PIPE_HEAD(inode)-PIPE_TAIL(inode))&(PAGE_SIZE-1))
    #define PIPE_EMPTY(inode) (PIPE_HEAD(inode)==PIPE_TAIL(inode))
    #define PIPE_FULL(inode) (PIPE_SIZE(inode)==(PAGE_SIZE-1))
    #define INC_PIPE(head) \
    __asm__("incl %0\n\tandl $4095,%0"::"m" (head))

    typedef char buffer_block[BLOCK_SIZE];

    struct buffer_head 
    {
	    char *b_data;			/* pointer to data block (1024 bytes) */
	    ulong b_blocknr;	/* block number */
	    uint16_t b_dev;		/* device (0 = free) */
	    uint8_t b_uptodate;
	    uint8_t b_dirt;		/* 0-clean,1-dirty */
	    uint8_t b_count;		/* users using this block */
	    uint8_t b_lock;		/* 0 - ok, 1 -locked */
	    struct task_struct *b_wait;
	    struct buffer_head *b_prev;
	    struct buffer_head *b_next;
	    struct buffer_head *b_prev_free;
	    struct buffer_head *b_next_free;
    };

    struct d_inode 
    {
	    uint16_t i_mode;
	    uint16_t i_uid;
	    uint32_t i_size;
	    uint32_t i_time;
	    uint8_t i_gid;
	    uint8_t i_nlinks;
	    uint16_t i_zone[9];
    };

    struct m_inode 
    {
	    uint16_t i_mode;
	    uint16_t i_uid;
	    uint32_t i_size;
	    uint32_t i_mtime;
	    uint8_t i_gid;
	    uint8_t i_nlinks;
	    uint16_t i_zone[9];
    /* these are in memory also */
	    struct task_struct * i_wait;
	    uint32_t i_atime;
	    uint32_t i_ctime;
	    uint16_t i_dev;
	    uint16_t i_num;
	    uint16_t i_count;
	    uint8_t i_lock;
	    uint8_t i_dirt;
	    uint8_t i_pipe;
	    uint8_t i_mount;
	    uint8_t i_seek;
	    uint8_t i_update;
    };

    struct file 
    {
	    uint16_t f_mode;
	    uint16_t f_flags;
	    uint16_t f_count;
	    struct m_inode * f_inode;
	    off_t f_pos;
    };

    struct super_block 
    {
	    uint16_t s_ninodes;
	    uint16_t s_nzones;
	    uint16_t s_imap_blocks;
	    uint16_t s_zmap_blocks;
	    uint16_t s_firstdatazone;
	    uint16_t s_log_zone_size;
	    uint s_max_size;
	    uint16_t s_magic;
    /* These are only in memory */
	    struct buffer_head * s_imap[I_MAP_SLOTS];
	    struct buffer_head * s_zmap[Z_MAP_SLOTS];
	    uint16_t s_dev;
	    struct m_inode * s_isup;
	    struct m_inode * s_imount;
	    uint32_t s_time;
	    struct task_struct * s_wait;
	    uint8_t s_lock;
	    uint8_t s_rd_only;
	    uint8_t s_dirt;
    };

    struct d_super_block 
    {
	    uint16_t s_ninodes;
	    uint16_t s_nzones;
	    uint16_t s_imap_blocks;
	    uint16_t s_zmap_blocks;
	    uint16_t s_firstdatazone;
	    uint16_t s_log_zone_size;
	    uint32_t s_max_size;
	    uint16_t s_magic;
    };

    struct dir_entry 
    {
	    uint16_t inode;
	    char name[NAME_LEN];
    };

    extern struct m_inode inode_table[NR_INODE];
    extern struct file file_table[NR_FILE];
    extern struct super_block super_block[NR_SUPER];
    extern struct buffer_head * start_buffer;
    extern int nr_buffers;
    
    extern void truncate(struct m_inode * inode);
    extern void sync_inodes(void);
    extern void wait_on(struct m_inode * inode);
    extern int bmap(struct m_inode * inode,int block);
    extern int create_block(struct m_inode * inode,int block);
    extern struct m_inode * namei(const char * pathname);
    extern int open_namei(const char * pathname, int flag, int mode,
	    struct m_inode ** res_inode);
    extern void iput(struct m_inode * inode);
    extern struct m_inode * iget(int dev,int nr);
    extern struct m_inode * get_empty_inode(void);
    extern struct m_inode * get_pipe_inode(void);
    extern struct buffer_head * get_hash_table(int dev, int block);
    extern struct buffer_head * getblk(int dev, int block);
    extern void ll_rw_block(int rw, struct buffer_head * bh);
    extern void brelse(struct buffer_head * buf);
    extern struct buffer_head * bread(int dev,int block);
    extern void bread_page(unsigned long addr,int dev,int b[4]);
    extern struct buffer_head * breada(int dev,int block,...);
    extern int new_block(int dev);
    extern void free_block(int dev, int block);
    extern struct m_inode * new_inode(int dev);
    extern void free_inode(struct m_inode * inode);
    extern int sync_dev(int dev);
    extern struct super_block * get_super(int dev);
    extern int ROOT_DEV;

    extern void mount_root(void);

#endif