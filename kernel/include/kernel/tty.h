#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H
 
#include <stddef.h>
#include <kernel/kdef.h> 

NAMESPACE_BEGIN(tty)

/**
 * @brief this function initializes the tty (output screen)
 * 
 */
void initialize(void);

/**
 * @brief this function outputs a single char into the tty
 * 
 * @param c - char to output
 */
void putchar(char c);

/**
 * @brief this function writes _size_ bytes of _data_ to tty
 * 
 * @param data - data to write
 * @param size - number of bytes to write from data
 */
void write(const char* data, size_t size);

/**
 * @brief this function writes a null terminated string to tty
 * 
 * @param data - null terminated string
 */
void writestring(const char* data);

/**
 * @brief this function cleans the tty, leaving blank chars 
 * 
 */
void clean(); 

NAMESPACE_END(tty)

#endif