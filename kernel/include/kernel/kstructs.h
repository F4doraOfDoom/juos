/**
 * @file kstructs.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file for general kernel structs, that don't have a specific place
 * to be at. If you're looking for a certain struct, you're likely to find it in a header
 * file thats more closely related to the subject of the struct.
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_STRUCTS_H_
#define KERNEL_STRUCTS_H_

#include <stdint.h>
#include "kdef.h"

struct RegistersStruct_x86_32
{
    uint32_t ds; // Data segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // all the registers at time of interrupt
    uint32_t int_no, err_code; // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss; // pushed by processor
} __PACKED;

typedef RegistersStruct_x86_32 registers32_t;

#endif //KERNEL_STRUCTS_H