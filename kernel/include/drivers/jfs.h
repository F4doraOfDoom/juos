#ifndef DRIVER_JFS_H
#define DRIVER_JFS_H

#include <kernel/kdef.h>
#include <kernel/storage.h>
#include <kernel/klog.h>
#include <kernel/vfs.h>

#include <kernel/data_structures/vector.hpp>

#include <string.h>

using kernel::FsHandler;
using kernel::data_structures::Vector;

NAMESPACE_BEGIN(drivers)

    NAMESPACE_BEGIN(jfs)

        MACRO(INODE_MAGIC, 0xbabecafe);
        MACRO(META_MAGIC, 0xdeadbabe);
        MACRO(FILE_NAME_SIZE, 16);

        enum class InodeType
        {
            Undefined,

            File = 0xa,
            Directory,
        };

        struct FsMeta
        {
            // for vertification that the meta block really is there
            uint64_t            magic = META_MAGIC;

            // major version indicator 
            uint32_t            version_major = 1;

            // sector # of the first block
            uint64_t            start_block;

            // minor version indicator
            uint32_t            version_minor = 1;
        };

        /**
         * @brief The basic fields shared by every inode
         * 
         */
        struct InodeBase
        {
            // for vertification that the inode exists
            const uint32_t      magic = INODE_MAGIC;

            // type of the inode
            InodeType           type = InodeType::Undefined;   

            // sector # of the next inode       
            // if value is -1, block is considered unused
            uint64_t            next = (uint64_t)-1;

            // size in sectors
            uint64_t            size = 0;
        };

        struct FilePermissions
        {
            bool read;
            bool write;
            bool execute;
        };

        struct FileInode
        {
            // until we implement directories
            char                name[FILE_NAME_SIZE] = { 0 };

            // base inode attributes
            InodeBase           base;

            // sector # of the directory the file is in 
            uint64_t            father;
        
            FilePermissions     root    {true, true, true};     // by default -- RWX
            FilePermissions     owner   {false, true, true};    // by default -- RW
            FilePermissions     other   {false, false, true};   // by default -- R
        };

        class JuosFileSystem : public FsHandler
        {
        public:
            JuosFileSystem(kernel::StorageDeviceHandler* storage_handler);

            virtual void CreateFile(const char* filename);

            void MakeNewFs();

            virtual void DeleteFile(const char* filename);

            virtual ~JuosFileSystem();

        private:
            kernel::StorageDeviceHandler*   _storage_handler;
            Vector<InodeBase*>  *           _inodes; 
            FsMeta                          _fs_meta;         
        };

    NAMESPACE_END(jfs)

NAMESPACE_END(drivers)

#endif // DRIVER_JFS_H