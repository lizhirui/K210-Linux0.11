#include "encoding.h"

#define LREG ld
#define SREG sd
#define LFREG flw
#define SFREG fsw

#define REGBYTES 8

#define oldtss a0
#define newtss a1

.section .text.kernel,"ax",@progbits
.globl switch_to_tss
.align 8
switch_to_tss:
	addi sp,sp,-64 * REGBYTES
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

	//a loop,to is loop variable
	li t0,63
	
stack_copy_to_oldtss:
	//calculate destination addr (oldtss + (t0 << 3))
	mv t1,t0
	slli t1,t1,3
	add t2,t1,oldtss
	add t3,t1,sp
	LREG t4,(t3)
	SREG t4,(t2)
	addi t0,t0,-1
	bgtz t0,stack_copy_to_oldtss
	
	//leaved loop

	//check stack integrity
	la t0,old_kernel_stack
	ld t0,(t0)
	sub t0,t0,sp
	mv a0,t0
	li t1,4096
	bge t0,t1,kernel_stack_overflow
	addi sp,sp,-REGBYTES
	SREG newtss,(sp)
	//call output_stack_usage
	LREG newtss,(sp)
	addi sp,sp,REGBYTES

	//load new sp
	LREG sp,2 * REGBYTES(newtss)

	//LREG a0,-1 * REGBYTES(sp)
	//mv a0,newtss
	//call syslog_flagp

	//a loop,to is loop variable
	//li t0,63

//newtss_copy_to_stack:
	//calculate source addr (newtss + (t0 << 3))
	//mv t1,t0
	//slli t1,t1,3
	//add t1,t1,newtss
	//LREG t2,(t1)
	//SREG t2,(sp)
	//addi t0,t0,-1
	//bgtz t0,stack_copy_to_newtss

	//LREG a0,sp
	//call syslog_flagp
	
	//leaved loop
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

	addi sp,sp,64 * REGBYTES
	ret

.globl fork_process_exit
fork_process_exit:
	addi sp,sp,-65 * REGBYTES
	call mepc_recover
	call interrupt_recover
	call privilege_trap_exit
	LREG ra,(sp)
	addi sp,sp,REGBYTES
	
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

	//addi sp,sp,64 * REGBYTES
	LREG x2,2 * REGBYTES(sp)
	mret

.globl stack_overflow_check
stack_overflow_check:
	addi sp,sp,-REGBYTES
	SREG ra,(sp)
	la t0,cur_kernel_stack
	ld t0,(t0)
	sub t0,t0,sp
	mv a0,t0
	li t1,4096
	bge t0,t1,kernel_stack_overflow
	LREG ra,(sp)
	addi sp,sp,REGBYTES
	ret