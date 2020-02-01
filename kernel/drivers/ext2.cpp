#include <drivers/ext2.h>

using namespace ext2;

static SuperBlock global_super_block;

// static buffer the size of SECTOR_SIZE_BYTES, for reading writing sectors
// static char     ext2_sector_buffer[SECTOR_SIZE_BYTES] = { 0 };
// buffer the size of block_size in bytes, for reading/writing block
static uint8_t*     ext2_block_buffer = nullptr;
//static uint8_t      __fs_buffer[SECTOR_SIZE_BYTES];

// how many sectors are in a block
static uint32_t sectors_per_block = 0;

static inline uint32_t BLOCK_TO_SECTOR(uint32_t block, uint32_t block_size)
{
    return (block * (block_size / SECTOR_SIZE_BYTES));
}

static inline void ZERO_BUFFER(uint8_t* buffer, size_t size)
{
    memset(buffer, '\x00', size);
}

static inline uint32_t BYTES_TO_BLOCKS(uint32_t bytes)
{
    return bytes / global_super_block.block_size;
}

static inline uint32_t INODE_IDX_TO_BLOCK_GROUP(uint32_t idx)
{
    return ((idx - 1) / global_super_block.group_inode_size) 
        + SUPER_BLOCK_IDX + 1; // first block_group is at the block after the super_block
}

static inline uint32_t INODE_IDX_IN_BLOCK_GROUP(uint32_t idx)
{
    return (idx - 1) % global_super_block.group_inode_size;
}

static inline uint32_t INODE_IDX_CONTAINING_BLOCK(uint32_t idx)
{
    return (idx * sizeof(Inode)) / global_super_block.block_size;
}

Fs::Fs(kernel::StorageDeviceHandler* storage_device, const FsDescriptor& descriptor)
                                                                : _storage_device(storage_device),
                                                                   _info(descriptor) 
{
    ASSERT(descriptor.block_size % SECTOR_SIZE_BYTES == 0);

#ifdef K_LOG_EXT2
    LOG_SA("EXT2: ", "Initializing... StorageDevice: %p, descriptor: %p\n", storage_device, descriptor);
#endif

    ext2_block_buffer = new uint8_t[descriptor.block_size];
    memset(_file_inodes_cache, 0, sizeof(_file_inodes_cache));
    
    sectors_per_block = (descriptor.block_size / SECTOR_SIZE_BYTES);

    _GetObjectOptions opts;
#ifdef K_NEW_STORAGE
    opts = _GetObjectOptions::ForceCreate;
#else
    opts = _GetObjectOptions::CreateIfNotFound;
#endif
    // get the super block from block 1
    global_super_block = _get_super_block(SUPER_BLOCK_IDX, opts, &descriptor);
    
    // check if first block group exists, if not, create
    auto block_group = _get_block_group((SUPER_BLOCK_IDX + 1), opts);
    block_group->block_bitmap = block_group->block_bitmap;

    _file_inodes_cache[0].inode_idx = 1;
    memcpy(_file_inodes_cache[0].name, "root", 5); 

    delete block_group;
}

void Fs::create_file(const char* filename)
{
    auto inode_idx = _get_available_inode_idx();

#ifdef K_LOG_EXT2
    LOG_SA("EXT2: ", "Create file request (name: %s, inode: %d)\n", filename, inode_idx);
#endif

    ASSERT(inode_idx != ((uint32_t)-1));
    ASSERT(strlen(filename) < FILENAME_LENGTH);

    memcpy(_file_inodes_cache[inode_idx].name, filename, strlen(filename));

    auto bg_idx = INODE_IDX_TO_BLOCK_GROUP(inode_idx);
    auto bg = _get_block_group(bg_idx, _GetObjectOptions::CreateIfNotFound);

    auto inode = &bg->inode_table[INODE_IDX_TO_BLOCK_GROUP(inode_idx)];

    printf("%d\n", inode->user_id);
    inode->user_id = 0x49;
    inode->size_lower = 0x1111;

    bg->inode_bitmap[INODE_IDX_IN_BLOCK_GROUP(inode_idx)] = 1;

    _write_block_group(bg, bg_idx);
}

void Fs::_write_block_group(const BlockGroupTable* bg, uint32_t block_idx)
{
    const auto& descriptor = bg->group_descriptor;

    _write_object_to_block(&bg->group_descriptor, block_idx, sizeof(BlockGroupDescriptor));

    _write_storage(bg->block_bitmap, descriptor.block_bitmap_block, BYTES_TO_BLOCKS(_info.block_size));
    _write_storage(bg->inode_bitmap, descriptor.inode_bitmap_block, BYTES_TO_BLOCKS(_info.block_size));
    _write_storage((uint8_t*)(bg->inode_table), descriptor.inode_table_start_block, BYTES_TO_BLOCKS(global_super_block.group_inode_size * sizeof(Inode)));
}

uint32_t Fs::_get_available_inode_idx()
{
    for (uint32_t i = 0; i < MAX_FILES; i++)
    {
        if (_file_inodes_cache[i].inode_idx == 0)
        {
            return i + 1;
        }
    }

    return -1;
}

SuperBlock Fs::_get_super_block(uint32_t sb_block_idx, _GetObjectOptions opt, const FsDescriptor* descriptor)
{
    SuperBlock super_block;

    if (opt == _GetObjectOptions::ForceCreate)
    {
#ifdef K_LOG_EXT2
        LOG_SA("EXT2: ", "Creating new Superblock at block %d\n", sb_block_idx);
#endif
        ASSERT(descriptor != nullptr);
        memset(&super_block, 0, sizeof(SuperBlock));

        super_block.total_inodes        = -1;
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

        _write_object_to_block(&super_block, 1, sizeof(SuperBlock));
    }
    else
    {
        _read_object_from_block(&super_block, 1, sizeof(SuperBlock));
    
        if (opt == _GetObjectOptions::NoCreate)
        {
            ASSERT(super_block.ext2_sign != 0);
        }
        else if (opt == _GetObjectOptions::CreateIfNotFound)
        {
#ifdef K_LOG_EXT2
    LOG_S("EXT2: ", "Super block not found, creating new...\n");
#endif
            super_block = _get_super_block(sb_block_idx, _GetObjectOptions::ForceCreate, descriptor);
        }

    }

    return super_block;
}

BlockGroupTable* Fs::_get_block_group(uint32_t block_idx, _GetObjectOptions opt)
{
    BlockGroupTable* block_group = nullptr;

    if (opt == _GetObjectOptions::ForceCreate)
    {
#ifdef K_LOG_EXT2
        LOG_SA("EXT2: ", "Creating new block group at block %d\n", block_idx);
#endif
        block_group = new BlockGroupTable(false);

        /**
         *  +----------------+---------------+--------------+-------------------------+
            |                |               |              |                         |
            |                |               |              |                         |
            |                |               |              |                         |
            | Block Group    |  Block Bitmap | Inode Bitmap |   Inode table           |
            | Descriptor     |               |              |                         |
            |                |               |              |                         |
            |n               |n+1            |n+2           |n+3                      |
            +----------------+---------------+--------------+-------------------------+
         */
        block_group->group_descriptor.block_bitmap_block         = block_idx + 1;
        block_group->group_descriptor.inode_bitmap_block         = block_idx + 2;
        block_group->group_descriptor.inode_table_start_block    = block_idx + 3;
        block_group->group_descriptor.group_n_unalloc_blocks     = _info.block_size;
        block_group->group_descriptor.group_n_unalloc_inodes     = _info.block_size;
        block_group->group_descriptor.n_dirs_in_group            = 0;

        _write_object_to_block(&block_group->group_descriptor, block_idx, sizeof(BlockGroupDescriptor));
        _zero_block(block_group->group_descriptor.block_bitmap_block);
        _zero_block(block_group->group_descriptor.inode_bitmap_block);
        _zero_block(block_group->group_descriptor.inode_table_start_block);
    }
    else
    {
        block_group = new BlockGroupTable(true);

        _read_object_from_block(&block_group->group_descriptor, block_idx, sizeof(BlockGroupDescriptor));
        
        if (opt == _GetObjectOptions::NoCreate)
        {
            ASSERT(block_group->group_descriptor.block_bitmap_block != 0);
            ASSERT(block_group->group_descriptor.inode_bitmap_block != 0);
        } 
        else if (opt == _GetObjectOptions::CreateIfNotFound)
        {
#ifdef K_LOG_EXT2
    LOG_SA("EXT2: ", "Block group %d not found, creating new...\n", block_idx);
#endif
            if (block_group->group_descriptor.block_bitmap_block == 0 || 
                block_group->group_descriptor.inode_bitmap_block == 0)
            {
                block_group = _get_block_group(block_idx, _GetObjectOptions::ForceCreate);
            }
        }
        
        block_group->block_bitmap = new uint8_t[_info.block_size];
        block_group->inode_bitmap = new uint8_t[_info.block_size];
        block_group->inode_table = new Inode[global_super_block.group_inode_size];

        //memset(block_group->block_bitmap, 'A', 0x400);
        _read_storage(block_group->block_bitmap, block_group->group_descriptor.block_bitmap_block, 1);
        _read_storage(block_group->inode_bitmap, block_group->group_descriptor.inode_bitmap_block, 1);

        uint32_t inode_table_block_size = (global_super_block.group_inode_size * sizeof(Inode)) / _info.block_size;
        _read_storage((uint8_t*)block_group->inode_table, block_group->group_descriptor.inode_table_start_block, inode_table_block_size);
    }

    return block_group;
}

void Fs::_write_object_to_block(const void* obj, uint32_t block, uint32_t obj_size)
{
    ZERO_BUFFER(ext2_block_buffer, _info.block_size);
    memcpy(ext2_block_buffer, obj, obj_size);
    _write_storage(ext2_block_buffer, block, 1);
}


void Fs::_read_object_from_block(void* obj, uint32_t block, uint32_t obj_size)
{
    ZERO_BUFFER(ext2_block_buffer, _info.block_size);
    _read_storage(ext2_block_buffer, block, 1);
    memcpy(obj, ext2_block_buffer, obj_size);
}

void Fs::_read_storage(uint8_t* buffer, uint32_t block, uint32_t n_blocks)
{    
    auto n_sectors      = sectors_per_block * n_blocks;
    auto sector_start   = BLOCK_TO_SECTOR(block, _info.block_size);

    for (uint32_t i = 0; i < n_sectors; i++)
    {
        _storage_device->read_sectors(buffer + (i * SECTOR_SIZE_BYTES), sector_start + i, 0);
    }
}

void Fs::_write_storage(const uint8_t* buffer, uint32_t block, uint32_t n_blocks)
{
    auto n_sectors      = sectors_per_block * n_blocks;
    auto sector_start   = BLOCK_TO_SECTOR(block, _info.block_size);

    for (uint32_t i = 0; i < n_sectors; i++)
    {
        _storage_device->write_sectors(buffer + (i * SECTOR_SIZE_BYTES), sector_start + i, 0);
    }
}

void Fs::_zero_block(uint32_t block)
{
    ZERO_BUFFER(ext2_block_buffer, _info.block_size);
    _write_storage(ext2_block_buffer, block, 1);
}

Fs::~Fs()
{
    delete[] ext2_block_buffer;
}