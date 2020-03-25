#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/klog.h>
 
#include <kernel/tty.h>

#include <arch/i386/vgh.h>

static void _terminal_setcolor(uint8_t color) __MAYBE_UNUSED;
static void _terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y);

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
 
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void Tty::Initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = _vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;

	LOG_SA("TTY: ", "Initializing. Width: %d Height: %d\n", VGA_WIDTH, VGA_HEIGHT);
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = _vga_entry(' ', terminal_color);
		}
	}
} 

void Tty::SetColor(unsigned int fg, unsigned int bg)
{
	//LOG_A("TTY: setting terminal colors: %d, %d\n", fg, bg);
	terminal_color = _vga_entry_color((vga_color)fg, (vga_color)bg);
}
 
void Tty::Putchar(char c) 
{
	unsigned char uc = c;

	switch (uc)
	{
		case '\n':
			terminal_column = 0;
			terminal_row++;
		break;

		default:
			_terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
			// move the cursor to new position
			terminal_column++;
		break;
	}

	// check if cursor reached end
	if (terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0;
		terminal_row++;
	}
	if (terminal_row == VGA_HEIGHT)
	{
		Tty::Clean();
		terminal_row = 0;
	}
}

void Tty::Write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		Putchar(data[i]);
}
 
void Tty::WriteString(const char* data) 
{
	Write(data, strlen(data));
}

void Tty::Clean()
{
	for (size_t i = 0; i < VGA_WIDTH; i++)
	{
		for (size_t j = 0; j < VGA_HEIGHT; j++)
		{
			terminal_buffer[i + j * VGA_WIDTH] = 0;
		}
	}
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
 