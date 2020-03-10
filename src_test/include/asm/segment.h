#ifndef __SEGMENT_H__
#define __SEGMENT_H__
	#include "common.h"
	#include "linux/sched.h"

	extern ulong get_fs();
	extern void set_fs(ulong v);
	extern ulong get_ds();
    extern uint8_t get_fs_byte(const uint8_t *addr);
	extern uint16_t get_fs_word(const uint16_t *addr);
	extern uint32_t get_fs_long(const uint32_t *addr);
	extern uint64_t get_fs_64long(const uint64_t *addr);
	extern void put_fs_byte(uint8_t val,uint8_t *addr);
	extern void put_fs_word(uint16_t val,uint16_t * addr);
	extern void put_fs_long(uint32_t val,uint32_t * addr);
	extern void put_fs_64long(uint64_t val,uint64_t * addr);

#endif