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