//#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/descriptors.h>

__NO_MANGELING void kernel_main(void) {
	terminal_initialize();
	initialize_gdt();

	terminal_writestring("hello world");
    //printf("Hello, kernel World!\n");
}