#ifndef DRIVER_ATA_H_
#define DRIVER_ATA_H_

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/timer.h>

#include <stdint.h>

NAMESPACE_BEGIN(ata)

    MACRO(PRIMARY_IO_BASE,       0x1F0); 
    MACRO(PRIMARY_COMMAND_BASE,  0x3F7);
    MACRO(PRIMARY_SELECT,        0xA0);

    MACRO(SECONDARY_IO_BASE,        0x170); 
    MACRO(SECONDARY_COMMAND_BASE,   0x376);
    MACRO(SECONDARY_SELECT,         0xB0);

    // same as Controller with Master/Slave
    enum class Bus : bool {
        Primary = true,
        Secondary = false
    };

    // saves which controller was last selected
    extern Bus selected_controller;

    enum class IoRegister : uint32_t {
        // name     // offset from
        Data            = 0,
        Error           = 1,
        Features        = 1,
        SectorCount     = 2,

        // different names for same ports
        SectorNumber    = 3, LBAlow  = 3,
        CylinderLow     = 4, LBAmid  = 4, 
        CylinderHigh    = 5, LBAhigh = 5,
        
        Head            = 6,
        Status          = 7,
        Command         = 7
    };

    enum class CommandRegister : uint32_t {
        AlternateStatus = 0,
        DeviceControl = 0,
        DriveAddress = 1
    };

    enum class Command : uint8_t {
        Identify = 0xEC
    };

    /**
     * @brief Get the IO base value of bus _bus_ 
     */
    inline uint32_t IO_BASE(Bus bus)
    {
        return (bus == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE);   
    }


    /**
     * @brief Get the command base value of bus _bus_ 
     */
    inline uint32_t COMMAND_BASE(Bus bus)
    {
        return (bus == Bus::Primary ? PRIMARY_COMMAND_BASE : SECONDARY_COMMAND_BASE);
    }

    /**
     * @brief Get the offset of a register _reg_ from IO port base of bus _bus_ 
     * 
     * @param reg register or type IoRegister
     * @param bus bus to offset from
     * @return uint32_t 
     */
    inline uint32_t IO_REG_OFFSET(IoRegister reg, Bus bus) 
    {
        return static_cast<int>(reg) + IO_BASE(bus);

    }

    /**
     * @brief Set value _val_ in IO register _reg_
     * 
     * @param reg - IO register to modify
     * @param val - value to set
     */
    inline void IO_REG_SET(IoRegister reg, uint8_t val)
    {
        uint32_t offset = IO_REG_OFFSET(reg, selected_controller); 
        outb(offset, val);
    }

    /**
     * @brief Select a controller _bus_
     * 
     * @param bus bus to select
     */
    inline void SELECT(Bus bus)
    {
        selected_controller = bus;
        uint32_t port = (bus == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE);
        uint32_t val = (bus == Bus::Primary ? PRIMARY_SELECT : SECONDARY_SELECT);
        outb(port, val); 
    }

    /**
     * @brief Send command _cmd_ to the control register
     * 
     * @param cmd - command to send
     */
    inline void COMMAND(Command cmd)
    {
        uint32_t port = COMMAND_BASE(selected_controller) + static_cast<uint32_t>(CommandRegister::DeviceControl);
        uint8_t val = static_cast<uint32_t>(cmd);
        outb(port, val);
    }

    inline uint32_t READ(IoRegister reg)
    {
        uint32_t port = IO_REG_OFFSET(reg, selected_controller);
        
        return inb(port);
    }

    struct FindDeviceResult
    {
        bool device_found = false;
    };

    /**
     * @brief Initialize the ATA driver
     * 
     */
    void initialize();

    /**
     * @brief Will poll the buses to see if there are any ata devices
     * 
     */
    FindDeviceResult find_devices();

NAMESPACE_END(ata)

#endif //DRIVER_ATA_H_