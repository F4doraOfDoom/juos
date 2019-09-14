//#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/kdef.h>

NO_MANGELING void kernel_main(void) {
	terminal_initialize();
	terminal_writestring("hello world");
    //printf("Hello, kernel World!\n");
}