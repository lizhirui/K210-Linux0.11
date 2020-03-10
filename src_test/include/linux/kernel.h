#ifndef __KERNEL_H__

    volatile void panic(const char * str);
    //int printf(const char * fmt, ...);
    int printk(const char * fmt, ...);

    #define suser() (current -> euid == 0)

#endif