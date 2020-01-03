#ifndef DRIVER_ATA_H_
#define DRIVER_ATA_H_

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/timer.h>
#include <kernel/kuseful.h>

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

    enum class StatusRegisterState : uint8_t {
        ERR  = 0,   	// Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
        IDX  = 1, 		// Index. Always set to zero.
        CORR = 2, 	 	// Corrected data. Always set to zero.
        DRQ  = 3, 		// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
        SRV  = 4, 		// Overlapped Mode Service Request.
        DF 	 = 5, 	    // Drive Fault Error (does not set ERR).
        RDY  = 6, 		// Bit is clear when drive is spun down, or after an error. Set otherwise.
        BSY  = 7 		// Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset. 
    };

    enum class CommandRegister : uint32_t {
        AlternateStatus = 0,
        DeviceControl = 0,
        DriveAddress = 1
    };

    enum class Command : uint8_t {
        Identify = 0xEC
    };

    union StatusRegister
    {
        struct StatusFlags {
            uint8_t Err  : 1;   	// Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
            uint8_t Idx  : 1; 		// Index. Always set to zero.
            uint8_t Corr : 1; 	 	// Corrected data. Always set to zero.
            uint8_t Drq  : 1; 		// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
            uint8_t Srv  : 1; 		// Overlapped Mode Service Request.
            uint8_t Df 	 : 1; 	    // Drive Fault Error (does not set ERR).
            uint8_t Rdy  : 1; 		// Bit is clear when drive is spun down, or after an error. Set otherwise.
            uint8_t Bsy  : 1;		// Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset. 
        } flags;

        uint8_t value;
    };

    struct DeviceInfoResult
    {
        bool        found                   = false;
        bool        not_ata                 = false;
        bool        is_hard_disk            = false;
        bool        LBA48_supported         = false;
        uint16_t    supported_UDMA_modes    = 0;
        bool        conductor_cable_found   = false;
        struct __PACKED {
            uint16_t low;
            uint16_t high;
        }  LBA28_sectors                     = { 0 };
        
        struct __PACKED {
            uint16_t low;
            uint16_t low_mid;
            uint16_t mid_high;
            uint16_t high;
        }    LBA48_sectors                  = { 0 };

    };

    struct FindDeviceResult
    {
        DeviceInfoResult master;
        DeviceInfoResult slave;
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