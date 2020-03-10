#include "encoding.h"

#define LREG ld
#define SREG sd
#define LFREG flw
#define SFREG fsw
#define REGBYTES 8
#define STKSHIFT 15 //Stack Size this value must be same as linker.ld

#define PRIVILEGE_SUPERVISOR 2
#define PRIVILEGE_MACHINE 3

.section .text.machinetrap,"ax",@progbits
.globl machine_trap_entry
.align 2
machine_trap_entry:
	//Reserve stack
	csrw mscratch,sp
	csrw sepc,t0
	csrr t0,mstatus
	//li t1,0x1800
	//and t0,t0,t1
	srli t0,t0,11
	andi t0,t0,0x03
	bnez t0,stack_switch_ok
	csrr t0,mhartid
	bnez t0,other_core_stack_switch
	la sp,cur_kernel_stack
	ld sp,(sp)
	j stack_switch_ok

other_core_stack_switch:
	la sp,_trapsp0
	csrr t0,mhartid
	sll t0,t0,STKSHIFT
	add sp,sp,t0

stack_switch_ok:
	addi sp,sp,-66 * REGBYTES

	//Backup GPRs
	SREG x1,1 * REGBYTES(sp)
	SREG x2,2 * REGBYTES(sp)
	SREG x3,3 * REGBYTES(sp)
	SREG x4,4 * REGBYTES(sp)
	SREG x5,5 * REGBYTES(sp)
	SREG x6,6 * REGBYTES(sp)
	SREG x7,7 * REGBYTES(sp)
	SREG x8,8 * REGBYTES(sp)
	SREG x9,9 * REGBYTES(sp)
	SREG x10,10 * REGBYTES(sp)
	SREG x11,11 * REGBYTES(sp)
	SREG x12,12 * REGBYTES(sp)
	SREG x13,13 * REGBYTES(sp)
	SREG x14,14 * REGBYTES(sp)
	SREG x15,15 * REGBYTES(sp)
	SREG x16,16 * REGBYTES(sp)
	SREG x17,17 * REGBYTES(sp)
	SREG x18,18 * REGBYTES(sp)
	SREG x19,19 * REGBYTES(sp)
	SREG x20,20 * REGBYTES(sp)
	SREG x21,21 * REGBYTES(sp)
	SREG x22,22 * REGBYTES(sp)
	SREG x23,23 * REGBYTES(sp)
	SREG x24,24 * REGBYTES(sp)
	SREG x25,25 * REGBYTES(sp)
	SREG x26,26 * REGBYTES(sp)
	SREG x27,27 * REGBYTES(sp)
	SREG x28,28 * REGBYTES(sp)
	SREG x29,29 * REGBYTES(sp)
	SREG x30,30 * REGBYTES(sp)
	SREG x31,31 * REGBYTES(sp)
	csrr t0,sepc
	csrr t1,mscratch
	SREG t0,5 * REGBYTES(sp)
	SREG t1,2 * REGBYTES(sp)

	//Backup FRs
	SFREG f0,(0 + 32) * REGBYTES(sp)
	SFREG f1,(1 + 32) * REGBYTES(sp)
	SFREG f2,(2 + 32) * REGBYTES(sp)
	SFREG f3,(3 + 32) * REGBYTES(sp)
	SFREG f4,(4 + 32) * REGBYTES(sp)
	SFREG f5,(5 + 32) * REGBYTES(sp)
	SFREG f6,(6 + 32) * REGBYTES(sp)
	SFREG f7,(7 + 32) * REGBYTES(sp)
	SFREG f8,(8 + 32) * REGBYTES(sp)
	SFREG f9,(9 + 32) * REGBYTES(sp)
	SFREG f10,(10 + 32) * REGBYTES(sp)
	SFREG f11,(11 + 32) * REGBYTES(sp)
	SFREG f12,(12 + 32) * REGBYTES(sp)
	SFREG f13,(13 + 32) * REGBYTES(sp)
	SFREG f14,(14 + 32) * REGBYTES(sp)
	SFREG f15,(15 + 32) * REGBYTES(sp)
	SFREG f16,(16 + 32) * REGBYTES(sp)
	SFREG f17,(17 + 32) * REGBYTES(sp)
	SFREG f18,(18 + 32) * REGBYTES(sp)
	SFREG f19,(19 + 32) * REGBYTES(sp)
	SFREG f20,(20 + 32) * REGBYTES(sp)
	SFREG f21,(21 + 32) * REGBYTES(sp)
	SFREG f22,(22 + 32) * REGBYTES(sp)
	SFREG f23,(23 + 32) * REGBYTES(sp)
	SFREG f24,(24 + 32) * REGBYTES(sp)
	SFREG f25,(25 + 32) * REGBYTES(sp)
	SFREG f26,(26 + 32) * REGBYTES(sp)
	SFREG f27,(27 + 32) * REGBYTES(sp)
	SFREG f28,(28 + 32) * REGBYTES(sp)
	SFREG f29,(29 + 32) * REGBYTES(sp)
	SFREG f30,(30 + 32) * REGBYTES(sp)
	SFREG f31,(31 + 32) * REGBYTES(sp)
	csrr t0,mstatus
	SREG t0,(0 + 64) * REGBYTES(sp)
	csrr t0,mie
	SREG t0,(1 + 64) * REGBYTES(sp)

	//csrr a0,mcause
	//bgez a0,supervisor_trap_check_skip
	j supervisor_trap_check_skip

	csrr t0,mstatus
	srli t0,t0,11
	andi t0,t0,3
	addi t0,t0,-3
	bnez t0,supervisor_trap_entry_insert_entry

supervisor_trap_check_skip:
	mv a0,zero
	addi a0,a0,PRIVILEGE_MACHINE
	csrr a1,mcause
	csrr a2,mepc
	csrr a3,mbadaddr
	jal privilege_trap_entry

	csrw mscratch,sp
	csrr a0,mcause
	csrr a1,mepc
	mv a2,sp
	add a3,sp,32 * REGBYTES
	mv a4,sp
	add a4,sp,64 * REGBYTES
	bgez a0,machine_exception_handler_call
	li t1,0x7FFFFFFFFFFFFFFF
	and a0,a0,t1
	jal machine_interrupt_handler
	j machine_handler_call_ok

machine_exception_handler_call:
	jal machine_exception_handler

machine_handler_call_ok:
	csrw mepc,a0
	li t0,MIP_MEIP
	csrw mie,t0
	jal privilege_trap_exit
	//close global interrupt flag
	csrr t0,mstatus
	andi t0,t0,~0x08
	csrw mstatus,t0

	//Restore CSR
	LREG t0,(0 + 64) * REGBYTES(sp)
	csrr t1,mstatus
	li t2,~0x1880
	and t1,t1,t2
	li t2,0x1880
	and t0,t0,t2
	or t1,t1,t0
	csrw mstatus,t1
	LREG t1,(1 + 64) * REGBYTES(sp)
	csrw mie,t1

	//Restore regs
	LREG x1,1 * REGBYTES(sp)
	LREG x3,3 * REGBYTES(sp)
	LREG x4,4 * REGBYTES(sp)
	LREG x5,5 * REGBYTES(sp)
	LREG x6,6 * REGBYTES(sp)
	LREG x7,7 * REGBYTES(sp)
	LREG x8,8 * REGBYTES(sp)
	LREG x9,9 * REGBYTES(sp)
	LREG x10,10 * REGBYTES(sp)
	LREG x11,11 * REGBYTES(sp)
	LREG x12,12 * REGBYTES(sp)
	LREG x13,13 * REGBYTES(sp)
	LREG x14,14 * REGBYTES(sp)
	LREG x15,15 * REGBYTES(sp)
	LREG x16,16 * REGBYTES(sp)
	LREG x17,17 * REGBYTES(sp)
	LREG x18,18 * REGBYTES(sp)
	LREG x19,19 * REGBYTES(sp)
	LREG x20,20 * REGBYTES(sp)
	LREG x21,21 * REGBYTES(sp)
	LREG x22,22 * REGBYTES(sp)
	LREG x23,23 * REGBYTES(sp)
	LREG x24,24 * REGBYTES(sp)
	LREG x25,25 * REGBYTES(sp)
	LREG x26,26 * REGBYTES(sp)
	LREG x27,27 * REGBYTES(sp)
	LREG x28,28 * REGBYTES(sp)
	LREG x29,29 * REGBYTES(sp)
	LREG x30,30 * REGBYTES(sp)
	LREG x31,31 * REGBYTES(sp)

	LFREG f0,(0 + 32) * REGBYTES(sp)
	LFREG f1,(1 + 32) * REGBYTES(sp)
	LFREG f2,(2 + 32) * REGBYTES(sp)
	LFREG f3,(3 + 32) * REGBYTES(sp)
	LFREG f4,(4 + 32) * REGBYTES(sp)
	LFREG f5,(5 + 32) * REGBYTES(sp)
	LFREG f6,(6 + 32) * REGBYTES(sp)
	LFREG f7,(7 + 32) * REGBYTES(sp)
	LFREG f8,(8 + 32) * REGBYTES(sp)
	LFREG f9,(9 + 32) * REGBYTES(sp)
	LFREG f10,(10 + 32) * REGBYTES(sp)
	LFREG f11,(11 + 32) * REGBYTES(sp)
	LFREG f12,(12 + 32) * REGBYTES(sp)
	LFREG f13,(13 + 32) * REGBYTES(sp)
	LFREG f14,(14 + 32) * REGBYTES(sp)
	LFREG f15,(15 + 32) * REGBYTES(sp)
	LFREG f16,(16 + 32) * REGBYTES(sp)
	LFREG f17,(17 + 32) * REGBYTES(sp)
	LFREG f18,(18 + 32) * REGBYTES(sp)
	LFREG f19,(19 + 32) * REGBYTES(sp)
	LFREG f20,(20 + 32) * REGBYTES(sp)
	LFREG f21,(21 + 32) * REGBYTES(sp)
	LFREG f22,(22 + 32) * REGBYTES(sp)
	LFREG f23,(23 + 32) * REGBYTES(sp)
	LFREG f24,(24 + 32) * REGBYTES(sp)
	LFREG f25,(25 + 32) * REGBYTES(sp)
	LFREG f26,(26 + 32) * REGBYTES(sp)
	LFREG f27,(27 + 32) * REGBYTES(sp)
	LFREG f28,(28 + 32) * REGBYTES(sp)
	LFREG f29,(29 + 32) * REGBYTES(sp)
	LFREG f30,(30 + 32) * REGBYTES(sp)
	LFREG f31,(31 + 32) * REGBYTES(sp)

	LREG sp,2 * REGBYTES(sp)
	mret