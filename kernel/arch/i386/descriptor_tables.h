#ifndef KERNEL_DESCRIPTOR_STRUCTS
#define KERNEL_DESCRIPTOR_STRUCTS

#include <kernel/descriptors.h>
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

    struct entry_struct
    {
        uint16_t    limit_low; //lower 16 bits of base
        uint16_t    base_low; //The base address of the segment 
        uint8_t     base_middle; //middle 8 bits of base
        uint8_t     access; // access flags specifiying protection level
        uint8_t     granularity;
        uint8_t     base_high;
    } __PACKED;

    struct ptr_struct
    {
        uint16_t    size; 
        uint32_t    base;
    } __PACKED;

    typedef entry_struct entry_t;
    typedef Segment_Access_Type Gsat;

    /*
        This function initializes the GDT table and writes it to the CPU
    */
    static void init();

    /*
        This function configures the GDT
        Input:
            int32_t     entry   - The index of the GDT entry
            uint32_t    base    - The base address of the GDT entry
            uint32_t    limit   - The upper limit of the GDT entry
            Gsat        access  - Entry access flags
            uint8_t     gran    - granuary byte
    */
    static void config_entry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);               

NAMESPACE_END(Gdt)

/*
    Written in a different assembly file.
    Writes a given GDT into the CPU.
*/
__NO_MANGELING void gdt_dump(uint32_t table);

#endif //KERNEL_DESCRIPTOR_STRUCTS