/**
 * @file kcommon.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing common functions for the kernel
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_COMMON_H_
#define KERNEL_COMMON_H_

#include "kdef.h"
#include <stdint.h>

/*
    Common port IO tasks
*/
__NO_MANGELING inline void outb(short port, char value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

__NO_MANGELING inline uint8_t inb(short port) {
   uint8_t ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

__NO_MANGELING inline uint16_t inw(short port) {
   uint16_t ret;
   asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

#endif // KERNEL_COMMON_H_