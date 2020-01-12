//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <kernel/paging.h>
#include <kernel/kheap.h>
#include <kernel/klog.h>
#include <kernel/kmm.h>
#include <kernel/knew.hpp>
#include <kernel/kuseful.h>
#include <drivers/ata.h>
#include <drivers/ext2.h>

#include <kernel/kstdcxx.hpp>
#include <list.hpp>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);
__NO_MANGELING void kernel_main(void);
__NO_MANGELING void __cxa_pure_virtual();

struct Something
{
	Something()
	{
		memcpy(arr, "Hello new world!\n", 16);
	}

	char arr[16] = { 0 };
};

__NO_MANGELING void kernel_main(void) {
	/**
	 * The initialization order is important 
	 * 1. tty
	 * 2. (gdt/ idt)
	 * 3. interrupts
	 * 4. memory managment
	 * everything else... 
	 *
	 */
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();

	// after this initialization, operators new and delete can be used
	memory_manager::initialize(
		K_HEAP_START, // beginning address
		K_HEAP_START + K_HEAP_INITIAL_SIZE, // end address
		0xCFFFF000, // max address
		false, // running in supervisor mode
		true // is read-write
	);

	timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	printf("Hello EXT2!\n");

	//static_assert(sizeof(ext2::InodeTypeAndPermission) == 2);
	printf("Sizeof: %d\n", OFFSET_OF(ext2::SuperBlock, ext2_sign));

	for (;;)
	{
		asm volatile (
			"hlt;"
		);
	}


}

void divide_by_zero(void* r)
{
	LOG_A("Divide by zero! %d", ((registers32_t*)r)->eax);
}

// enable pure virtual functions
__NO_MANGELING void __cxa_pure_virtual() {}