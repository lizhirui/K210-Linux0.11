#ifndef __RISCVFUNC_H__
#define __RISCVFUNC_H__

    #define reg_zero 0
    #define reg_ra 1
    #define reg_sp 2
    #define reg_gp 3
    #define reg_tp 4
    #define reg_t0 5
    #define reg_t1 6
    #define reg_t2 7
    #define reg_fp 8
    #define reg_s0 8
    #define reg_s1 9
    #define reg_a0 10
    #define reg_a1 11
    #define reg_a2 12
    #define reg_a3 13
    #define reg_a4 14
    #define reg_a5 15
    #define reg_a6 16
    #define reg_a7 17
    #define reg_s2 18
    #define reg_s3 19
    #define reg_s4 20
    #define reg_s5 21
    #define reg_s6 22
    #define reg_s7 23
    #define reg_s8 24
    #define reg_s9 25
    #define reg_s10 26
    #define reg_s11 27
    #define reg_t3 28
    #define reg_t4 29
    #define reg_t5 30
    #define reg_t6 31

    void regs_backup(uint64_t regs[32],uint64_t fregs[32]);

    #include "syslog.h"
    #include "csr_define.h"
    #include "privilege.h"
    #include "core.h"
    #include "page_table_entry.h"
    #include "page_table.h"

#endif