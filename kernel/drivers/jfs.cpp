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

static void _ReadFromStorage(kernel::StorageDeviceHandler* storage, uint32_t lba, void* object, size_t object_size)
{
    LOCK(_write_lock);

    memset(_write_buffer, 0, SECTOR_SIZE);
    storage->ReadSectors(_write_buffer, lba, 1);
    memcpy(object, _write_buffer, object_size);

    UNLOCK(_write_lock);
}

JuosFileSystem::JuosFileSystem(kernel::StorageDeviceHandler* storage_handler)
    : _storage_handler(storage_handler)
{
    _inodes = new Vector<InodeBase*>();
}

void JuosFileSystem::CreateFile(const String& name)
{
    if (name.getLength() > FILE_NAME_SIZE)
    {
#if defined(K_LOG_JFS)
        LOG_SA("JFS: ", "Filename %s exceedes max size (%d)\n", name, FILE_NAME_SIZE);
#endif
        return;
    }

    auto file_inode = new FileInode();
    file_inode->base.type = InodeType::File;

    auto new_sector = _FindFirstAvailableSector();

    file_inode->base.next = 0;
    file_inode->base.size = 10; // default file size of 10

    file_inode->base.self_sector = new_sector.next_sector;
    memcpy(file_inode->name, name.c_str(), sizeof(file_inode->name)); 

    _inodes->push_back(reinterpret_cast<InodeBase*>(file_inode));
    _WriteToStorage(_storage_handler, new_sector.next_sector, file_inode, sizeof(FileInode));

    // now update previous inode in linked list
    if (new_sector.prev_inode)
    {
        printf("Updating previous inode at sector %d\n", new_sector.prev_inode->self_sector);
        //new_sector.prev_inode->next = new_sector.next_sector;
        //_WriteToStorage(_storage_handler, new_sector.prev_inode->self_sector, new_sector.prev_inode, sizeof(FileInode));
    }
    

    printf("Wrote new file %s at sector %d\n", name.c_str(), new_sector.next_sector);
}

void JuosFileSystem::ReadFs()
{
    FsMeta meta;

    _ReadFromStorage(this->_storage_handler, 1, &meta, sizeof(FsMeta));

    if (meta.magic != META_MAGIC)
    {
        printf("FATAL: Meta block not found, trying default starting inode...\n");
        meta.start_block = 2;
    }
    else
    {
        printf("INFO: Found meta block... Starting block = %d\n", meta.start_block);
    }

    uint32_t next_inode_sector = meta.start_block;
    while (next_inode_sector)
    {
        char buffer[SECTOR_SIZE] = { 0 };
        InodeBase* inode = (InodeBase*)buffer;
        _ReadFromStorage(_storage_handler, next_inode_sector, buffer, SECTOR_SIZE);

        if (inode->magic == INODE_MAGIC)
        {
            printf("Found inode at sector: %d, ", inode->self_sector);
            switch (inode->type)
            {
                case InodeType::File:
                    printf("type: file, name: %s, ", ((FileInode*)inode)->name);
                break;

                default:
                    printf("type: unknown, ");
                break;

            }

            uint32_t next = inode->self_sector + inode->size + 1; 
            printf("size: %d; Trying next: %d\n", inode->size, next);
            next_inode_sector = next;
        }
        else
        {
            break;
        }
    }
}

void _PrintInodeFormat(const char* name, bool is_file, FilePermissions root, FilePermissions owner, FilePermissions other, uint32_t size)
{
    printf(is_file ? "-" : "d");
    printf("%c%c%c", root.execute ? 'x' : '-', root.write ? 'w' : '-', root.read ? 'r' : '-');
    printf("%c%c%c", owner.execute ? 'x' : '-', owner.write ? 'w' : '-', owner.read ? 'r' : '-');
    printf("%c%c%c", other.execute ? 'x' : '-', other.write ? 'w' : '-', other.read ? 'r' : '-');
    printf("          %d      ", size);
    printf("    %s\n", name);
}

void JuosFileSystem::ListFs()
{
    printf("Permissions:        Size:       Name:\n");
    for (auto inode : *_inodes)
    {
        FileInode* file = (FileInode*)inode;
        _PrintInodeFormat(file->name, true, file->base.root, file->base.owner, file->base.other, file->base.size);
    }
}

JuosFileSystem::_FindFirstResponse JuosFileSystem::_FindFirstAvailableSector()
{
    JuosFileSystem::_FindFirstResponse res;
    res.next_sector = _fs_meta.start_block;
    res.prev_inode = nullptr;

    for (const auto& inode : *_inodes)
    {
        if (inode->next == 0)
        {
            res.next_sector = inode->self_sector + inode->size + 1;
            res.prev_inode  = inode;
        }
    }

    return res;
}

void JuosFileSystem::DeleteFile(const String& name)
{

}

void JuosFileSystem::MakeNewFs()
{
    _fs_meta.start_block = 2;

    // fs meta should be found at sector 1
    _WriteToStorage(_storage_handler, 1, &_fs_meta, sizeof(_fs_meta));

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