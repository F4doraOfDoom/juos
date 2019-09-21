#ifndef KERNEL_DESCRIPTOR_TABLES_STRUCTS_I386
#define KERNEL_DESCRIPTOR_TABLES_STRUCTS_I386

#include <kernel/descriptors_tables.h>
#include <kernel/kdef.h>
#include <stdint.h>

NAMESPACE_BEGIN(Gdt)

    enum class Segment_Access_Type : uint8_t
    {
        Kernel_Code  = 0x9A,
        Kernel_Data  = 0x92,
        User_Code    = 0xFA,
        User_Data    = 0XF2
    };

    struct EntryStruct
    {
        uint16_t    limit_low; //lower 16 bits of base
        uint16_t    base_low; //The base address of the segment 
        uint8_t     base_middle; //middle 8 bits of base
        uint8_t     access; // access flags specifiying protection level
        uint8_t     granularity;
        uint8_t     base_high;
    } __PACKED;

    struct PtrStruct
    {
        uint16_t    size; 
        uint32_t    base;
    } __PACKED;

    typedef EntryStruct entry_t;
    typedef Segment_Access_Type Gsat;

NAMESPACE_END(Gdt)


NAMESPACE_BEGIN(Idt)

    struct EntryStruct
    {
        uint16_t base_low; // lower 16 bits of base addr
        uint16_t sel; // segment selector
        uint8_t zero; // should always be zero
        uint8_t flags; // entry flags
        uint16_t base_high; // higher 16 bits of base addr
    } __PACKED;

    struct PtrStruct
    {
        uint16_t size;
        uint32_t base;
    } __PACKED;

    struct RegistersStruct
    {
        uint32_t ds; // Data segment
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // all the registers at time of interrupt
        uint32_t int_no, err_code; // Interrupt number and error code
        uint32_t eip, cs, eflags, useresp, ss; // pushed by processor
    } __PACKED;

    typedef EntryStruct entry_t;
    typedef RegistersStruct registers_t;

NAMESPACE_END(Idt)

#endif //KERNEL_DESCRIPTOR_STRUCTS