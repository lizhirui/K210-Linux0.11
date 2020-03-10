#include "encoding.h"

#define LREG ld
#define SREG sd
#define LFREG flw
#define SFREG fsw
#define REGBYTES 8
#define STKSHIFT 15 //Stack Size this value must be same as linker.ld

#define PRIVILEGE_SUPERVISOR 2
#define PRIVILEGE_MACHINE 3

.section .text.start,"ax",@progbits
.globl _start
_start:
    j 1f
    .word 0xdeadbeef
    .align 3
    .global g_wake_up
    g_wake_up:
        .dword 1
        .dword 0
1:
	//Initialize CSRs
	csrw mideleg,0
	csrw medeleg,0
	csrw mie,0
	csrw mip,0
	//Initialize trap handler address
	la t0,machine_trap_entry
	csrw mtvec,t0
	mv t0,zero
	csrw stvec,t0

	//Initialize general-purpose registers
	li x1,0
	li x2,0
	li x3,0
	li x4,0
	li x5,0
	li x6,0
	li x7,0
	li x8,0
	li x9,0
	li x10,0
	li x11,0
	li x12,0
	li x13,0
	li x14,0
	li x15,0
	li x16,0
	li x17,0
	li x18,0
	li x19,0
	li x20,0
	li x21,0
	li x22,0
	li x23,0
	li x24,0
	li x25,0
	li x26,0
	li x27,0
	li x28,0
	li x29,0
	li x30,0
	li x31,0

	
	li t0,MSTATUS_FS
	csrs mstatus,t0

	//Initialize float registers
	fssr x0
	fmv.w.x f0,x0
	fmv.w.x f1,x0
	fmv.w.x f2,x0
	fmv.w.x f3,x0
	fmv.w.x f4,x0
	fmv.w.x f5,x0
	fmv.w.x f6,x0
	fmv.w.x f7,x0
	fmv.w.x f8,x0
	fmv.w.x f9,x0
	fmv.w.x f10,x0
	fmv.w.x f11,x0
	fmv.w.x f12,x0
	fmv.w.x f13,x0
	fmv.w.x f14,x0
	fmv.w.x f15,x0
	fmv.w.x f16,x0
	fmv.w.x f17,x0
	fmv.w.x f18,x0
	fmv.w.x f19,x0
	fmv.w.x f20,x0
	fmv.w.x f21,x0
	fmv.w.x f22,x0
	fmv.w.x f23,x0
	fmv.w.x f24,x0
	fmv.w.x f25,x0
	fmv.w.x f26,x0
	fmv.w.x f27,x0
	fmv.w.x f28,x0
	fmv.w.x f29,x0
	fmv.w.x f30,x0
	fmv.w.x f31,x0

	//Initialize Global Pointer
	.option push
	.option norelax
	la gp,__global_pointer$
	.option pop

	//Initialize Local Pointer
    la  tp,_end + 63
    and tp,tp,-64
    csrr a0,mhartid

	//Reserve 32KB stack for current core
	add sp,a0,1
	sll sp,sp,STKSHIFT
    add sp,sp,tp

	j entry

.globl get_sp
get_sp:
	mv a0,sp
	ret