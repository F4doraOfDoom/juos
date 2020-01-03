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

union Bits32 
{
    struct 
    {
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
        uint8_t bit8 : 1;
        uint8_t bit9 : 1;
        uint8_t bit10 : 1;
        uint8_t bit11 : 1;
        uint8_t bit12 : 1;
        uint8_t bit13 : 1;
        uint8_t bit14 : 1;
        uint8_t bit15 : 1;
        uint8_t bit16 : 1;
        uint8_t bit17 : 1;
        uint8_t bit18 : 1;
        uint8_t bit19 : 1;
        uint8_t bit20 : 1;
        uint8_t bit21 : 1;
        uint8_t bit22 : 1;
        uint8_t bit23 : 1;
        uint8_t bit24 : 1;
        uint8_t bit25 : 1;
        uint8_t bit26 : 1;
        uint8_t bit27 : 1;
        uint8_t bit28 : 1;
        uint8_t bit29 : 1;
        uint8_t bit30 : 1;
        uint8_t bit31 : 1;
        uint8_t bit32 : 1;
    } bits;

    uint32_t value;
};

#endif //KERNEL_STRUCTS_H