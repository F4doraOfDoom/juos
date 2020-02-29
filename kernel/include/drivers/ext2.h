#ifndef DRIVER_EXT2_H_
#define DRIVER_EXT2_H_

#include <stdint.h>
#include <string.h>

#include <kernel/storage.h>
#include <kernel/vfs.h>

#include <kernel/kdef.h>
#include <kernel/kuseful.h>
#include <kernel/klog.h>


NAMESPACE_BEGIN(ext2)

    MACRO(SECTOR_SIZE_BYTES,    512);
    MACRO(EXT2_SIGN,            0xef53);

    MACRO(FILENAME_LENGTH,      512);
    MACRO(MAX_FILES,            1024);

    MACRO(SUPER_BLOCK_IDX,      1);

    enum class FsState : uint16_t
    {
        Clean,
        HasErrors
    } __PACKED;

    enum class ErrorHandleGuide : uint16_t
    {
        Ignore,
        RemountRO, // remount read-only
        Panic
    } __PACKED;

    enum class OsId : uint32_t
    {
        Linux   = 0,
        Gnu     = 1,
        Maxis   = 2, 
        FreeBSD = 3,
        Other   = 4  
    } __PACKED;

    enum class OptionalFeatureFlags
    {
        Preallocate     = 0x1,  // Preallocate some number of (contiguous?) blocks (see byte 205 in the superblock) to a directory when creating a new one (to reduce fragmentation?)  
        AFS             = 0x2,  // AFS server inodes exist 
        Journal         = 0x4,  // File system has a journal (Ext3) 
        Extended        = 0x8,  // Inodes have extended attributes 
        CanResize       = 0x10, // File system can resize itself for larger partitions  
        HashIndex       = 0x20  // Directories use hash index         
    };

    inline auto operator |(OptionalFeatureFlags lhs, OptionalFeatureFlags rhs)
    {
        return (static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    struct SuperBlock
    {
        uint32_t total_inodes            ; //  Total number of inodes in file system 
        uint32_t total_blocks            ; //  Total number of blocks in file system 
        uint32_t n_su_blocks             ; //  Number of blocks reserved for superuser (see offset 80) 
        uint32_t unallocated_blocks      ; //  Total number of unallocated blocks 
        uint32_t unallocated_inodes      ; //  Total number of unallocated inodes 
        uint32_t sb_block                ; //  Block number of the block containing the superblock 
        uint32_t block_size              ; //  log2 (block size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the block size) 
        uint32_t fragment_size           ; //  log2 (fragment size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the fragment size) 
        uint32_t group_block_size        ; //  Number of blocks in each block group 
        uint32_t group_fragment_size     ; //  Number of fragments in each block group 
        uint32_t group_inode_size        ; //  Number of inodes in each block group 
        uint32_t last_mount_time         ; //  Last mount time (in POSIX time)     
        uint32_t last_written_time       ; //  Last written time (in POSIX time) 
        uint16_t mount_since_last_ck     ; //  Number of times the volume has been mounted since its last consistency check (fsck) 
        uint16_t n_mounts_before_ck      ; //  Number of mounts allowed before a consistency check (fsck) must be done 
        uint16_t ext2_sign               ; //  Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume 
        FsState fs_state                 ; //  File system state (see below) 
        ErrorHandleGuide err_handling    ; //  What to do when an error is detected (see below) 
        uint16_t version_minor           ; //  Minor portion of version (combine with Major portion below to construct full version field) 
        uint32_t last_ck_time            ; //  POSIX time of last consistency check (fsck) 
        uint32_t forced_ck_interval      ; //  Interval (in POSIX time) between forced consistency checks (fsck) 
        OsId os_id                       ; //  Operating system ID from which the filesystem on this volume was created (see below)  
        uint32_t version_major           ; //  Major portion of version (combine with Minor portion above to construct full version field) 
        uint16_t uid_can_reserve         ; //  User ID that can use reserved blocks 
        uint16_t gid_can_reserve         ; //  Group ID that can use reserved blocks 
    } __PACKED;

    struct BlockGroupDescriptor
    {
        uint32_t block_bitmap_block      : 4; //  Block address of block usage bitmap 
        uint32_t inode_bitmap_block      : 4; //  Block address of inode usage bitmap 
        uint32_t inode_table_start_block : 4; //  Starting block address of inode table 
        uint32_t group_n_unalloc_blocks  : 2; //  Number of unallocated blocks in group    
        uint32_t group_n_unalloc_inodes  : 2; //  Number of unallocated inodes in group    
        uint32_t n_dirs_in_group         : 2; //  Number of directories in group 
    private:
        uint32_t _unused                 : 14; // unused          
    } __PACKED;

    struct InodeTypeAndPermission
    {
        enum class Type
        {
            FIFO                = 0x1000,
            CharacterDevice     = 0x2000,
            Directory           = 0x4000,
            BlockDevice         = 0x6000,
            File                = 0x8000,
            SymbolicLink        = 0xA000,
            Socket              = 0xC000
        } __PACKED;

        enum class PermissionFlags
        {
            OtherX      = 0x1,
            OtherW      = 0x2,
            OtherR      = 0x4,

            GroupX      = 0x8,
            GroupW      = 0x10,
            GroupR      = 0x20,

            UserX       = 0x40,
            UserW       = 0x80,
            UserR       = 0x100,

            StickyBit   = 0x200,
            SetGroupId  = 0x400,
            SetUserId   = 0x800,
        } __PACKED;


        // as defined in PermissionFlags
        uint8_t other_x     : 1;
        uint8_t other_w     : 1;
        uint8_t other_r     : 1;

        uint8_t group_x     : 1;
        uint8_t group_w     : 1;
        uint8_t group_r     : 1;
        
        uint8_t user_x      : 1;
        uint8_t user_w      : 1;
        uint8_t user_r      : 1;    

        uint8_t sticky_bit  : 1;
        uint8_t set_grp_id  : 1;
        uint8_t set_usr_id  : 1;

        // as defined in Type
        uint32_t type       : 4;
    } __PACKED;
 
    union InodeFlags
    {
        struct flags
        {
            uint8_t secure_deletion         : 1; // Secure deletion (not used) 
            uint8_t keep_copy_on_deletion   : 1; // Keep a copy of data when deleted (not used) 
            uint8_t file_compression        : 1; // File compression (not used) 
            uint8_t syncronouse_updates     : 1; // Synchronous updates—new data is written immediately to disk 
            uint8_t immutable_file          : 1; // Immutable file (content cannot be changed) 
            uint8_t append_only             : 1; // Append only 
            uint8_t not_in_dump             : 1; // File is not included in 'dump' command 
            uint8_t lat_no_update           : 1; // Last accessed time should not updated 
            uint8_t reserved                : 8; // reserved
            uint8_t hash_indexed_dir        : 1; // Hash indexed directory 
            uint8_t afs_dir                 : 1; // AFS Directory
            uint8_t journal_file_data       : 1; // Journal file data 
        };

        uint32_t value;
    } __PACKED;

    struct Inode
    {
        InodeTypeAndPermission  type_permission     ; // Type and Permissions (see below) 
        uint16_t                user_id             ; // User Id  
        uint16_t                size_lower          ; // Lower 32 bits of size in bytes
        uint32_t                last_access_time    ; // Last Access Time (in POSIX time)   
        uint32_t                creation_time       ; // Creation Time (in POSIX time) 
        uint32_t                last_mod_time       ; // Last Modification time (in POSIX time) 
        uint32_t                deletion_time       ; // Deletion time (in POSIX time) 
        uint16_t                group_id            ; // Group ID
        uint16_t                n_hard_links        ; //  Count of hard links (directory entries) to this inode. When this reaches 0, the data blocks are marked as unallocated. 
        uint32_t                n_disk_sectors      ; //  Count of disk sectors (not Ext2 blocks) in use by this inode, not counting the actual inode structure nor directory entries linking to the inode.         
        InodeFlags              flags               ;
        uint32_t                os_specific_1       ; // Operating System Specific value #1
        uint32_t                block_ptrs[12]      ; // Pointers to data blocks
        uint32_t                indirect_ptr_1      ; // Singly Indirect Block Pointer (Points to a block that is a list of block pointers to data) 
        uint32_t                indirect_ptr_2      ; // Doubly Indirect Block Pointer (Points to a block that is a list of block pointers to Singly Indirect Blocks) 
        uint32_t                indirect_ptr_3      ; // Triply Indirect Block Pointer (Points to a block that is a list of block pointers to Doubly Indirect Blocks) 
        uint32_t                generation_number   ; // Generation number (Primarily used for NFS) 
        uint32_t                extended_attr_blk   ; // In Ext2 version 0, this field is reserved. In version >= 1, Extended attribute block (File ACL). 
        uint32_t                file_size_upper     ; // In Ext2 version 0, this field is reserved. In version >= 1, Upper 32 bits of file size (if feature bit set) if it's a file, Directory ACL if it's a directory 
        uint32_t                fragment_block_addr ; // Block address of fragment 
        uint32_t                os_specific_2[3]    ; // Operating System Specific Value #2                 
    };

    struct BlockGroupTable
    {
        /**
         * @brief Construct a new Block Group objects
         * 
         * @param is_read - is this object constructed to read data from storage?
         * 
         */
        BlockGroupTable(bool is_read) : _read(is_read) {}

        // deprecated default constructor
        BlockGroupTable() {}

        BlockGroupDescriptor    group_descriptor;
        uint8_t*                block_bitmap;
        uint8_t*                inode_bitmap;
        //Inode*                  inode_table;
        uint8_t                 inode_table_block_size;

        ~BlockGroupTable()
        {
            if (_read)
            {
                if (block_bitmap) delete[] block_bitmap;
                if (inode_bitmap) delete[] inode_bitmap;
                //delete[] inode_table;
            }
        }
    private:
        // Needed to know in order to clean up resources
        bool _read;
    };

    struct FsDescriptor
    {
        uint32_t block_size;
        uint32_t super_block_idx;
    };

    class Fs : public kernel::FsHandler
    {
    public:
        
        // cannot copy FS
        Fs(const Fs&) = delete;
        Fs(Fs&&) = delete;
        Fs& operator=(const Fs&) = delete;
        Fs& operator=(Fs&&) = delete;

        /**
         * @brief Construct a new Ext 2 Fs object
         * 
         * @param storage_device - pointer to a class implementing StorageDeviceHandler,
         * that handles reading and writing from the OS's storage medium
         */
        Fs(kernel::StorageDeviceHandler* storage_device, const FsDescriptor& descriptor);

        virtual void create_file(const char* filename) override;

        virtual void delete_file(const char* filename) override {}

        // destructor to clean up resources
        ~Fs();

    private:

        struct _WriteBlockGroupRequest
        {
            const BlockGroupTable*  bg;
            uint32_t                block_idx;
            const Inode&            inode;
            uint32_t                inode_idx;
        };

        enum class _GetObjectOptions 
        {
            NoCreate,           // Dont create new block group
            CreateIfNotFound,   // Create object only if requested object is not found
            ForceCreate         // Create new objects always, even it already exists
        };

        void _read_storage(uint8_t* buffer, uint32_t block, uint32_t n);

        void _write_storage(const uint8_t* buffer, uint32_t block, uint32_t n);

        void _read_object_from_block(void* obj, uint32_t block, uint32_t obj_size);

        void _write_object_to_block(const void* obj, uint32_t block, uint32_t obj_size);

        void _zero_block(uint32_t block);

        uint32_t    _get_available_inode_idx();

        /**
         * @brief Parse SuperBlock metadata from block _sb_block_idx_s_
         * if _create_new_ is true, create a new SuperBlock using information from _descriptor_ (must not be null),
         * and write it to block _sb_block_idx
         * 
         * @param sb_block_idx 
         * @param create_new 
         * @return SuperBlock 
         */
        SuperBlock _get_super_block(uint32_t sb_block_idx, _GetObjectOptions opt, const FsDescriptor* descriptor = nullptr);

        /**
         * @brief Parse the block group metadata of block group _block_idx_
         * if _create_new_ is true, create a new block group, and the returned object's pointer
         * will be null;
         * 
         * @param block_idx 
         * @param create_new 
         */
        BlockGroupTable*    _get_block_group(uint32_t block_idx, _GetObjectOptions opt);

        //void _print_filesystem();

        void _write_block_group(const _WriteBlockGroupRequest&& request);

        kernel::StorageDeviceHandler*   _storage_device;
        FsDescriptor                    _info;

        struct FileInode
        {
            uint8_t      name[256] = { 0 };
            uint32_t     inode_idx;
        };
        FileInode _file_inodes_cache[1024];

    };

NAMESPACE_END(ext2)

#endif //DRIVER_EXT2_H_