/**
 * @file kdef.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing kernel definitions
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_DEFINES_H
#define KERNEL_DEFINES_H

#include <stddef.h>
#include <stdio.h>

#define __NO_MANGELING          extern "C"

#define GO_PANIC(message, ...)                  \
        printf(message, __VA_ARGS__);                       \
        printf("File: " __FILE__ "\n");                       \
        printf("Line: %c\n", __LINE__ + 45);                       \
        while(1) { asm volatile (""); }         \  

#define NAMESPACE_BEGIN(name)   namespace name {
#define NAMESPACE_END(name)     }

// Set up arch flags
#ifdef __i386__
#define ARCH _i386_
#endif 

/*
    Compiler specific
*/
#ifdef __GNUC__
    #define __PACKED    __attribute__((packed))
    #define __IS_USED   __attribute__((used))
#else
    // add flags from different compilers
#endif


#endif