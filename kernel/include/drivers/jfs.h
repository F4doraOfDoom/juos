#ifndef DRIVER_JFS_H
#define DRIVER_JFS_H

#include <kernel/kdef.h>
#include <kernel/storage.h>
#include <kernel/klog.h>
#include <kernel/vfs.h>
#include <kernel/josh.h>

#include <kernel/data_structures/vector.hpp>

#include <string.h>

using kernel::FsHandler;
using kernel::data_structures::Vector;

NAMESPACE_BEGIN(drivers)

    NAMESPACE_BEGIN(jfs)

        MACRO(INODE_MAGIC, 0xbabecafe);
        MACRO(META_MAGIC, 0xdeadbabe);
        MACRO(FILE_NAME_SIZE, 16);
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
            uint64_t            father;
        } __PACKED;

        struct DirectoryInode
        {
            // base inode attributes
            InodeBase           base;

            // sector # of the directory the directory is in 
            uint64_t            father;            
        };

        class JuosFileSystem : public FsHandler
        {
        public:
            JuosFileSystem(kernel::StorageDeviceHandler* storage_handler);

            virtual void CreateFile(const String& filename) override;

            virtual void CreateFile(const char* filename) override 
            {
                CreateFile(String(filename));
            };

            virtual void CreateDirectory(const String& path) override {}

            virtual void WriteFile(const String& filename, const String& text) override;

            virtual bool FileExists(const String& filename) const override;

            virtual void ReadFile(const String& filename, char* buffer, uint32_t max_size);

            virtual void DeleteFile(const String& filename) override;

            virtual void DeleteFile(const char* filename) override
            {
                DeleteFile(String(filename));
            }

            void MakeNewFs() override;

            void ReadFs(bool delete_cache) override;

            void ListFs() override;

            virtual ~JuosFileSystem();

        private:
            struct _FindFirstResponse {
                uint32_t            next_sector;
                InodeBase*    prev_inode;
            };

            _FindFirstResponse _FindFirstAvailableSector();

            kernel::StorageDeviceHandler*   _storage_handler;
            Vector<InodeBase*>  *           _inodes; 
            FsMeta                          _fs_meta;         
        };

    NAMESPACE_END(jfs)

NAMESPACE_END(drivers)

#endif // DRIVER_JFS_H