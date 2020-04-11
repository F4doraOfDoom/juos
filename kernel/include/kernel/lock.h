#ifndef KERNEL_LOCK_H
#define KERNEL_LOCK_H

#include <kernel/kdef.h>

#define DECLARE_LOCK(name) volatile int name 
#define LOCK(name) \
	while (!__sync_bool_compare_and_swap(& name, 0, 1)); \
	__sync_synchronize();
#define UNLOCK(name) \
	__sync_synchronize(); \
	name = 0;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(Concurrency)

    class Lock
    {
    public:
        void Aquire()
        {
            asm volatile(
                ".intel_syntax noprefix;"
                ".aquire_lock:"
                "lock bts [%0], 1;"
                "jc .spin_wait;"
                ".spin_wait:"
                "test dword [%0], 1;"
                "jnz .spin_wait;"
                "jmp .aquire_lock;"
            
                : "=r"(_lock)
            );        
        }

        void Release()
        {
            asm volatile(
                "lock bts [%0], 0;"
                : "=r"(_lock)
            );
        }

    private:
        volatile bool _lock;
    };

    NAMESPACE_END(Concurrency)

NAMESPACE_END(kernel)

#endif // KERNEL_LOCK_H