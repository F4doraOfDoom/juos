#include <drivers/ata.h>

using namespace ata;

Bus ata::selected_controller;

void ata::initialize()
{
    FindDeviceResult device = find_devices();

    if (device.device_found == false)
    {
        GO_PANIC("No ATA device was found", "");
    }
}

FindDeviceResult ata::find_devices()
{
    FindDeviceResult res;

    SELECT(Bus::Primary);

    IO_REG_SET(IoRegister::LBAlow, 0);
    IO_REG_SET(IoRegister::LBAmid, 0);
    IO_REG_SET(IoRegister::LBAhigh, 0);

    COMMAND(Command::Identify);    

    uint32_t device_propreties = READ(IoRegister::Status);

    if (device_propreties == 0)
    {
        res.device_found = false;
    }
    else
    {
        res.device_found = true;
        printf("Status: %x %d %b\n", device_propreties, device_propreties, device_propreties);
    }

    return res;
}

