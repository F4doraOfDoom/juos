#ifndef DRIVER_ATA_H_
#define DRIVER_ATA_H_

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/timer.h>
#include <kernel/kuseful.h>
#include <kernel/storage.h>

#include <stdint.h>
#include <string.h>

NAMESPACE_BEGIN(ata)

    MACRO(PRIMARY_IO_BASE,       0x1F0); 
    MACRO(PRIMARY_COMMAND_BASE,  0x3F7);
    MACRO(PRIMARY_SELECT,        0xA0);

    MACRO(SECONDARY_IO_BASE,        0x170); 
    MACRO(SECONDARY_COMMAND_BASE,   0x376);
    MACRO(SECONDARY_SELECT,         0xB0);

    MACRO(SECTOR_SIZE_BYTES,         512);
    MACRO(SECTOR_SIZE_WORDS,        SECTOR_SIZE_BYTES/2);  

    MACRO(INTERRUPT_REGISTER,       0x3F6);

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
        Head            = 6, Drive   = 6,

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

    enum class DeviceType : uint8_t {
        Master = 0,
        Slave = 1
    };

    enum class Command : uint8_t {
        Identify = 0xEC,
        ReadBuffer = 0xE4,
        WriteBuffer = 0xE8,
        ReadSectorsWithRetry = 0x20,
        WriteSectorsWithRetry = 0x30,
        ReadDMA28 = 0xC8,
        ReadDMA48 = 0x25,
        WriteDMA28 = 0xCA,
        WriteDMA48 = 0x35,
        CacheFlush = 0xE7,
        Standby = 0xE0,
        StandbyLBA28 = (Standby | 0x40), // bit 6 enables for LBA 28 bit
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
    } __PACKED;

    struct PRDT {
        uint32_t buffer_phys;
        uint16_t transfer_size;
        uint16_t mark_end;
    } __PACKED;

    struct DeviceInfoResult
    {
        bool        found                   = false;
        bool        not_ata                 = false;
        bool        is_hard_disk            = false;
        bool        LBA48_supported         = false;
        uint16_t    supported_UDMA_modes    = 0;
        bool        conductor_cable_found   = false;
        union
        {
            struct __PACKED {
                uint16_t low;
                uint16_t high;
            }  sectors                     = { 0, 0 };
            uint32_t value;
        }   LBA28_sectors;
        
        union 
        {
            struct __PACKED {
                uint16_t low;
                uint16_t low_mid;
                uint16_t mid_high;
                uint16_t high;
            }  sectors                  = { 0, 0, 0, 0 };
            uint64_t value;
        } LBA48_sectors;

    };

    struct FindDeviceResult
    {
        DeviceInfoResult master;
        DeviceInfoResult slave;
    };
 
    class Device : public kernel::StorageDeviceHandler
    {
    private:
        struct Request {
            uint8_t* buffer;
            uint32_t lba;
            uint32_t sectors;
        };

    public:
        using ReadRequest = Request;
        using WriteRequest = Request;

        Device(DeviceType type, const DeviceInfoResult& info);

        Device(const Device&) = delete;
        Device(Device&) = delete;
        void operator=(Device&) = delete;
        void operator=(const Device&) = delete;

        /**
         * @brief DEPRECATED - use read_sectors instead
         * 
         * @param buffer 
         * @param loc 
         * @param count 
         * @return true 
         * @return false 
         */
        bool read_bytes(uint8_t* buffer, uint32_t loc, uint32_t count);

        /**
         * @brief DEPRECATED - user write_sectors instead
         * 
         * @param buffer 
         * @param loc 
         * @param count 
         * @return true 
         * @return false 
         */
        bool write_bytes(const uint8_t* buffer, uint32_t loc, uint32_t count);

        /**
         * @brief Read a _sectors_ count of sectors from LBA _lba_ to _buffer_
         * 
         * @param buffer - buffer to read into
         * @param lba - logical base address of sectors to read from
         * @param sectors - amount of sectors
         * @return true if read is successful (not really working though)
         * @return false if read is unsuccessful
         */
        virtual bool ReadSectors(uint8_t* buffer, uint32_t lba, uint32_t sectors) override;

         /**
         * @brief Write a _sectors_ count of sectors to LBA _lba_ from _buffer_
         * 
         * @param buffer - buffer to Write from
         * @param lba - logical base address of sectors to Write from
         * @param sectors - amount of sectors
         * @return true if Write is successful(not really working though)
         * @return false if Write is unsuccessful
         */
        virtual bool WriteSectors(const uint8_t* buffer, uint32_t lba, uint32_t sectors) override;

    private:
        //uint32_t    _int_buffer[512] = { 0 };
        
        const DeviceType  _device_type;
        const Bus         _bus_type;  
        const uint32_t    _lba_28_sectors;
        const uint64_t    _lba_48_sectors;
    };

    /**
     * @brief Initialize the ATA driver
     * 
     */
    template <class AtaDeviceType>
    [[nodiscard]] AtaDeviceType* create_device();

    /**
     * @brief Will poll the buses to see if there are any ata devices
     * 
     */
    FindDeviceResult find_devices();

NAMESPACE_END(ata)

#endif //DRIVER_ATA_H_