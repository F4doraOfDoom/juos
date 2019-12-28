#ifndef DRIVER_ATA_H_
#define DRIVER_ATA_H_

#include <kernel/kdef.h>

NAMESPACE_BEGIN(ata)

    /**
     * @brief Initialize the ATA driver
     * 
     */
    void initialize();

NAMESPACE_END(ata)

#endif //DRIVER_ATA_H_