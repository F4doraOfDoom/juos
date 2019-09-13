//#include <stdio.h>

#include <kernel/tty.h>

int main(void) {
	terminal_initialize();
	terminal_writestring("hello world");
    //printf("Hello, kernel World!\n");

    return 0;
}