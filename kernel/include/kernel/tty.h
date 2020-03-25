/**
 * @file Tty.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and structs related to output
 * to the Tty
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H
 
#include <stddef.h>
#include <kernel/kdef.h> 

NAMESPACE_BEGIN(Tty)

    /**
     * @brief this function initializes the Tty (output screen)
     * 
     */
    void Initialize(void);

    /**
     * @brief this function outputs a single char into the Tty
     * 
     * @param c - char to output
     */
    void Putchar(char c);

    /**
     * @brief this function writes _size_ bytes of _data_ to Tty
     * 
     * @param data - data to Write
     * @param size - number of bytes to Write from data
     */
    void Write(const char* data, size_t size);

    /**
     * @brief this function writes a null terminated string to Tty
     * 
     * @param data - null terminated string
     */
    void WriteString(const char* data);

    /**
     * @brief this function cleans the Tty, leaving blank chars 
     * 
     */
    void Clean(); 


    /**
     * @brief Set the printing color scheme
     * 
     * @param fg - fore ground color
     * @param bg - back ground color
     */
    void SetColor(unsigned int fg, unsigned int bg);

NAMESPACE_END(Tty)

#endif