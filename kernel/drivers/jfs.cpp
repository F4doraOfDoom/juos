#include <drivers/jfs.h>
#include <kernel/lock.h>
#include <algorithms.hpp>

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

void JuosFileSystem::_AddItemToDirectory(uint32_t dir_data_begin_sec, const String& inode_name, uint32_t inode_sector, bool is_dir)
{
    char buffer[SECTOR_SIZE] = { 0 };

    _ReadFromStorage(_storage_handler, dir_data_begin_sec, buffer, sizeof(buffer));

    DirectoryData* dir_data = (DirectoryData*)buffer;

    for (uint32_t i = 0; i < 32; i++)
    {
        // if unused
        if (dir_data->children[i].sector == 0)
        {
            strncpy(dir_data->children[i].child_name, inode_name.c_str(), inode_name.getLength());
            dir_data->children[i].sector = inode_sector;
            dir_data->children[i].is_dir = is_dir;
            break;
        }
    }

    _WriteToStorage(_storage_handler, dir_data_begin_sec, buffer, sizeof(buffer));
}

void JuosFileSystem::CreateFile(const String& name, const Path& path)
{
    if (name.getLength() > FILE_NAME_SIZE)
    {
#if defined(K_LOG_JFS)
        LOG_SA("JFS: ", "Filename %s exceedes max size (%d)\n", name, FILE_NAME_SIZE);
#endif
        return;
    }

    //assert path exists
    auto base_dir = _FindDirectory(path);

    if (base_dir == nullptr)
    {
        printf("Path %s not found.\n", path.ToString().c_str());
        return;
    }

    auto new_sector = _FindFirstAvailableSector();

    auto file_inode = new_sector.is_end ? new FileInode() : 
                (new_sector.prev_inode->type == InodeType::File ? (FileInode*)new_sector.prev_inode : new FileInode() );

    file_inode->base.next           = new_sector.is_end ? 0 : new_sector.previous_next;
    file_inode->base.type = InodeType::File;
    file_inode->base.disk_size      = 10; // default file size of 10
    file_inode->base.struct_size    = sizeof(FileInode);
    file_inode->base.data_size      = 0;
    file_inode->base.state          = InodeState::Used; 
    file_inode->father              = base_dir->self_sector;

    file_inode->base.self_sector = new_sector.next_sector; 
    memcpy(file_inode->base.name, name.c_str(), sizeof(file_inode->base.name)); 

    // if the new inode is not on the end of the linked list, we will be reusing the inode 
    // used by the previous file, so no need to push it back into the inode list 
    if (new_sector.is_end)
    {
        _inodes->push_back(reinterpret_cast<InodeBase*>(file_inode));
    }

    _WriteToStorage(_storage_handler, new_sector.next_sector, file_inode, sizeof(FileInode));

    // now update previous inode in linked list
    if (new_sector.prev_inode && new_sector.is_end)
    {
        printf("Updating previous inode at sector %d\n", new_sector.prev_inode->self_sector);
        new_sector.prev_inode->next = new_sector.next_sector;
        _WriteToStorage(_storage_handler, new_sector.prev_inode->self_sector, new_sector.prev_inode, sizeof(FileInode));
    }

    // update the current directory
    _AddItemToDirectory(base_dir->self_sector + 1, name, new_sector.next_sector, false);

    printf("Wrote new file %s at sector %d\n", name.c_str(), new_sector.next_sector);
}

void JuosFileSystem::DeleteFile(const String& filename, const Path& path)
{
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return strcmp(f->name, filename.c_str()) == 0;
    });

    if (inode == nullptr)
    {
        printf("Could not find file named %s\n", filename.c_str());
        return;
    }

    (*inode)->state = InodeState::Unused;
    _WriteToStorage(_storage_handler, (*inode)->self_sector, *inode, (*inode)->struct_size);
}


void JuosFileSystem::_ListDirectoryChildren(uint32_t dir_sector)
{
    char buffer[SECTOR_SIZE] = { 0 };

    _ReadFromStorage(_storage_handler, dir_sector, buffer, SECTOR_SIZE);

    DirectoryData* dir_data = (DirectoryData*)buffer;

    for (uint32_t i = 0; i < 32; i++)
    {
        auto child = dir_data->children[i];
        if (child.sector)
        {
            printf("%s at %d %s ", child.child_name, child.sector, child.is_dir ? "(dir)" : "");
        }
    }
}

static DirectoryData _ReadDirectory(kernel::StorageDeviceHandler* storage, uint32_t dir_data_sector)
{
    char buffer[SECTOR_SIZE] = { 0 };

    _ReadFromStorage(storage, dir_data_sector, buffer, SECTOR_SIZE);

    DirectoryData* dir_data = (DirectoryData*)buffer;   

    return *dir_data;
}

void _TreeInt(kernel::StorageDeviceHandler* storage, uint32_t current_sector, const String& prev_name, uint32_t depth)
{
    char buffer[SECTOR_SIZE] = { 0 };
    DirectoryData dir_data;

    _ReadFromStorage(storage, current_sector, buffer, SECTOR_SIZE);

    InodeBase* inode = (InodeBase*)buffer;

    for (uint32_t i = 0; i < depth; i++)
    {
        printf("-");
    }

    switch (inode->type)
    {
        case InodeType::Directory:
            dir_data = _ReadDirectory(storage, inode->self_sector + 1);

            printf("%s\n", prev_name.c_str());
            for (uint32_t i = 0; i < 32; i++)
            {
                auto child = dir_data.children[i];
                if (child.sector)
                {
                    if (child.is_dir)
                    {
                        _TreeInt(storage, child.sector, child.child_name, depth++);
                    }
                    else
                    {
                        for (uint32_t i = 0; i < depth + 1; i++)
                        {
                            printf("-");
                        }
 
                        printf("%s\n", child.child_name);
                    }
                }
            }
        break;

        case InodeType::File:
            printf("%s\n", prev_name.c_str());
        break;

        default:
            printf("TREE: Unrecognized file type.\n");
        break;
    }
}

void JuosFileSystem::Tree(const Path& path)
{
    auto dir_base = _FindDirectory(path);

    if (dir_base == nullptr)
    {
        printf("Could not find path %d.\n", path.ToString().c_str());
    }

    _TreeInt(_storage_handler, dir_base->self_sector, "/", 0);
}



void JuosFileSystem::ReadFs(bool delete_cache)
{
    FsMeta meta;

    if (delete_cache) 
    {
        if (_inodes)
        {
            for (auto inode : *_inodes)
            {
                switch (inode->type)
                {
                    case InodeType::File:
                        delete (FileInode*)inode;
                    break;

                    default:
                        printf("Unknown inode type %d. Cant delete!\n", inode->type);
                    break;
                }
            }
        }
    }

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
            printf("Found inode at sector: %d, struct size %d, data size %d, ", inode->self_sector, inode->struct_size, inode->data_size);
            switch (inode->state)
            {

                case InodeState::Unused:
                    printf("type: unused, ");
                    if (delete_cache)
                    {
                        _inodes->push_back(new InodeBase(*inode));
                    }
                break;

                case InodeState::Used:
                    switch (inode->type)
                    {
                        case InodeType::File:
                            printf("type: file, name: %s, father: %d, ", ((FileInode*)inode)->base.name, ((FileInode*)inode)->father);
                            if (delete_cache)
                            {
                                _inodes->push_back((InodeBase*)new FileInode(*(FileInode*)inode));
                            }
                        break;

                        case InodeType::Directory:
                            printf("type: directory, name: %s, ", ((FileInode*)inode)->base.name);
                            printf("files = ");
                            _ListDirectoryChildren(inode->self_sector + 1);
                            printf("), ");

                            if (delete_cache)
                            {
                                _inodes->push_back((InodeBase*)new FileInode(*(FileInode*)inode));
                            }
                        break;

                        default:
                            printf("type: unknown, ");
                        break;
                    }
                break;

                default:
                    printf("unknown inode state %d, ", inode->state);
                break;
            }

            uint32_t next = inode->next; 
            printf("size: %d; Trying next: %d\n", inode->disk_size, next);
            next_inode_sector = next;
        }
        else
        {
            break;
        }
    }
}

void _PrintInodeFormat(const char* name, bool is_file, FilePermissions root, FilePermissions owner, FilePermissions other, uint32_t size, uint32_t unused)
{
    printf(is_file ? "-" : "d");
    printf("%c%c%c", root.execute ? 'x' : '-', root.write ? 'w' : '-', root.read ? 'r' : '-');
    printf("%c%c%c", owner.execute ? 'x' : '-', owner.write ? 'w' : '-', owner.read ? 'r' : '-');
    printf("%c%c%c", other.execute ? 'x' : '-', other.write ? 'w' : '-', other.read ? 'r' : '-');
    printf("          %d      ", size);
    printf("    %s%s\n", name, unused ? " (unused)" : "");
}

void JuosFileSystem::ListFs(const Path& path)
{
    printf("Permissions:        Size:       Name:\n");
    for (auto inode : *_inodes)
    {
        FileInode* file = (FileInode*)inode;
        _PrintInodeFormat(file->base.name, inode->type == InodeType::File, file->base.root, file->base.owner, file->base.other, file->base.disk_size, file->base.state == InodeState::Unused);
    }
}

bool JuosFileSystem::FileExists(const String& filename, const Path& path) const
{
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return strcmp(f->name, filename.c_str()) == 0;
    });

    return inode != nullptr;
 }

void JuosFileSystem::ReadFile(const String& filename, const Path& path, char* buffer, uint32_t max_size)
{
    // TODO refactor
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return strcmp(f->name, filename.c_str()) == 0;
    });

    if (inode == nullptr)
    {
        printf("Could not find file named %s\n", filename.c_str());
        return;
    }

    uint32_t data_location = (*inode)->self_sector + 1;
    _ReadFromStorage(_storage_handler, data_location, buffer, max_size);
}

void JuosFileSystem::CreateDirectory(const String& name, const Path& path)
{
if (name.getLength() > FILE_NAME_SIZE)
    {
#if defined(K_LOG_JFS)
        LOG_SA("JFS: ", "Directory %s exceedes max size (%d)\n", name, FILE_NAME_SIZE);
#endif
        return;
    }

    //assert path exists
    auto base_dir = _FindDirectory(path);

    if (base_dir == nullptr && path.components.size() > 0)
    {
        printf("Path %s not found.\n", path.ToString().c_str());
        return;
    }

    auto new_sector = _FindFirstAvailableSector();

    auto dir_inode = new_sector.is_end ? new DirectoryInode() : 
                (new_sector.prev_inode->type == InodeType::Directory ? (DirectoryInode*)new_sector.prev_inode : new DirectoryInode());

    dir_inode->base.next           = new_sector.is_end ? 0 : new_sector.previous_next;
    dir_inode->base.type           = InodeType::Directory;
    dir_inode->base.disk_size      = 10; // default file size of 10
    dir_inode->base.struct_size    = sizeof(FileInode);
    dir_inode->base.data_size      = 0;
    dir_inode->base.state          = InodeState::Used; 

    dir_inode->base.self_sector = new_sector.next_sector; 
    memcpy(dir_inode->base.name, name.c_str(), sizeof(dir_inode->base.name)); 

    // if the new inode is not on the end of the linked list, we will be reusing the inode 
    // used by the previous file, so no need to push it back into the inode list 
    if (new_sector.is_end)
    {
        _inodes->push_back(reinterpret_cast<InodeBase*>(dir_inode));
    }

    _WriteToStorage(_storage_handler, new_sector.next_sector, dir_inode, sizeof(FileInode));

    // now update previous inode in linked list
    if (new_sector.prev_inode && new_sector.is_end)
    {
        printf("Updating previous inode at sector %d\n", new_sector.prev_inode->self_sector);
        new_sector.prev_inode->next = new_sector.next_sector;
        _WriteToStorage(_storage_handler, new_sector.prev_inode->self_sector, new_sector.prev_inode, sizeof(FileInode));
    }

    if (base_dir)
    {
        // update the current directory
        _AddItemToDirectory(base_dir->self_sector + 1, name, new_sector.next_sector, true);
    }

    printf("Wrote new file %s at sector %d\n", name.c_str(), new_sector.next_sector);
}

InodeBase* JuosFileSystem::_GetCachedInodeBySector(uint32_t sector)
{
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return f->self_sector == sector;
    });   

    if (inode)
    {
        return *inode;
    }

    return nullptr;
}

InodeBase* JuosFileSystem::_FindDirectoryInt(kernel::StorageDeviceHandler* storage, const Path& path, uint32_t current_inode_sector, uint32_t path_index)
{

    String current_search;

    if (path_index > path.components.size())
    {
        return nullptr;
    }
    else
    {
        current_search = path.components[path_index];
    }

    InodeBase current_inode;
    _ReadFromStorage(storage, current_inode_sector, &current_inode, sizeof(InodeBase));

    if (current_inode.type == InodeType::Directory)
    {
        char buffer[512] = { 0 };
        _ReadFromStorage(storage, current_inode.self_sector + 1, buffer, 512);

        DirectoryData* directory_data = (DirectoryData*)buffer;
        
        for (uint32_t i = 0; i < 32; i++)
        {
            auto child = directory_data->children[i];

            if (child.is_dir)
            {
                // recurse
                if (String(child.child_name).compare(current_search))
                {
                    if ((path_index + 1) == path.components.size())
                    {
                        return _GetCachedInodeBySector(current_inode.self_sector); 
                    }
                    else
                    {
                        return _FindDirectoryInt(storage, path, child.sector, path_index++);
                    }
                }
            }
        }
    }

    return nullptr;
} 

InodeBase* JuosFileSystem::_FindDirectory(const Path& path)
{
    auto current_path = Path(path);
    auto current_inode = (*_inodes)[0]; // get the root of the file system tree

    if ((*_inodes).size() == 0)
    {
        return nullptr;
    }

    if (path.components.size() == 1 && path.components[0].compare("ROOT"))
    {
        return current_inode;
    }
   
    return _FindDirectoryInt(_storage_handler, path, current_inode->self_sector, 1);
}

InodeBase* JuosFileSystem::_GetCachedInodeByName(const String& filename)
{
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return strcmp(f->name, filename.c_str()) == 0;
    });

    if (inode)
    {
        return *inode;
    }

    return nullptr;
}


void JuosFileSystem::WriteFile(const String& filename, const Path& path, const String& text)
{
    auto inode = std::find_if(_inodes->begin(), _inodes->end(), [&](auto& f) {
        return strcmp(f->name, filename.c_str()) == 0;
    });

    if (inode == nullptr)
    {
        printf("Could not find file named %s\n", filename.c_str());
        return;
    }

    uint32_t data_begin = (*inode)->self_sector + 1; 
    _WriteToStorage(_storage_handler, data_begin, (void*)text.c_str(), text.getLength());

    // update inode's size
    (*inode)->data_size = text.getLength();
    _WriteToStorage(_storage_handler, (*inode)->self_sector, *inode, (*inode)->struct_size);

    printf("Wrote %d bytes to file %s (sector %d)\n", text.getLength(), (*inode)->name, data_begin);
}

JuosFileSystem::_FindFirstResponse JuosFileSystem::_FindFirstAvailableSector()
{
    JuosFileSystem::_FindFirstResponse res;
    res.next_sector = _fs_meta.start_block;
    res.prev_inode = nullptr;
    res.is_end = true;

    for (const auto& inode : *_inodes)
    {
        res.prev_inode = inode;
        if (inode->next == 0)
        {
            res.next_sector = inode->self_sector + inode->disk_size + 1;
            res.is_end = true;
            break;
        }
        else if (inode->state == InodeState::Unused)
        {
            res.next_sector = inode->self_sector;
            res.is_end = false;
            res.previous_next = inode->next;
            break;
        }
    }

    return res;
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

    CreateDirectory("ROOT", {});
}

JuosFileSystem::~JuosFileSystem()
{
    delete _inodes;
}