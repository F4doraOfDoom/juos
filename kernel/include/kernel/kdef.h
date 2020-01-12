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

#define NOT     !
#define OR      ||
#define AND     &&  

#define MACRO(name, val)            constexpr uint32_t name = val
#define MACRO_T(type, name, val)    consexptr type name = val

#define __NO_MANGELING          extern "C"

#define GO_PANIC(message, ...)                  \
        printf("KERNEL PANIC!\n"); \
        printf(message, __VA_ARGS__);                       \
        printf("File: " __FILE__ "\n");                       \
        printf("Line: %d\n", __LINE__);                       \
        while(1) { asm volatile (""); }         \

#define ASSERT(condition)   \
    if (!(condition)) {GO_PANIC("Failed Assertion: " #condition "\n", "")}

#define NAMESPACE_BEGIN(name)   namespace name {
#define NAMESPACE_END(name)     }

#define CHECK_LOG_LEVEL(flag, level) (flag >= level)

// Set up arch flags
#ifdef __i386__
#define ARCH _i386_
#endif 

/*
    Compiler specific
*/
#ifdef __GNUC__ 
    #pragma GCC diagnostic ignored "-Wattributes"
    #define __PACKED        __attribute__((packed))
    #define __IS_USED       __attribute__((used))
    #define __ALIGNED(x)    __attribute__((aligned(x)))
    #define __MAYBE_UNUSED  __attribute__((unused))
    #define __IGNORE(warning)   GCC diagnostic ignored "warning"
#else
    // add flags from different compilers
#endif


#endif