#ifndef _RISCV_LOCK_H__
#define _RISCV_LOCK_H__

typedef long _lock_t;
typedef _lock_t _LOCK_RECURSIVE_T;
typedef _lock_t _LOCK_T;

#include <_ansi.h>

#define __LOCK_INIT(CLASS,NAME) CLASS _lock_t NAME = 0;
#define __LOCK_INIT_RECURSIVE(CLASS,NAME) CLASS _lock_t NAME = 0;

void _lock_init(_lock_t *lock);
void _lock_init_recursive(_lock_t *lock);
void _lock_close(_lock_t *lock);
void _lock_close_recursive(_lock_t *lock);
void _lock_acquire(_lock_t *lock);
void _lock_acquire_recursive(_lock_t *lock);
int _lock_try_acquire(_lock_t *lock);
int _lock_try_acquire_recursive(_lock_t *lock);
void _lock_release(_lock_t *lock);
void _lock_release_recursive(_lock_t *lock);

#define __lock_init(lock) _lock_init(&(lock))
#define __lock_init_recursive(lock) _lock_init_recursive(&(lock))
#define __lock_close(lock) _lock_close(&(lock))
#define __lock_close_recursive(lock) _lock_close_recursive(&(lock))
#define __lock_acquire(lock) _lock_acquire(&(lock))
#define __lock_acquire_recursive(lock) _lock_acquire_recursive(&(lock))
#define __lock_try_acquire(lock) _lock_try_acquire(&(lock))
#define __lock_try_acquire_recursive(lock) _lock_try_acquire_recursive(&(lock))
#define __lock_release(lock) _lock_release(&(lock))
#define __lock_release_recursive(lock) _lock_release_recursive(&(lock))

#endif /* _RISCV_LOCK_H__ */
