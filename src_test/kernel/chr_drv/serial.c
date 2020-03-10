#include "common.h"
#include "linux/tty.h"
#include "linux/sched.h"

#define WAKEUP_CHARS (TTY_BUF_SIZE / 4)

#define TTY_ID 0
#define TTY_READ_ID (TTY_ID << 1)
#define TTY_WRITE_ID (TTY_READ_ID | 0x01)

extern volatile uart_t *const uart[3];

extern void do_tty_interrupt(int tty);

void serial_send_interrupt(void *ctx)
{
    struct tty_queue *cur = table_list[TTY_WRITE_ID];
    ulong size = (cur -> head - cur -> tail) & (TTY_BUF_SIZE - 1);

    if(cur -> proc_list != NULL)
    {
        cur -> proc_list -> state = TASK_RUNNING;
    }

    if(size != 0)
    {
        uart[UART_DEBUG_DEVICE] -> THR = cur -> buf[cur -> tail];
        cur -> tail = (cur -> tail + 1) & (TTY_BUF_SIZE - 1);
    }
}

void serial_rev_interrupt(void *ctx)
{
    struct tty_queue *cur = table_list[TTY_READ_ID];
    ulong size = (cur -> head - cur -> tail) & (TTY_BUF_SIZE - 1);

    if(size < (TTY_BUF_SIZE - 1))
    {
        //not full
        cur -> buf[cur -> head] = uart[UART_DEBUG_DEVICE] -> RBR & 0xFF;
        cur -> head++;
    }

    do_tty_interrupt(TTY_ID);
}

void rs_init()
{
    uart_irq_register(UART_DEVICE_3,UART_SEND,serial_send_interrupt,NULL,PLIC_NUM_PRIORITIES);
    uart_irq_register(UART_DEVICE_3,UART_RECEIVE,serial_rev_interrupt,NULL,PLIC_NUM_PRIORITIES);
}

//This routine gets called when tty_write has put something into the write_queue.
//It must check whether the queue is empty,and set the interrupt register accordingly
void rs_write(struct tty_struct *tty)
{
    //sysctl_disable_irq();
    
    if(!EMPTY(tty -> write_q))
    {
        while(uart[UART_DEBUG_DEVICE] -> LSR & (1U << 5));
        serial_send_interrupt(NULL);
    }

    //sysctl_enable_irq();
}