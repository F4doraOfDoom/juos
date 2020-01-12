#include <drivers/ata.h>

using namespace ata;

//static char ata_buffer[512] = { 0 };
//PRDT prdt;

static ata::Bus selected_controller;

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
    uint32_t offset = IO_REG_OFFSET(reg, ::selected_controller); 
    outb(offset, val);
}

/**
 * @brief Select a controller _bus_
 * 
 * @param bus bus to select
 */
inline void SELECT(Bus bus)
{
    ::selected_controller = bus;
    uint32_t port = (bus == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE);
    uint32_t val = (bus == Bus::Primary ? PRIMARY_SELECT : SECONDARY_SELECT);
    outb(port, val); 
}


inline uint8_t READ_BYTE(IoRegister reg)
{
    uint16_t port = IO_REG_OFFSET(reg, ::selected_controller);
    
    return inb(port);
}

inline uint16_t READ_WORD(IoRegister reg)
{
    uint16_t port = IO_REG_OFFSET(reg, ::selected_controller);

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
 * @return - 0 if succeded, status register value otherwise
 */
inline bool COMMAND(Command cmd)
{
    WAIT_NO_BUSY();

    WAIT_UNTIL_READY();

    uint32_t port = (::selected_controller == Bus::Primary ? PRIMARY_IO_BASE : SECONDARY_IO_BASE) + ((int)IoRegister::Command);
    uint8_t val = static_cast<uint8_t>(cmd);
    outb(port, val);

    auto status = READ_BYTE(IoRegister::Status);
    return status;
}

inline void DISABLE_INTERRUPTS()
{
    outb(INTERRUPT_REGISTER, 0x2);
}

inline void RESET_COMMAND_REGISTER()
{
    COMMAND((ata::Command)0);
}

inline void SET_HEAD(uint32_t lba, uint32_t slavebit)
{
    outb(0x1F6, (0xE0 | (slavebit <<  4) | (lba >> 24 & 0x0F))); 
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
    IO_REG_SET(IoRegister::LBAlow, (unsigned char)lba);
    IO_REG_SET(IoRegister::LBAmid, (unsigned char)(lba >> 8));
    IO_REG_SET(IoRegister::LBAhigh, (unsigned char)(lba >> 16));
}

static DeviceInfoResult _get_device_info(Bus bus);

Device ata::create_device()
{
#ifdef K_LOG_GENERAL
    LOG_S("ATA Driver: ", "Initializing...\n");
#endif 

    FindDeviceResult device_info = find_devices();

    if (device_info.master.found == false && device_info.slave.found == false)
    {
        GO_PANIC("No ATA device was found!\n", "");
    }
    else
    {
        if (device_info.master.not_ata && device_info.slave.not_ata)
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
    device_info.master.is_hard_disk,
    device_info.master.LBA48_supported,
    device_info.master.supported_UDMA_modes,
    device_info.master.conductor_cable_found,
    device_info.master.LBA28_sectors,
    device_info.master.LBA48_sectors
    );
#endif
    } 

    // kind of ignore slave
    return Device(DeviceType::Master, device_info.master);
}

FindDeviceResult ata::find_devices()
{
    return {
        _get_device_info(Bus::Primary),
        // currently we'll use only the primary device
       {} //2 _get_device_info(Bus::Secondary)
    };
}

Device::Device(DeviceType type, const DeviceInfoResult& res) : 
        _device_type(type),
        _bus_type(Bus::Primary),
        _lba_28_sectors(res.LBA28_sectors.value),
        _lba_48_sectors(res.LBA48_sectors.value)
{
}

bool Device::read_bytes(char* output, uint32_t location, uint32_t count)
{
    auto sectors = ALIGN_VAL(count, SECTOR_SIZE_BYTES) / SECTOR_SIZE_BYTES;
    auto lba = (int)(location / SECTOR_SIZE_BYTES);

    char* buffer = new char[sectors * SECTOR_SIZE_BYTES];
    //char buffer[512] = {0};

    bool success = read_sectors(buffer, lba, sectors);

    int offset = (location == 0 ? 0 : location % SECTOR_SIZE_BYTES);

    memcpy(output, buffer + offset, count);

    delete[] buffer;

    return success;
}

bool Device::write_bytes(const char* input, uint32_t location, uint32_t count)
{
    auto sectors = ALIGN_VAL(count, SECTOR_SIZE_BYTES) / SECTOR_SIZE_BYTES;
    auto lba = (int)(location / SECTOR_SIZE_BYTES);

    char* buffer = new char[sectors * SECTOR_SIZE_BYTES];
    
    read_sectors(buffer, lba, sectors);

    int offset = (location == 0 ? 0 : location % SECTOR_SIZE_BYTES);

    memcpy(buffer + offset, input, count);

    bool success = write_sectors(buffer, lba, sectors);

    delete[] buffer;

    return success;
}

bool Device::read_sectors(char* buffer, uint32_t LBA, uint32_t sectors)
{
    char slavebit =0;   //0 for master device 1 for slave   sets bit 5 in 1f6

    DISABLE_INTERRUPTS();

    SET_HEAD(LBA, slavebit);

    DELAY_400_NS();

    SET_SECTOR_COUNT(sectors);
    timer::sleep(1);

    SET_LBA(LBA);

    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    COMMAND(Command::ReadSectorsWithRetry);
    timer::sleep(1);
    
    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    rep_insw(IO_REG_OFFSET(IoRegister::Data, _bus_type), buffer, 256);
    
    return READ_BYTE(IoRegister::Status);
}
 
bool Device::write_sectors(const char* buffer, uint32_t LBA, uint32_t sectors)
{
    DISABLE_INTERRUPTS();

    SET_HEAD(LBA, 0);

    DELAY_400_NS();

    SET_SECTOR_COUNT(sectors);
    timer::sleep(1);

    SET_LBA(LBA);
    
    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    COMMAND(Command::WriteSectorsWithRetry);
    timer::sleep(1);
    
    WAIT_NO_BUSY();
    WAIT_UNTIL_READY();

    READ_BYTE(IoRegister::Status);

    rep_outsw(0x1f0, buffer, 256);

    COMMAND(Command::CacheFlush);

    return READ_BYTE(IoRegister::Status);
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
        device_propreties.LBA28_sectors.sectors.low     = device_information[60];
        device_propreties.LBA28_sectors.sectors.high    = device_information[61];

        // the number of sectors is made 64 bit long - made out of 4, 16 bit numbers;
        device_propreties.LBA48_sectors.sectors.low         = device_information[100];
        device_propreties.LBA48_sectors.sectors.low_mid     = device_information[101];
        device_propreties.LBA48_sectors.sectors.mid_high    = device_information[102];
        device_propreties.LBA48_sectors.sectors.high        = device_information[103];

        return device_propreties;
    }

    // please no angry gcc
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    return { .found = false };
}

