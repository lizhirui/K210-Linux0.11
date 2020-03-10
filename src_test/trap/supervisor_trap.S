#include "encoding.h"

#define LREG ld
#define SREG sd
#define LFREG flw
#define SFREG fsw
#define REGBYTES 8
#define STKSHIFT 15 //Stack Size this value must be same as linker.ld

#define PRIVILEGE_SUPERVISOR 1
#define PRIVILEGE_MACHINE 3

.section .text.supervisortrap,"ax",@progbits
.globl supervisor_trap_entry
.globl supervisor_trap_entry_insert_entry
supervisor_trap_entry_insert_entry:
	mv a0,zero
	addi a0,a0,PRIVILEGE_SUPERVISOR
	csrr a1,mcause
	jal privilege_trap_entry
	mv t0,zero
	addi t0,t0,1
	csrr a0,mcause//backup mcause
	csrr a1,mepc//backup mepc
	la t1,supervisor_trap_entry_insert
	csrw mepc,t1
	csrw mie,0//disable global interrupt
	csrr t1,sstatus
	ori t1,t1,0x100
	csrw sstatus,t1
	csrr t0,sscratch
	li t1,1
	sd t1,0(t0)
	mret

supervisor_trap_entry:
	csrr t0,sscratch
	sd zero,0(t0)
	//Reserve stack
	addi sp,sp,-64 * REGBYTES

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
	
	mv a0,zero
	addi a0,a0,PRIVILEGE_SUPERVISOR
	csrr a1,scause
	jal privilege_trap_entry
	mv t0,zero

supervisor_trap_entry_insert:
	bnez t0,supervisor_trap_entry_insert_2
	csrr a0,scause
	csrr a1,sepc

supervisor_trap_entry_insert_2:
	mv a2,sp
	add a3,sp,32 * REGBYTES
	bgez a0,supervisor_exception_handler_call
	li t1,0x7FFFFFFFFFFFFFFF
	and a0,a0,t1
	jal supervisor_interrupt_handler
	j supervisor_handler_call_ok

supervisor_exception_handler_call:
	jal supervisor_exception_handler

supervisor_handler_call_ok:
	jal privilege_trap_exit

	//Restore regs
	csrr t0,sscratch
	ld t1,0(t0)
	beqz t1,supervisor_mie_set_syscall_skip
	csrw sepc,a0
	li a0,0
	ecall
supervisor_mie_set_syscall_skip:
	LREG x1,1 * REGBYTES(sp)
	LREG x2,2 * REGBYTES(sp)
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

	addi sp,sp,64 * REGBYTES
	sret