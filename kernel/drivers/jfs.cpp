#include <drivers/jfs.h>
#include <kernel/lock.h>

using namespace drivers::jfs;

MACRO(SECTOR_SIZE, 512);

static uint8_t _write_buffer[SECTOR_SIZE] = { 0 };
static DECLARE_LOCK(_write_lock);

// doesn't support objects bigger than 512 bytes
static void _WriteToStorage(kernel::StorageDeviceHandler* storage, uint32_t lba, void* object, size_t object_size)
{
    LOCK(_write_lock);

    memset(_write_buffer, 0, SECTOR_SIZE);
    memcpy(_write_buffer, object, object_size);
    storage->WriteSectors(_write_buffer, lba, 1);

    UNLOCK(_write_lock);
}

JuosFileSystem::JuosFileSystem(kernel::StorageDeviceHandler* storage_handler)
    : _storage_handler(storage_handler)
{
    _inodes = new Vector<InodeBase*>();
}

void JuosFileSystem::CreateFile(const char* name)
{
    if (strlen(name) > FILE_NAME_SIZE)
    {
#if defined(K_LOG_JFS)
        LOG_SA("JFS: ", "Filename %s exceedes max size (%d)\n", name, FILE_NAME_SIZE);
#endif
        return;
    }

    auto file_inode = new FileInode();
    file_inode->base.type = InodeType::File;

    _inodes->push_back(reinterpret_cast<InodeBase*>(file_inode));
}

void JuosFileSystem::DeleteFile(const char* name)
{

}

void JuosFileSystem::MakeNewFs()
{
    _fs_meta.start_block = 2;

    _WriteToStorage(_storage_handler, _fs_meta.start_block, &_fs_meta, sizeof(_fs_meta));

    if (_inodes) 
    {
        for (auto inode : *_inodes)
        {
            delete inode;
        }

        delete _inodes;
    }

    _inodes = new Vector<InodeBase*>();
}

JuosFileSystem::~JuosFileSystem()
{
    delete _inodes;
}