#include <drivers/ext2.h>

using namespace ext2;

static SuperBlock global_super_block;
static char ext2_buffer[SECTOR_SIZE_BYTES] = { 0 };

// how many sectors are in a block
static uint32_t sectors_per_block = 0;

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

    global_super_block = _get_super_block(1);
}

SuperBlock Fs::_get_super_block(uint32_t sb_block_idx, bool create_new, const FsDescriptor* descriptor)
{
    SuperBlock super_block;

    if (create_new)
    {
#ifdef K_LOG_EXT2
        LOG_SA("EXT2: ", "Creating new Superblock at block %d\n", sb_block_idx);
#endif
        ASSERT(descriptor != nullptr);
        memset(&super_block, 0, sizeof(SuperBlock));

        super_block.total_inodes        = 0;
        super_block.total_inodes        = 0;
        super_block.n_su_blocks         = 0;
        super_block.unallocated_blocks  = 0;
        super_block.unallocated_inodes  = 0;
        super_block.sb_block            = sb_block_idx;
        super_block.block_size          = descriptor->block_size;
        super_block.group_block_size    = descriptor->block_size;
        super_block.group_inode_size    = descriptor->block_size;
        
        // fragments not supported
        super_block.fragment_size       = -1;
        super_block.group_fragment_size = -1;
        
        // time not supported
        super_block.last_mount_time     = -1;
        super_block.last_written_time   = -1;

        // fsck not supported
        super_block.mount_since_last_ck = -1;
        super_block.n_mounts_before_ck  = -1;
        super_block.last_ck_time        = -1;
        super_block.forced_ck_interval  = -1;

        super_block.ext2_sign           = EXT2_SIGN;
        super_block.fs_state            = FsState::Clean;
        super_block.err_handling        = ErrorHandleGuide::Panic;
        
        // lol
        super_block.version_minor       = 0x69;
        super_block.version_major       = 0x420;
        super_block.os_id               = OsId::Other;

        // id not supported
        super_block.uid_can_reserve     = -1;
        super_block.gid_can_reserve     = -1;

        sectors_per_block = (descriptor->block_size / SECTOR_SIZE_BYTES);

        memcpy(ext2_buffer, &super_block, sizeof(SuperBlock));
        _write_storage(ext2_buffer, 1, 1);
    }
    else
    {
        _read_storage(ext2_buffer, sb_block_idx, 1);
        memcpy(&super_block, ext2_buffer, sizeof(super_block));
    }

    return super_block;
}

void Fs::_read_storage(char* buffer, uint32_t block, uint32_t n_blocks)
{    
    _storage_device->read_sectors(buffer, BLOCK_TO_SECTOR(block, _info.block_size), sectors_per_block * n_blocks);
}

void Fs::_write_storage(const char* buffer, uint32_t block, uint32_t n_blocks)
{
    _storage_device->write_sectors(buffer, BLOCK_TO_SECTOR(block, _info.block_size), sectors_per_block * n_blocks);
}