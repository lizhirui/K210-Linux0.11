#ifndef __CSR_DEFINE_H__
#define __CSR_DEFINE_H__
    
    //Machine Vendor ID Register
    typedef struct csr_define_mvendorid
    {
        ulong vendor;
    }csr_define_mvendorid;

    //Machine Architecture ID Register
    typedef struct csr_define_marchid
    {
        ulong architecture_id;
    }csr_define_marchid;

    //Machine Implementation ID Register
    typedef struct csr_define_mimpid
    {
        ulong implementation;
    }csr_define_mimpid;

    //Hard ID Register
    typedef struct csr_define_mhartid
    {
        ulong hart_id;
    }csr_define_mhartid;

    //Machine Status Register
    typedef struct csr_define_mstatus
    {
        ulong uie : 1;
        ulong sie : 1;
        ulong hie : 1;
        ulong mie : 1;
        ulong upie : 1;
        ulong spie : 1;
        ulong hpie : 1;
        ulong mpie : 1;
        ulong spp : 1;
        ulong hpp : 2;
        ulong mpp : 2;
        ulong fs : 2;
        ulong xs : 2;
        ulong mprv : 1;
        ulong pum : 1;
        ulong mxr : 1;
        ulong : 4;
        ulong vm : 5;
        ulong : 34;
        ulong sd : 1;
    }csr_define_mstatus;

    //Machine ISA Register misa
    typedef struct csr_define_misa
    {
        ulong a : 1;//Atomic extension
        ulong b : 1;//Tentatively reserved for Bit operations extension
        ulong c : 1;//Compressed extension
        ulong d : 1;//Double-precision floating-point extension
        ulong e : 1;//RV32E base ISA
        ulong f : 1;//Single-precision floating-point extension
        ulong g : 1;//Additional standard extensions present
        ulong h : 1;//Hypervisor mode implemented
        ulong i : 1;//RV32I/64I/128I base ISA
        ulong j : 1;//Reserved
        ulong k : 1;//Reserved
        ulong l : 1;//Tentatively reserved for Decimal Floatin-Point extension
        ulong m : 1;//Integer Multiply/Divide extension
        ulong n : 1;//User-level interrupts supported
        ulong o : 1;//Reserved
        ulong p : 1;//Tentatively reserved for Packed-SIMD extension
        ulong q : 1;//Quad-precision floating-point extension
        ulong r : 1;//Reserved
        ulong s : 1;//Supervisor mode implemented
        ulong t : 1;//Tentatively reserved for TransactionalMemory extension
        ulong u : 1;//User mode implemented
        ulong v : 1;//Tentatively reserved for Vector extension
        ulong w : 1;//Reserved
        ulong x : 1;//Non-standard extensions present
        ulong y : 1;//Reserved
        ulong z : 1;//Reserved
        ulong : 36;//WIRI
        ulong base : 2;//Base 1-32 2-64 3-128
    }csr_define_misa;

    //Machine Exception Delegation Register
    typedef struct csr_define_medeleg
    {
        ulong instruction_address_misaligned : 1;
        ulong instruction_access_fault : 1;
        ulong illegal_instruction : 1;
        ulong breakpoint : 1;
        ulong load_address_misaligned : 1;
        ulong load_access_fault : 1;
        ulong store_or_amo_address_misaligned : 1;
        ulong store_or_amo_access_fault : 1;
        ulong environment_call_from_u_mode : 1;
        ulong environment_call_from_s_mode : 1;
        ulong environment_call_from_h_mode : 1;
        ulong environment_call_from_m_mode : 1;
        ulong : 52;
    }csr_define_medeleg;

    //Machine Interrupt Delegation Register
    typedef struct csr_define_mideleg
    {
        ulong user_software_interrupt : 1;
        ulong supervisor_software_interrupt : 1;
        ulong : 1;
        ulong machine_software_interrupt : 1;
        ulong user_timer_interrupt : 1;
        ulong supervisor_timer_interrupt : 1;
        ulong : 1;
        ulong machine_timer_interrupt : 1;
        ulong user_external_interrupt : 1;
        ulong supervisor_external_interrupt : 1;
        ulong : 1;
        ulong machine_external_interrupt : 1;
        ulong : 52;
    }csr_define_mideleg;

    //Machine Trap-Vector Base-Address Register
    typedef struct csr_define_mtvec
    {
        ulong : 2;
        ulong trap_vector_base_address : 62;
    }csr_define_mtvec;

    //Machine Interrupt-pending Register
    typedef struct csr_define_mip
    {
        ulong usip : 1;
        ulong ssip : 1;
        ulong : 1;
        ulong msip : 1;
        ulong utip : 1;
        ulong stip : 1;
        ulong : 1;
        ulong mtip : 1;
        ulong ueip : 1;
        ulong seip : 1;
        ulong : 1;
        ulong meip : 1;
        ulong : 52;
    }csr_define_mip;

    //Machine Interrupt-enable Register
    typedef struct csr_define_mie
    {
        ulong usie : 1;
        ulong ssie : 1;
        ulong : 1;
        ulong msie : 1;
        ulong utie : 1;
        ulong stie : 1;
        ulong : 1;
        ulong mtie : 1;
        ulong ueie : 1;
        ulong seie : 1;
        ulong : 1;
        ulong meie : 1;
        ulong : 52;
    }csr_define_mie;

    //Machine Cycle Counter
    typedef struct csr_define_mcycle
    {
        ulong value;
    }csr_define_mcycle;

    //Machine Instructions Retired Counter
    typedef struct csr_define_minstret
    {
        ulong value;
    }csr_define_minstret;

    //Machine Supervisor Counter-Enable Register
    typedef struct csr_define_mscounteren
    {
        ulong CY : 1;
        ulong TM : 1;
        ulong IR : 1;
        ulong : 61;
    }csr_define_mscounteren;

    //Machine User Counter-Enable Register
    typedef struct csr_define_mucounteren
    {
        ulong CY : 1;
        ulong TM : 1;
        ulong IR : 1;
        ulong : 61;
    }csr_define_mucounteren;

    //Machine Scratch Register
    typedef struct csr_define_mscratch
    {
        ulong value;
    }csr_define_mscratch;

    //Machine Exception Program Counter
    typedef struct csr_define_mepc
    {
        ulong value;
    }csr_define_mepc;

    //Machine Cause Register
    typedef struct csr_define_mcause
    {
        ulong exception_code : 63;
        ulong interrupt : 1;
    }csr_define_mcause;

    //Machine Bad Address
    typedef struct csr_define_mbadaddr
    {
        ulong value;
    }csr_define_mbadaddr;

    //Machine Single Base Register
    typedef struct csr_define_mbase
    {
        ulong value;
    }csr_define_mbase;

    //Machine Single Bound Register
    typedef struct csr_define_mbound
    {
        ulong value;
    }csr_define_mbound;

    //Machine Instruction Base Register
    typedef struct csr_define_mibase
    {
        ulong value;
    }csr_define_mibase;

    //Machine Instruction Bound Register
    typedef struct csr_define_mibound
    {
        ulong value;
    }csr_define_mibound;

    //Machine Data Base Register
    typedef struct csr_define_mdbase
    {
        ulong Value;
    }csr_define_mdbase;

    //Machine Data Bound Register
    typedef struct csr_define_mdbound
    {
        ulong value;
    }csr_define_mdbound;

    //Supervisor Status Register
    typedef struct csr_define_sstatus
    {
        ulong uie : 1;
        ulong sie : 1;
        ulong : 2;
        ulong upie : 1;
        ulong spie : 1;
        ulong : 2;
        ulong spp : 1;
        ulong : 4;
        ulong fs : 2;
        ulong xs : 2;
        ulong : 1;
        ulong pum : 1;
        ulong : 44;
        ulong sd : 1;
    }csr_define_sstatus;

    //Supervisor Trap-Vector Base-Address Register
    typedef struct csr_define_stvec
    {
        ulong : 2;
        ulong trap_vector_base_address : 62;
    }csr_define_stvec;

     //Supervisor Exception Delegation Register
    typedef struct csr_define_sedeleg
    {
        ulong instruction_address_misaligned : 1;
        ulong instruction_access_fault : 1;
        ulong illegal_instruction : 1;
        ulong breakpoint : 1;
        ulong load_address_misaligned : 1;
        ulong load_access_fault : 1;
        ulong store_or_amo_address_misaligned : 1;
        ulong store_or_amo_access_fault : 1;
        ulong environment_call_from_u_mode : 1;
        ulong environment_call_from_s_mode : 1;
        ulong environment_call_from_h_mode : 1;
        ulong environment_call_from_m_mode : 1;
        ulong : 52;
    }csr_define_sedeleg;

    //Supervisor Interrupt Delegation Register
    typedef struct csr_define_sideleg
    {
        ulong user_software_interrupt : 1;
        ulong supervisor_software_interrupt : 1;
        ulong hypervisor_software_interrupt : 1;
        ulong machine_software_interrupt : 1;
        ulong user_timer_interrupt : 1;
        ulong supervisor_timer_interrupt : 1;
        ulong hypervisor_timer_interrupt : 1;
        ulong machine_timer_interrupt : 1;
        ulong user_external_interrupt : 1;
        ulong supervisor_external_interrupt : 1;
        ulong hypervisor_external_interrupt : 1;
        ulong machine_external_interrupt : 1;
        ulong : 52;
    }csr_define_sideleg;

    //Supervisor Interrupt-pending Register
    typedef struct csr_define_sip
    {
        ulong usip : 1;
        ulong ssip : 1;
        ulong : 2;
        ulong utip : 1;
        ulong stip : 1;
        ulong : 2;
        ulong ueip : 1;
        ulong seip : 1;
        ulong : 54;
    }csr_define_sip;

    //Supervisor Interrupt-enable Register
    typedef struct csr_define_sie
    {
        ulong usie : 1;
        ulong ssie : 1;
        ulong : 2;
        ulong utie : 1;
        ulong stie : 1;
        ulong : 2;
        ulong ueie : 1;
        ulong seie : 1;
        ulong : 54;
    }csr_define_sie;

    //Supervisor Scratch Register
    typedef struct csr_define_sscratch
    {
        ulong value;
    }csr_define_sscratch;

    //Supervisor Exception Program Counter
    typedef struct csr_define_sepc
    {
        ulong value;
    }csr_define_sepc;

    //Supervisor Cause Register
    typedef struct csr_define_scause
    {
        ulong exception_code : 63;
        ulong interrupt : 1;
    }csr_define_scause;

    //Supervisor Bad Register
    typedef struct csr_define_sbadaddr
    {
        ulong value;
    }csr_define_sbadaddr;

    //Supervisor Page-Table Base Register
    typedef struct csr_define_sptbr
    {
        ulong ppn : 38;
        ulong asid : 26;
    }csr_define_sptbr;

    //Cycle counter for RDCYCLE instruction
    typedef struct csr_define_cycle
    {
        ulong value;
    }csr_define_cycle;

    //Timer for RDTIME instruction
    typedef struct csr_define_time
    {
        ulong value;
    }csr_define_time;
    
    //Instructions-retired counter for RDINSTRET instruction
    typedef struct csr_define_instret
    {
        ulong value;
    }csr_define_instret;

    #define csr_mvendorid 0xF11
    #define csr_marchid 0xF12
    #define csr_mimpid 0xF13
    #define csr_mhartid 0xF14
    #define csr_mstatus 0x300
    #define csr_misa 0x301
    #define csr_medeleg 0x302
    #define csr_mideleg 0x303
    #define csr_mie 0x304
    #define csr_mtvec 0x305
    #define csr_mscratch 0x340//It saves a address which is relative to the core.
    #define csr_mepc 0x341
    #define csr_mcause 0x342
    #define csr_mbadaddr 0x343
    #define csr_mip 0x344
    #define csr_mbase 0x380
    #define csr_mbound 0x381
    #define csr_mibase 0x382
    #define csr_mibound 0x383
    #define csr_mdbase 0x384
    #define csr_mdbound 0x385
    #define csr_mcycle 0xB00
    #define csr_minstret 0xB02
    #define csr_mucounteren 0x320
    #define csr_mscounteren 0x321
    #define csr_sstatus 0x100
    #define csr_sedeleg 0x102
    #define csr_sideleg 0x103
    #define csr_sie 0x104
    #define csr_stvec 0x105
    #define csr_sscratch 0x140//It saves a address which is relative to the core as same as mscratch.
    #define csr_sepc 0x141
    #define csr_scause 0x142
    #define csr_sbadaddr 0x143
    #define csr_sip 0x144
    #define csr_sptbr 0x180
    #define csr_cycle 0xC00
    #define csr_time 0xC01
    #define csr_sinstret 0xC02

    typedef union csr_define_common
    {
        ulong value;
        csr_define_misa misa;
        csr_define_mvendorid mvendorid;
        csr_define_marchid marchid;
        csr_define_mimpid mimpid;
        csr_define_mhartid mhartid;
        csr_define_mstatus mstatus;
        csr_define_medeleg medeleg;
        csr_define_mideleg mideleg;
        csr_define_mie mie;
        csr_define_mtvec mtvec;
        csr_define_mscratch mscratch;
        csr_define_mepc mepc;
        csr_define_mcause mcause;
        csr_define_mbadaddr mbadaddr;
        csr_define_mip mip;
        csr_define_mbase mbase;
        csr_define_mbound mbound;
        csr_define_mibase mibase;
        csr_define_mibound mibound;
        csr_define_mdbase mdbase;
        csr_define_mdbound mdbound;
        csr_define_mcycle mcycle;
        csr_define_minstret minstret;
        csr_define_mucounteren mucounteren;
        csr_define_mscounteren mscounteren;
        csr_define_sstatus sstatus;
        csr_define_sedeleg sedeleg;
        csr_define_sideleg sideleg;
        csr_define_sie sie;
        csr_define_stvec stvec;
        csr_define_sscratch sscratch;
        csr_define_sepc sepc;
        csr_define_scause scause;
        csr_define_sbadaddr sbadaddr;
        csr_define_sip sip;
        csr_define_sptbr sptbr;
        csr_define_cycle cycle;
        csr_define_time time;
        csr_define_instret instret;
    }csr_define_common;

    #ifndef _VS
        #define csr_asm_read(reg) \
            ({ \
                csr_define_common __tmp; \
                asm volatile("csrr %0," #reg : "=r"(__tmp.value)); \
                __tmp; \
            })

        #define csr_asm_write(reg,value) \
            ({ \
                asm volatile("csrw " #reg ",%0" :: "r"(value)); \
            })
    #else
        extern csr_define_common __csr_define__tmp__;
        #define csr_asm_read(reg) (__csr_define__tmp__)
        #define csr_asm_write(reg,value)
    #endif

    #define csr_read(reg) csr_asm_read(reg)
    #define csr_write(reg,value) csr_asm_write(reg,value)

#endif