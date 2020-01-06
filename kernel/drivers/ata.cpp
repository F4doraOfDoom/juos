#include <drivers/ata.h>

using namespace ata;

Bus ata::selected_controller;
//static char ata_buffer[512] = { 0 };
//PRDT prdt;

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
    return static_cast<int>(reg) + (bus == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE);
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


inline uint8_t READ_BYTE(IoRegister reg)
{
    uint16_t port = IO_REG_OFFSET(reg, selected_controller);
    
    return inb(port);
}

inline uint16_t READ_WORD(IoRegister reg)
{
    uint16_t port = IO_REG_OFFSET(reg, selected_controller);

    return inw(port);
}

/**
 * @brief Wait until the status register's BSY bit is off
 * 
 */
inline void WAIT_NO_BUSY()
{
    StatusRegister status;
    do {
        status.value = READ_BYTE(IoRegister::Status);
    } while (status.flags.Bsy);
}

/**
 * @brief Wait until the status register's RDY bit is set
 * 
 */
inline void WAIT_UNTIL_READY()
{
    StatusRegister status;
    do {
        status.value = READ_BYTE(IoRegister::Status);
    } while (status.flags.Rdy == 0);
}

/**
 * @brief Send command _cmd_ to the control register
 * 
 * @param cmd - command to send
 */
inline void COMMAND(Command cmd)
{
    WAIT_NO_BUSY();

    WAIT_UNTIL_READY();

    uint32_t port = (selected_controller == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE) + ((int)IoRegister::Command);
    uint8_t val = static_cast<uint8_t>(cmd);
    outb(port, val);

}

inline void RESET_COMMAND_REGISTER()
{
    COMMAND((ata::Command)0);
}

inline void SET_SECTOR_COUNT(uint32_t count)
{
    IO_REG_SET(IoRegister::SectorCount, count);
}

inline void DELAY_400_NS()
{
    for (int i = 0; i < 4; i++) 
    {
        READ_BYTE(IoRegister::Status);
    }
}

inline void SET_LBA(uint32_t lba)
{
    IO_REG_SET(IoRegister::LBAlow, lba & 0x000000ff);
    IO_REG_SET(IoRegister::LBAmid, lba & 0x0000ff00 >> 8);
    IO_REG_SET(IoRegister::LBAhigh, lba & 0x00ff0000 >> 16);
}

static DeviceInfoResult _get_device_info(Bus bus);

void ata::initialize()
{
#ifdef K_LOG_GENERAL
    LOG_S("ATA Driver: ", "Initializing...\n");
#endif 

    FindDeviceResult device = find_devices();

    if (device.master.found == false && device.slave.found == false)
    {
        GO_PANIC("No ATA device was found!\n", "");
    }
    else
    {
        if (device.master.not_ata && device.slave.not_ata)
        {
            GO_PANIC("ATA: Device found, but not ATA compilant!\n", "");
        }

#ifdef K_LOG_GENERAL
    LOG_S("ATA Driver: ", "ATA device found!\n" )
#endif

#ifdef K_LOG_ATA
    LOG_SA("ATA Driver Information:\n",
    "Is HDD: %d\n"
    "LBA48 addressing mode available: %d\n"
    "Supported UDMA modes: %d\n"
    "Conductor cable found: %d\n"
    "LBA28 available sectors: %d\n"
    "LBA48 available sectors: %d\n",
    device.master.is_hard_disk,
    device.master.LBA48_supported,
    device.master.supported_UDMA_modes,
    device.master.conductor_cable_found,
    device.master.LBA28_sectors,
    device.master.LBA48_sectors
    );
#endif
    } 
}

FindDeviceResult ata::find_devices()
{
    return {
        _get_device_info(Bus::Primary),
        // currently we'll use only the primary device
       {} //2 _get_device_info(Bus::Secondary)
    };
}

void ata::read()
{
    asm volatile ("cli;");

    IO_REG_SET(IoRegister::Drive, static_cast<uint8_t>(Command::StandbyLBA28));
    SET_SECTOR_COUNT(1);
    SET_LBA(100);

    COMMAND(Command::ReadSectorsWithRetry);

    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    char buf[512] = { 0 };
    insw((unsigned short)IoRegister::Data, buf, 256);

    printf("data read: %s\n", buf);

    asm volatile ("sti;");
}

void ata::write(char* buf, size_t len)
{
    asm volatile ("cli;");

    IO_REG_SET(IoRegister::Drive, static_cast<uint8_t>(Command::StandbyLBA28));
    SET_SECTOR_COUNT(1);
    SET_LBA(100);

    COMMAND(Command::WriteSectorsWithRetry);

    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    char buff[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    outsw((unsigned short)IoRegister::Data, buff, 256);

    COMMAND(Command::CacheFlush);
    DELAY_400_NS();

    asm volatile ("sti;");
}

DeviceInfoResult _get_device_info(Bus bus)
{
    uint16_t device_information[256] = { 0 };
    DeviceInfoResult device_propreties;

    SELECT(bus);

    IO_REG_SET(IoRegister::LBAlow, 0);
    IO_REG_SET(IoRegister::LBAmid, 0);
    IO_REG_SET(IoRegister::LBAhigh, 0);

    COMMAND(Command::Identify); 

    StatusRegister status = { .value = READ_BYTE(IoRegister::Status) };  

    // status should be non zero if device exists
    if (status.value != 0)
    {
        device_propreties.found = true;

        // poll status until not not busy (bit 7 clears)
        while (status.flags.Bsy)
        {
            status.value = READ_BYTE(IoRegister::Status);
            printf("%d %x %b\n", status.value, status.value, status.value);
        }

        // if LBAmid or LBAhigh non zero, device is not ATA
        bool device_not_ata = READ_BYTE(IoRegister::LBAmid) && READ_BYTE(IoRegister::LBAhigh);

        if (device_not_ata)
        {
            device_propreties.not_ata = device_not_ata;
        }
        else
        {
            // continue polling until either DRQ or ERR sets
            while (status.flags.Drq != 0 AND status.flags.Err != 0)
            { 
                status.value = READ_BYTE(IoRegister::Status);
            }
        }
        
        // read 256, 16 bit values
        for (uint32_t i = 0; i < 256; i++)
        {
            device_information[i] = READ_WORD(IoRegister::Data);
        }

        device_propreties.is_hard_disk          = device_information[0];
        device_propreties.LBA48_supported       = BIT_IS_CLEAR(device_information[82], 10);
        device_propreties.supported_UDMA_modes  = device_information[88];
        device_propreties.conductor_cable_found = (bus == Bus::Primary) AND
                                                BIT_IS_CLEAR(device_information[93], 11);
        
        // the number of sectors is made 32 bit long - made out of 2, 16 bit numbers;
        device_propreties.LBA28_sectors.low     = device_information[60];
        device_propreties.LBA28_sectors.high    = device_information[61];

        // the number of sectors is made 64 bit long - made out of 4, 16 bit numbers;
        device_propreties.LBA48_sectors.low         = device_information[100];
        device_propreties.LBA48_sectors.low_mid     = device_information[101];
        device_propreties.LBA48_sectors.mid_high    = device_information[102];
        device_propreties.LBA48_sectors.high        = device_information[103];

        return device_propreties;
    }

    return { .found = false };
}

