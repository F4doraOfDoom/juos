#ifndef KERNEL_STORAGE_H_
#define KERNEL_STORAGE_H_

#include <stdint.h>

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

        /**
         * @brief All drivers handling storage (such as an ATA driver for the HDD),
         * must implement this interface, as it is used by the FS implementation to read/Write
         * sectors to the storage medium.
         * 
         */
        struct StorageDeviceHandler
        {
                /**
                 * @brief Read a _sectors_ count of sectors from LBA _lba_ to _buffer_
                 * 
                 * @param buffer - buffer to read into
                 * @param lba - logical base address of sectors to read from
                 * @param sectors - amount of sectors
                 * @return true if read is successful (not really working though)
                 * @return false if read is unsuccessful
                 */
                virtual bool ReadSectors(uint8_t* buffer, uint32_t lba, uint32_t sectors) = 0;

                /**
                 * @brief Write a _sectors_ count of sectors to LBA _lba_ from _buffer_
                 * 
                 * @param buffer - buffer to Write from
                 * @param lba - logical base address of sectors to Write from
                 * @param sectors - amount of sectors
                 * @return true if Write is successful(not really working though)
                 * @return false if Write is unsuccessful
                 */
                virtual bool WriteSectors(const uint8_t* buffer, uint32_t lba, uint32_t sectors) = 0;

                virtual ~StorageDeviceHandler() = default;
        };

NAMESPACE_END(kernel)

#endif // KERNEL_STORAGE_H_