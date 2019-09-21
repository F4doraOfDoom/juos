#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
 
#include <kernel/tty.h>

#include "include/vgh.h"

static void _terminal_setcolor(uint8_t color);
static void _terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y);

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
 
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
 
void Tty::initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = _vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = _vga_entry(' ', terminal_color);
		}
	}
} 
 
void Tty::putchar(char c) 
{
	unsigned char uc = c;
	_terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
		{
			terminal_row = 0;
		}
	}
}

void Tty::write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		putchar(data[i]);
}
 
void Tty::writestring(const char* data) 
{
	write(data, strlen(data));
}

void _terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void _terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = _vga_entry(c, color);
}
 