/*
 * 'tty.h' defines some structures used by tty_io.c and some defines.
 *
 * NOTE! Don't touch this without checking that nothing in rs_io.s or
 * con_io.s breaks. Some constants are hardwired into the system (mainly
 * offsets into 'tty_queue'
 */

#ifndef __TTY_H__
#define __TTY_H__

    #include "termios.h"

    #define TTY_BUF_SIZE 1024

    struct tty_queue 
    {
	    uint data;
	    uint head;
	    uint tail;
	    struct task_struct *proc_list;
	    char buf[TTY_BUF_SIZE];
    };

    #define INC(a) ((a) = ((a) + 1) & (TTY_BUF_SIZE - 1))
    #define DEC(a) ((a) = ((a) - 1) & (TTY_BUF_SIZE - 1))
    #define EMPTY(a) ((a).head == (a).tail)
    #define LEFT(a) (((a).tail - (a).head - 1) & (TTY_BUF_SIZE - 1))
    #define LAST(a) ((a).buf[(TTY_BUF_SIZE - 1) & ((a).head - 1)])
    #define FULL(a) (!LEFT(a))
    #define CHARS(a) (((a).head - (a).tail) & (TTY_BUF_SIZE - 1))
    #define GETCH(queue,c) \
        do{c = (queue).buf[(queue).tail];INC((queue).tail);}while(0)
    #define PUTCH(c,queue) \
        do{(queue).buf[(queue).head] = (c);INC((queue).head);}while(0)

    #define INTR_CHAR(tty) ((tty) -> termios.c_cc[VINTR])
    #define QUIT_CHAR(tty) ((tty) -> termios.c_cc[VQUIT])
    #define ERASE_CHAR(tty) ((tty) -> termios.c_cc[VERASE])
    #define KILL_CHAR(tty) ((tty) -> termios.c_cc[VKILL])
    #define EOF_CHAR(tty) ((tty) -> termios.c_cc[VEOF])
    #define START_CHAR(tty) ((tty) -> termios.c_cc[VSTART])
    #define STOP_CHAR(tty) ((tty) -> termios.c_cc[VSTOP])
    #define SUSPEND_CHAR(tty) ((tty) -> termios.c_cc[VSUSP])

    struct tty_struct 
    {
	    struct termios termios;
	    int pgrp;
	    int stopped;
	    void (*write)(struct tty_struct *tty);
	    struct tty_queue read_q;
	    struct tty_queue write_q;
	    struct tty_queue secondary;
	};

    extern struct tty_struct tty_table[];
    extern struct tty_queue *table_list[];

    /*	intr=^C		quit=^|		erase=del	kill=^U
	    eof=^D		vtime=\0	vmin=\1		sxtc=\0
	    start=^Q	stop=^S		susp=^Z		eol=\0
	    reprint=^R	discard=^U	werase=^W	lnext=^V
	    eol2=\0
    */
    #define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
    //#define INIT_C_CC "\003\034\010\025\004\0\1\0\021\023\032\0\022\017\027\026\0"

    void rs_init(void);
    void tty_init(void);

    int64_t tty_read(ulong channel,char *buf,int64_t nr);
    int64_t tty_write(ulong channel,char *buf,int64_t nr);

    void rs_write(struct tty_struct * tty);

    void copy_to_cooked(struct tty_struct * tty);

#endif