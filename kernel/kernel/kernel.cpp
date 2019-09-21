//#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/descriptors_tables.h>

__NO_MANGELING void kernel_main(void) {
	Tty::initialize();
	Gdt::initialize();

	Tty::writestring("hello world");
    //printf("Hello, kernel World!\n");
}