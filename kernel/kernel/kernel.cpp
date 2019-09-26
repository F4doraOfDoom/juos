//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <include/dtables_structs.h>

#include <stdio.h>

#define TIMER_PORT 32

void divide_by_zero(void*);

__NO_MANGELING void kernel_main(void) {
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();
	timer::initialize(100);

	interrupts::set_handler(TIMER_PORT, [](void*) {
		printf("hello!");
	});
	interrupts::set_handler(0, divide_by_zero);

	printf("Hello world!");
	// asm volatile(
	// 	"movl $1, %eax;"
	// 	"movl $0, %ebx;"
	// 	"divl %ebx;"
	// );
}

void divide_by_zero(void* r)
{
	//tty::writestring("STOP! YOU VIOLATED THE LAW!");
	printf("Divide by zero! %c", ((registers32_t*)r)->eax);
}