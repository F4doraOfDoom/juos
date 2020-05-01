#ifndef DRIVER_JFS_H
#define DRIVER_JFS_H

#include <kernel/kdef.h>
#include <kernel/storage.h>
#include <kernel/klog.h>
#include <kernel/vfs.h>
#include <kernel/josh.h>

#include <kernel/data_structures/vector.hpp>

#include <string.h>

using kernel::vfs::FsHandler;
using kernel::vfs::Path;
using kernel::data_structures::Vector;

NAMESPACE_BEGIN(drivers)

    NAMESPACE_BEGIN(jfs)

        MACRO(INODE_MAGIC, 0xbabecafe);
        MACRO(META_MAGIC, 0xdeadbabe);
        MACRO(FILE_NAME_SIZE, 11);
        MACRO(UNUSED_INODE, -1);
        MACRO(LAST_INODE, 0);

        enum class InodeType
        {
            Undefined,

            File = 0xa,
            Directory,
        };

        enum class InodeState : uint32_t
        {
            Undefined, 

            Unused = 0xc,
            Used
        } __PACKED;

        struct FsMeta
        {
            // for vertification that the meta block really is there
            uint64_t            magic = META_MAGIC;

            // major version indicator 
            uint32_t            version_major   = 1;

            // sector # of the first block
            uint64_t            start_block     = 2;

            // minor version indicator
            uint32_t            version_minor   = 1;
        } __PACKED;

        struct FilePermissions
        {
            bool read;
            bool write;
            bool execute;
        } __PACKED;
        

        /**
         * @brief The basic fields shared by every inode
         * 
         */
        struct InodeBase
        {
            // for vertification that the inode exists
            const uint32_t      magic = INODE_MAGIC;
            
            // until we implement directories
            char                name[FILE_NAME_SIZE] = { 0 };

            // type of the inode
            InodeType           type = InodeType::Undefined;   

            // permissions
            FilePermissions     root    {true, true, true};     // by default -- RWX
            FilePermissions     owner   {false, true, true};    // by default -- RW
            FilePermissions     other   {false, false, true};   // by default -- R

            // sector # of the next inode       
            // if value is 0, there is no next block
            uint32_t            next = LAST_INODE;

            // the sector in which this inode is present
            uint32_t            self_sector = 0;            

            InodeState          state = InodeState::Unused;

            // how many sectors the inode takes
            uint32_t            disk_size = 0;

            // how many bytes the data takes
            uint32_t            data_size = 0;

            // we don't know the size of the real inode, because it could be one of multiple types,
            // (i.e. file or directory). This field remembers the real size of the inode.
            uint32_t            struct_size = 0;
        } __PACKED;

        struct FileInode
        {
            // base inode attributes
            InodeBase           base;

            // sector # of the directory the file is in 
            uint32_t            father;
        } __PACKED;

        struct DirectoryInode
        {
            // base inode attributes
            InodeBase           base;

            // sector # of the directory the file is in 
            uint32_t            father;
        } __PACKED;

        struct DirectoryData
        {
            struct ChildLocationPair
            {
                char        child_name[FILE_NAME_SIZE]  = { 0 };
                bool        is_dir                      = false;
                uint32_t    sector                      = 0; 
            } __PACKED;

            // TODO: For now we support only 512 byte directories
            ChildLocationPair       children[512 / sizeof(ChildLocationPair)];
        } __PACKED;

        class JuosFileSystem : public kernel::vfs::FsHandler
        {
        public:
            JuosFileSystem(kernel::StorageDeviceHandler* storage_handler);

            virtual void CreateFile(const String& filename, const Path& path) override;

            virtual void CreateDirectory(const String& dirname, const Path& path) override;

            virtual void WriteFile(const String& filename, const Path& path, const String& text) override;

            virtual bool FileExists(const String& filename, const Path& path) const override;

            virtual void ReadFile(const String& filename, const Path& path, char* buffer, uint32_t max_size);

            virtual void DeleteFile(const String& filename, const Path& path) override;

            void MakeNewFs() override;

            void ReadFs(bool delete_cache) override;

            void Tree(const Path& path) override;

            void ListFs(const Path& path) override;

            virtual ~JuosFileSystem();

        private:
            struct _FindFirstResponse {
                uint32_t            next_sector     = 0;
                InodeBase*          prev_inode      = 0;
                bool                is_end          = false;// is the inode at the end of the linked list?    
                uint32_t            previous_next   = 0; // next used by the preivous inode (only relevant when $is_end is false)
            };

            InodeBase* _GetCachedInodeBySector(uint32_t sector);

            bool       _PathExists(const Path& path);

            InodeBase* _GetCachedInodeByName(const String& name);

            InodeBase* _FindDirectory(const Path& path);

            void       _ListDirectoryChildren(uint32_t dir_sector);
            
            InodeBase* _FindDirectoryInt(kernel::StorageDeviceHandler* storage, const Path& path, uint32_t current_inode_sector, uint32_t path_index);

            void       _AddItemToDirectory(uint32_t dir_data_begin_sec, const String& inode_name, uint32_t inode_sector, bool is_dir);

            _FindFirstResponse _FindFirstAvailableSector();

            kernel::StorageDeviceHandler*   _storage_handler;
            Vector<InodeBase*>  *           _inodes; 
            FsMeta                          _fs_meta;   
        };

    NAMESPACE_END(jfs)

NAMESPACE_END(drivers)

#endif // DRIVER_JFS_H