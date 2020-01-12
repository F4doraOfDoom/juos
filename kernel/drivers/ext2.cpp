#include <drivers/ext2.h>

using namespace ext2;

SuperBlock super_block;
static char ext2_buffer[SECTOR_SIZE_BYTES] = { 0 };

static inline uint32_t BLOCK_TO_SECTOR(uint32_t block, uint32_t block_size)
{
    return (block * (block_size / SECTOR_SIZE_BYTES));
}

Fs::Fs(kernel::StorageDeviceHandler* storage_device, const FsDescriptor& descriptor)
                                                                : _storage_device(storage_device),
                                                                   _info(descriptor) 
{
    ASSERT(descriptor.block_size % SECTOR_SIZE_BYTES == 0);

    /**
     * Initialize the file system
     */

    memset(&super_block, 0, sizeof(SuperBlock));

    super_block.ext2_sign = EXT2_SIGN;
    super_block.total_inodes = 100;
    super_block.fs_state = FsState::HasErrors;

    memcpy(ext2_buffer, &super_block, sizeof(SuperBlock));
    _write_storage(ext2_buffer, 1, 1);

}

void Fs::_read_storage(char* buffer, uint32_t block, uint32_t n)
{    
    _storage_device->read_sectors(buffer, BLOCK_TO_SECTOR(block, _info.block_size), n);
}

void Fs::_write_storage(const char* buffer, uint32_t block, uint32_t n)
{
    _storage_device->write_sectors(buffer, BLOCK_TO_SECTOR(block, _info.block_size), n);
}