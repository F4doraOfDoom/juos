//#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/descriptors_tables.h>
#include <kernel/interrupts.h>

#include <stdio.h>

void divide_by_zero(void*);

__NO_MANGELING void kernel_main(void) {
	tty::initialize();
	gdt::initialize();
	idt::initialize();
	interrupts::initialize();

	interrupts::set_handler(0, divide_by_zero);

	tty::writestring("Hello world!");
	asm volatile(
		"movl $1, %eax;"
		"movl $0, %ebx;"
		"divl %ebx;"
	);
    //printf("Hello, kernel World!\n");
}

void divide_by_zero(void*)
{
	//tty::writestring("STOP! YOU VIOLATED THE LAW!");
	putchar(65);
}