#ifndef DRIVER_EXT2_H_
#define DRIVER_EXT2_H_

#include <kernel/storage.h>
#include <kernel/kdef.h>
#include <stdint.h>

NAMESPACE_BEGIN(ext2)

    enum class FsState 
    {
        Clean,
        HasErrors
    } __PACKED;

    enum class ErrorHandleGuide
    {
        Ignore,
        RemountRO, // remount read-only
        Panic
    } __PACKED;

    enum class OsId
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
        uint32_t total_inodes            : 4; //  Total number of inodes in file system 
        uint32_t total_blocks            : 4; //  Total number of blocks in file system 
        uint32_t n_su_blocks             : 4; //  Number of blocks reserved for superuser (see offset 80) 
        uint32_t unallocated_blocks      : 4; //  Total number of unallocated blocks 
        uint32_t unallocated_inodes      : 4; //  Total number of unallocated inodes 
        uint32_t sb_block                : 4; //  Block number of the block containing the superblock 
        uint32_t block_size              : 4; //  log2 (block size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the block size) 
        uint32_t fragment_size           : 4; //  log2 (fragment size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the fragment size) 
        uint32_t group_block_size        : 4; //  Number of blocks in each block group 
        uint32_t group_fragment_size     : 4; //  Number of fragments in each block group 
        uint32_t group_inode_size        : 4; //  Number of inodes in each block group 
        uint32_t last_mount_time         : 4; //  Last mount time (in POSIX time)     
        uint32_t last_written_time       : 4; //  Last written time (in POSIX time) 
        uint32_t mount_since_last_ck     : 2; //  Number of times the volume has been mounted since its last consistency check (fsck) 
        uint32_t n_mounts_before_ck      : 2; //  Number of mounts allowed before a consistency check (fsck) must be done 
        uint32_t ext2_sign               : 2; //  Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume 
        FsState fs_state                 : 2; //  File system state (see below) 
        uint32_t err_handling_spec       : 2; //  What to do when an error is detected (see below) 
        uint32_t version_minor           : 2; //  Minor portion of version (combine with Major portion below to construct full version field) 
        uint32_t last_ck_time            : 4; //  POSIX time of last consistency check (fsck) 
        uint32_t forced_ck_interval      : 4; //  Interval (in POSIX time) between forced consistency checks (fsck) 
        OsId os_id                       : 4; //  Operating system ID from which the filesystem on this volume was created (see below)  
        uint32_t version_major           : 4; //  Major portion of version (combine with Minor portion above to construct full version field) 
        uint32_t uid_can_reserve         : 2; //  User ID that can use reserved blocks 
        uint32_t gid_can_reserve         : 2; //  Group ID that can use reserved blocks 
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
            SetUserId   = 0x800
        } __PACKED;

        struct PermissionStruct 
        {
            uint32_t OtherX      : 1;
            uint32_t OtherW      : 1;
            uint32_t OtherR      : 1;
            uint32_t GroupX      : 1;
            uint32_t GroupW      : 1;
            uint32_t GroupR      : 1;
            uint32_t UserX       : 1;
            uint32_t UserW       : 1;
            uint32_t UserR       : 1;
            uint32_t StickyBit   : 1;
            uint32_t SetGroupId  : 1;
            uint32_t SetUserId   : 1;
        } __PACKED;

        union Permissions
        {
            PermissionStruct    flags;
            uint32_t            value : 18;
        } __PACKED;

        Type        type;
        Permissions permissions;

    } __PACKED;

    struct Inode
    {
        InodeTypeAndPermission  type_permission;         // Type and Permissions (see below) 
        uint32_t                user_id             : 2; // User Id  
        uint32_t                size_lower          : 4; // Lower 32 bits of size in bytes
        uint32_t                last_access_time    : 4; // Last Access Time (in POSIX time)   
        uint32_t                creation_time       : 4; // Creation Time (in POSIX time) 
        uint32_t                last_mod_time       : 4; // Last Modification time (in POSIX time) 
        uint32_t                deletion_time       : 4; // Last Modification time (in POSIX time) 
    
        // TODO coninue
    };

NAMESPACE_END(ext2)

#endif //DRIVER_EXT2_H_