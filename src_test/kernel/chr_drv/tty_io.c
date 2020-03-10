#include "common.h"
#include "ctype.h"
#include "errno.h"
#include "signal.h"
#include "asm/segment.h"

#define ALRMMASK (1 << (SIGALRM - 1))
#define KILLMASK (1 << (SIGKILL - 1))
#define INTMASK (1 << (SIGINT - 1))
#define QUITMASK (1 << (SIGQUIT - 1))
#define TSIPMASK (1 << (SIGTSTP - 1))

#include "linux/sched.h"
#include "linux/tty.h"

#define _L_FLAG(tty,f) ((tty)->termios.c_lflag & f)
#define _I_FLAG(tty,f) ((tty)->termios.c_iflag & f)
#define _O_FLAG(tty,f) ((tty)->termios.c_oflag & f)

#define L_CANON(tty) _L_FLAG((tty),ICANON)
#define L_ISIG(tty) _L_FLAG((tty),ISIG)
#define L_ECHO(tty) _L_FLAG((tty),ECHO)
#define L_ECHOE(tty) _L_FLAG((tty),ECHOE)
#define L_ECHOK(tty) _L_FLAG((tty),ECHOK)
#define L_ECHOCTL(tty) _L_FLAG((tty),ECHOCTL)
#define L_ECHOKE(tty) _L_FLAG((tty),ECHOKE)

#define I_UCLC(tty) _I_FLAG((tty),IUCLC)
#define I_NLCR(tty) _I_FLAG((tty),INLCR)
#define I_CRNL(tty) _I_FLAG((tty),ICRNL)
#define I_NOCR(tty) _I_FLAG((tty),IGNCR)

#define O_POST(tty) _O_FLAG((tty),OPOST)
#define O_NLCR(tty) _O_FLAG((tty),ONLCR)
#define O_CRNL(tty) _O_FLAG((tty),OCRNL)
#define O_NLRET(tty) _O_FLAG((tty),ONLRET)
#define O_LCUC(tty) _O_FLAG((tty),OLCUC)

#define TTY_CHANNEL_NUM 1

struct tty_struct tty_table[] = 
{
    {
        {
            ICRNL,//no translation
            0,//no translation
            0,//no control flag
            ISIG | ICANON | ECHO | ECHOPRT | ECHOKE,//no local flag
            0,//tty
            INIT_C_CC
        },
        0,//pgrp
        0,//unstopped
        rs_write,//write function
        {0,0,0,0,""},//read queue
        {0,0,0,0,""},//write queue
        {0,0,0,0,""}//secondary queue
    },
};

//these are the tables used by the machine code handlers.
//you can implement pseudo-tty's or something by changing them.
//Currently not done
struct tty_queue *table_list[] = 
{
    &tty_table[0].read_q,&tty_table[0].write_q
};

void tty_init()
{
    rs_init();
}

void tty_intr(struct tty_struct *tty,int mask)
{
    int i;

    if(tty -> pgrp <= 0)
    {
        return;
    }

    for(i = 0;i < NR_TASKS;i++)
    {
        if(task[i] && (task[i] -> pgrp == tty -> pgrp))
        {
            task[i] -> signal |= mask;
        }
    }
}

static void sleep_if_empty(struct tty_queue *queue)
{
    sysctl_disable_irq();

    while((!current -> signal) && EMPTY(*queue))
    {
        interruptible_sleep_on(&queue -> proc_list);
    }
    
    sysctl_enable_irq();
}

static void sleep_if_full(struct tty_queue *queue)
{
    if(!FULL(*queue))
    {
        return;
    }

    sysctl_disable_irq();

    while((!current -> signal) && (LEFT(*queue) < 128))
    {
        interruptible_sleep_on(&queue -> proc_list);
    }
    
    sysctl_enable_irq();
}

void wait_for_keypress()
{
    sleep_if_empty(&tty_table[0].secondary);
}

void copy_to_cooked(struct tty_struct *tty)
{
    signed char c;

    while((!EMPTY(tty -> read_q)) && (!FULL(tty -> secondary)))
    {
        GETCH(tty -> read_q,c);

        if(c == '\r')
        {
            if(I_CRNL(tty))
            {
                c = '\n';
            }
            else if(I_NOCR(tty))
            {
                continue;
            }
        }
        else if((c == '\n') && I_NLCR(tty))
        {
            c = '\r';
        }

        if(I_UCLC(tty))
        {
            c = tolower(c);
        }

        if(L_CANON(tty))
        {
            if(c == KILL_CHAR(tty))
            {
                //deal with killing the input line
                while((!(EMPTY(tty -> secondary))) || ((c = LAST(tty -> secondary)) == '\n') || (c == EOF_CHAR(tty)))
                {
                    if(L_ECHO(tty))
                    {
                        if(c < 32)
                        {
                            PUTCH(127,tty -> write_q);
                        }

                        PUTCH(127,tty -> write_q);
                        tty -> write(tty);
                    }

                    DEC(tty -> secondary.head);
                }

                continue;
            }

            if(c == ERASE_CHAR(tty))
            {
                if(EMPTY(tty -> secondary) || ((c = LAST(tty -> secondary)) == '\n') || (c == EOF_CHAR(tty)))
                {
                    continue;
                }

                if(L_ECHO(tty))
                {
                    if(c < 32)
                    {
                        PUTCH(127,tty -> write_q);
                    }

                    PUTCH(127,tty -> write_q);
                    tty -> write(tty);
                }

                DEC(tty -> secondary.head);
                continue;
            }

            if(c == STOP_CHAR(tty))
            {
                tty -> stopped = 1;
                continue;
            }

            if(c == START_CHAR(tty))
            {
                tty -> stopped = 0;
                continue;
            }
        }

        if(L_ISIG(tty))
        {
            if(c == INTR_CHAR(tty))
            {
                tty_intr(tty,INTMASK);
                continue;
            }

            if(c == QUIT_CHAR(tty))
            {
                tty_intr(tty,QUITMASK);
                continue;
            }
        }

        if((c == '\n') || (c == EOF_CHAR(tty)))
        {
            tty -> secondary.data++;
        }

        if(L_ECHO(tty))
        {
            if(c == '\n')
            {
                PUTCH('\r',tty -> write_q);
                PUTCH('\n',tty -> write_q);
            }
            else if(c < 32)
            {
                if(L_ECHOCTL(tty))
                {
                    PUTCH('^',tty -> write_q);
                    PUTCH(c + 64,tty -> write_q);
                }
            }
            else
            {
                PUTCH(c,tty -> write_q);
            }

            tty -> write(tty);
        }

        PUTCH(c,tty -> secondary);
    }

    wake_up(&tty -> secondary.proc_list);
}

int64_t tty_read(ulong channel,char *buf,int64_t nr)
{
    struct tty_struct *tty;
    char c,*b = buf;
    int minimum,time,flag = 0;
    int64_t oldalarm;

    if((channel > (TTY_CHANNEL_NUM - 1)) || (nr < 0))
    {
        return -1;
    }

    tty = &tty_table[channel];
    oldalarm = current -> alarm;
    time = 10L * tty -> termios.c_cc[VTIME];
    minimum = tty -> termios.c_cc[VMIN];

    if(time && (!minimum))
    {
        minimum = 1;

        if(flag = ((!oldalarm) || ((time + jiffies) < oldalarm)))
        {
            current -> alarm = time + jiffies;
        }
    }

    if(minimum > nr)
    {
        minimum = nr;
    }

    while(nr > 0)
    {
        if(flag && (current -> signal & ALRMMASK))
        {
            current -> signal &= ~ALRMMASK;
            break;
        }

        if(current -> signal)
        {
            break;
        }

        if(EMPTY(tty -> secondary) || (L_CANON(tty) && (!tty -> secondary.data) && (LEFT(tty -> secondary) > 20)))
        {
            sleep_if_empty(&tty -> secondary);
            continue;
        }

        do
        {
            GETCH(tty -> secondary,c);

            if((c == EOF_CHAR(tty)) || (c == '\n'))
            {
                tty -> secondary.data--;
            }

            if((c == EOF_CHAR(tty)) && L_CANON(tty))
            {
                return (b - buf);
            }
            else
            {
                put_fs_byte(c,b++);

                if(!--nr)
                {
                    break;
                }
            }

        }while((nr > 0) && (!EMPTY(tty -> secondary)));

        if(time && (!L_CANON(tty)))
        {
            if(flag = ((!oldalarm) || ((time + jiffies) < oldalarm)))
            {
                current -> alarm = time + jiffies;
            }
            else
            {
                current -> alarm = oldalarm;
            }
        }

        if(L_CANON(tty))
        {
            if(b - buf)
            {
                break;
            }
        }
        else if((b - buf) >= minimum)
        {
            break;
        }
    }

    current -> alarm = oldalarm;

    if(current -> signal && (!(b - buf)))
    {
        return -EINTR;
    }

    return b - buf;
}

int64_t tty_write(ulong channel,char *buf,int64_t nr)
{
    static cr_flag = 0;
    struct tty_struct *tty;
    char c,*b = buf;
    int64_t i;

    if((channel > (TTY_CHANNEL_NUM - 1)) || (nr < 0))
    {
        return -1;
    }

    tty = tty_table + channel;

    while(nr > 0)
    {
        sleep_if_full(&tty -> write_q);

        if(current -> signal)
        {
            break;
        }

        while((nr > 0) && (!FULL(tty -> write_q)))
        {
            c = get_fs_byte(b);

            if(O_POST(tty))
            {
                if((c == '\r') && O_CRNL(tty))
                {
                    c = '\n';
                }
                else if((c == '\n') && O_NLRET(tty))
                {
                    c = '\r';
                }

                if((c == '\n') && (!cr_flag) && O_NLCR(tty))
                {
                    cr_flag = 1;
                    PUTCH('\r',tty -> write_q);
                    continue;
                }

                if(O_LCUC(tty))
                {
                    c = toupper(c);
                }
            }

            b++;
            nr--;
            cr_flag = 0;
            PUTCH(c,tty -> write_q);
        }
        
        tty -> write(tty);

        if(nr > 0)
        {
            schedule();
        }
    }
    
    return (b - buf);
}

void do_tty_interrupt(int tty)
{
    copy_to_cooked(tty_table + tty);
}

void chr_dev_init(void)
{
    
}