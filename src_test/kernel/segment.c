#include "common.h"

static ulong fs_is_kernel = 0;

extern inline ulong get_fs()
{
	return fs_is_kernel;
}

extern inline void set_fs(ulong v)
{
	fs_is_kernel = v;
}

extern inline ulong get_ds()
{
	return 1;
}

extern inline uint8_t get_fs_byte(const uint8_t *addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	return *((uint8_t *)addr_t);
}

extern inline uint16_t get_fs_word(const uint16_t *addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	return *((uint16_t *)addr_t);
}

extern inline uint32_t get_fs_long(const uint32_t *addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	return *((uint32_t *)addr_t);
}

extern inline uint64_t get_fs_64long(const uint64_t *addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	return *((uint64_t *)addr_t);
}

extern inline void put_fs_byte(uint8_t val,uint8_t *addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	*((uint8_t *)addr_t) = val;
}

extern inline void put_fs_word(uint16_t val,uint16_t * addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	*((uint16_t *)addr_t) = val;
}

extern inline void put_fs_long(uint32_t val,uint32_t * addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	*((uint32_t *)addr_t) = val;
}

extern inline void put_fs_64long(uint64_t val,uint64_t * addr)
{
	ulong addr_t = (ulong)addr;
	(!fs_is_kernel) ? user_addr_to_kernel(&addr_t) : 0;
	*((uint64_t *)addr_t) = val;
}