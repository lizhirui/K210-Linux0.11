#ifndef __BLK_H__
#define __BLK_H__

    #define NR_BLK_DEV 7

    //NR_REQUEST is the number of entries in the request-queue.
    //NOTE that writes may use only the low 2/3 of these:reads take precedence
    //32 seems to be a reasonable number:enough to get some benefit from the elevator-mechanism,
    //but not so much as to lock a lot of buffers when they are in the queue.
    //64 seems to be too many(easily long pauses in reading when heavy writing/syncing is going on)
    #define NR_REQUEST 32

    //this is an expanded form so that we can use the same request for paging requests when this is implemented.
    //In paging,'bh' is NULL,and 'waiting' is used to wait for read/write completion
    struct request
    {
        int dev;//-1 if no request
        int cmd;//READ or WRITE
        int errors;
        ulong sector;
        ulong nr_sectors;
        char *buffer;
        struct task_struct *waiting;
        struct buffer_head *bh;
        struct request *next;
    };

    //This is used in the elevator algorithm:
    //Note that reads always go before writes.
    //This is natural:reads are much more time-critical than writes
    #define IN_ORDER(s1,s2) \
        ((((s1) -> cmd < (s2) -> cmd)) || (((s1) -> cmd == (s2) -> cmd) && \
        (((s1) -> dev < (s2) -> dev) || (((s1) -> dev == (s2) -> dev) && \
        ((s1) -> sector < (s2) -> sector)))))

    struct blk_dev_struct
    {
        void (*request_fn)();
        struct request *current_request;
    };

    extern struct blk_dev_struct blk_dev[NR_BLK_DEV];
    extern struct request request[NR_REQUEST];
    extern struct task_struct *wait_for_request;

    #define MAJOR_NR_RAMDISK 1

    #ifdef MAJOR_NR
        //Add entries as needed.Currently the only block devices
        //supported are ram-disks

        #if(MAJOR_NR == MAJOR_NR_RAMDISK)
            #define DEVICE_NAME "ramdisk"
            #define DEVICE_REQUEST do_rd_request
            #define DEVICE_NR(device) ((device) & 7)
            #define DEVICE_ON(device)
            #define DEVICE_OFF(device)
        #elif
            #error "unknown blk device"
        #endif

        #define CURRENT (blk_dev[MAJOR_NR].current_request)
        #define CURRENT_DEV DEVICE_NR(CURRENT -> dev)

        static void (DEVICE_REQUEST)();

        extern inline void unlock_buffer(struct buffer_head *bh)
        {
            if(!bh -> b_lock)
            {
                printk(DEVICE_NAME ": free buffer being unlocked\n");
            }

            bh -> b_lock = 0;
            wake_up(&bh -> b_wait);
        }

        extern inline void end_request(int uptodate)
        {
            DEVICE_OFF(CURRENT -> dev)

            if(CURRENT -> bh)
            {
                CURRENT -> bh -> b_uptodate = uptodate;
                unlock_buffer(CURRENT -> bh);
            }

            if(!uptodate)
            {
                printk(DEVICE_NAME " I/O error\r\n");
                printk("dev %04x,block %d\r\n",CURRENT -> dev,CURRENT -> bh -> b_blocknr);
            }

            wake_up(&CURRENT -> waiting);
            wake_up(&wait_for_request);
            CURRENT -> dev = -1;
            CURRENT = CURRENT -> next;
        }

        #define INIT_REQUEST \
            repeat:\
                if(!CURRENT) \
                    return; \
                \
                if(MAJOR(CURRENT -> dev) != MAJOR_NR) \
                { \
                    panic(DEVICE_NAME ": request list destroyed"); \
                } \
                \
                if(CURRENT -> bh) \
                { \
                    if(!CURRENT -> bh -> b_lock) \
                    { \
                        panic(DEVICE_NAME ": block not locked"); \
                    } \
                } \

    #endif
        
#endif