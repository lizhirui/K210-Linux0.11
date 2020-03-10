#include "encoding.h"

#define LREG ld
#define SREG sd
#define LFREG flw
#define SFREG fsw

#define REGBYTES 8

#define SIG_CHLD 17
#define state 0
#define counter 8
#define priority 16
#define signal 24
#define sigaction 32
#define blocked (sigaction + 32 * 24)

//offsets within sigaction
#define sa_handler 0
#define sa_mask 8
#define sa_flags 12
#define sa_restorer 16

//#define nr_system_calls 73
#define nr_system_calls 2048

.section .text.kernel,"ax",@progbits
.globl system_call
.globl syscall_from_kernel
.align 8
syscall_from_kernel:
	.dword 0

bad_sys_call:
	li a0,-1
	ret

reschedule:
	la ra,ret_from_sys_call
	tail schedule

system_call:
	addi sp,sp,-2 * REGBYTES
	sd ra,(sp)
	sd s0,1 * REGBYTES(sp)
	li t0,nr_system_calls - 1
	sub t0,t0,a0
	bltz t0,bad_sys_call
	slli a0,a0,3
	la t0,sys_call_table
	add t0,t0,a0
	ld t0,0(t0)
	mv a0,a1
	mv a1,a2
	mv a2,a3
	jalr t0
	mv s0,a0
	la t0,current
	ld t0,0(t0)
	ld t1,state(t0)
	//bnez t1,reschedule
	ld t1,counter(t0)
	//beqz t1,reschedule

ret_from_sys_call:
	la t0,current
	ld t0,0(t0)
	la t1,task
	ld t1,0(t1)
	sub t1,t0,t1
	beqz t1,3f
	la t1,syscall_from_kernel
	ld t1,0(t1)
	beqz t1,3f
	ld t1,signal(t0)
	ld t2,blocked(t0)
	not t2,t2
	and t1,t1,t2
	mv a0,t1
	call bitscan
	addi t3,a0,1
	beqz t3,3f
	li t3,1
	sll t3,t3,a0
	not t3,t3
	and t1,t1,t3
	sd t1,signal(t0)
	addi a0,a0,1
	call do_signal
3:	mv a0,s0
	ld s0,1 * REGBYTES(sp)
	ld ra,(sp)
	addi sp,sp,2 * REGBYTES
	ret

.globl sys_execve
sys_execve:
	addi sp,sp,-REGBYTES
	sd ra,(sp)
	jal do_execve
	ld ra,(sp)
	addi sp,sp,REGBYTES
	ret

.globl sys_fork
sys_fork:
	addi sp,sp,-REGBYTES
	sd ra,(sp)
	call find_empty_process
	bltz a0,1f
	call copy_process
	ld ra,(sp)
	addi sp,sp,REGBYTES
1:	ret

.globl sys_setup
.globl sys_read
.globl sys_open
.globl sys_close
.globl sys_creat
.globl sys_link
.globl sys_unlink
.globl sys_chdir
.globl sys_mknod
.globl sys_chmod
.globl sys_chown
.globl sys_stat
.globl sys_lseek
.globl sys_mount
.globl sys_umount
.globl sys_fstat
.globl sys_utime
.globl sys_access
.globl sys_sync
.globl sys_mkdir
.globl sys_rmdir
.globl sys_dup
.globl sys_pipe
.globl sys_getegid
.globl sys_ioctl
.globl sys_fcntl
.globl sys_chroot
.globl sys_ustat
.globl sys_dup2

//sys_setup:ret
//sys_read:ret
//sys_open:ret
//sys_close:ret
//sys_creat:ret
//sys_link:ret
//sys_unlink:ret
//sys_chdir:ret
//sys_mknod:ret
//sys_chmod:ret
//sys_chown:ret
//sys_stat:ret
//sys_lseek:ret
//sys_mount:ret
//sys_umount:ret
//sys_fstat:ret
//sys_utime:ret
//sys_access:ret
//sys_sync:ret
//sys_mkdir:ret
//sys_rmdir:ret
//sys_dup:ret
//sys_pipe:ret
sys_getegid:ret
//sys_ioctl:ret
//sys_fcntl:ret
//sys_chroot:ret
//sys_ustat:ret
//sys_dup2:ret