//source file: ./kernel/include/


//source file: ./kernel/include/arch


//source file: ./kernel/include/arch/i386


//source file: ./kernel/include/arch/i386/interrupts.h
#ifndef KERNEL_ISR_I386
#define KERNEL_ISR_I386

#include <stdio.h>

#include <kernel/kdef.h>
#include <kernel/kstructs.h>
#include <kernel/kuseful.h>
#include <kernel/kcommon.h>
#include <kernel/interrupts.h>
#include <kernel/klog.h>

#include "idt.h"

#define IRQ_0 32
#define IRQ_1 33
#define IRQ_2 34
#define IRQ_3 35
#define IRQ_4 36
#define IRQ_5 37
#define IRQ_6 38
#define IRQ_7 39
#define IRQ_8 40
#define IRQ_9 41
#define IRQ_10 42
#define IRQ_11 43
#define IRQ_12 44
#define IRQ_13 45
#define IRQ_14 46
#define IRQ_15 47

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_ACK_MASTER()  outb(0x20,0x20);
#define PIC_ACK_SLAVE()   outb(0x20,0x20); outb(0xa0,0x20);

// End of Interrupt
#define PIC_RESET 0x20
#define PIC_MASTER 0x20
#define PIC_SLAVE 0xA0

#define PIC_IS_SLAVE(num)   IN_RANGE_C(num, IRQ_8, IRQ_15)
#define PIC_IS_MASTER(num)  IN_RANGE_C(num, IRQ_0, IRQ_7)

#define I386_INTERRUPTS 256

extern Interrupts::handler interrupt_handlers[I386_INTERRUPTS];

typedef void (*isr_t)(registers32_t);

NAMESPACE_BEGIN(Interrupts)

    /*
        A function that calls idt::edit_entry
    */
    void AddToIdt(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags);

NAMESPACE_END(Interrupts)

__NO_MANGELING void isr_handler(registers32_t);
__NO_MANGELING void irq_handler(registers32_t);

#endif //KERNEL_ISR_I386

//source file: ./kernel/include/arch/i386/idt.h
#ifndef KERNEL_DESCRIPTOR_TABLES_I386
#define KERNEL_DESCRIPTOR_TABLES_I386

#include <stdint.h>
#include <kernel/kdef.h>
#include <kernel/tty.h>
#include <kernel/interrupts.h>
#include <kernel/dtables.h>
#include <kernel/kcommon.h>

#include "pic.h"

void _pic_remap(uint8_t offset1, uint8_t offset2);
void _set_all_interrupts();

NAMESPACE_BEGIN(Idt)

     struct EntryStruct
    {
        uint16_t base_low; // lower 16 bits of base addr
        uint16_t sel; // segment selector
        uint8_t zero; // should always be zero
        uint8_t flags; // entry flags
        uint16_t base_high; // higher 16 bits of base addr
    } __PACKED;

    struct PtrStruct
    {
        uint16_t size;
        uint32_t base;
    } __PACKED;

    typedef EntryStruct entry_t;


    /*
        This function initializes the IDT table and writes it to the CPU
    */
     void _Init();

    /*
        This function configures an IDT entry by index
            int32_t entry - index of the IDT entry
            uint32_5 base - the base addr of the interrupt handler
            uint16_t sel - segment selector
            uint8_t flags - IDT flags
    */
    void EditEntry(int32_t entry, uint32_t base, uint16_t sel, uint8_t flags);

    template <uint32_t N>
     inline void _request_isr();

NAMESPACE_END(Idt)

/*
    Written in a different assembly file
    Writes a given IDT into the CPU
*/
__NO_MANGELING void idt_dump(uint32_t table);

/*
    These functions will be written in a basic_callers.S
*/
__NO_MANGELING void isr_0();
__NO_MANGELING void isr_1();
__NO_MANGELING void isr_2();
__NO_MANGELING void isr_3();
__NO_MANGELING void isr_4();
__NO_MANGELING void isr_5();
__NO_MANGELING void isr_6();
__NO_MANGELING void isr_7();
__NO_MANGELING void isr_8();
__NO_MANGELING void isr_9();
__NO_MANGELING void isr_10();
__NO_MANGELING void isr_11();
__NO_MANGELING void isr_12();
__NO_MANGELING void isr_13();
__NO_MANGELING void isr_14();
__NO_MANGELING void isr_15();
__NO_MANGELING void isr_16();
__NO_MANGELING void isr_17();
__NO_MANGELING void isr_18();
__NO_MANGELING void isr_19();
__NO_MANGELING void isr_20();
__NO_MANGELING void isr_21();
__NO_MANGELING void isr_22();
__NO_MANGELING void isr_23();
__NO_MANGELING void isr_24();
__NO_MANGELING void isr_25();
__NO_MANGELING void isr_26();
__NO_MANGELING void isr_27();
__NO_MANGELING void isr_28();
__NO_MANGELING void isr_29();
__NO_MANGELING void isr_30();
__NO_MANGELING void isr_31();

#endif //KERNEL_DESCRIPTOR_TABLES_I386

//source file: ./kernel/include/arch/i386/vgh.h
#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H
 
#include <stdint.h>
 
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t _VgaEntryColor(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t _VgaEntry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
#endif

//source file: ./kernel/include/arch/i386/pic.h
#ifndef KERNEL_PIC_I386
#define KERNEL_PIC_I386

#include <stdint.h>

#include <kernel/kdef.h>

#include "interrupts.h"

NAMESPACE_BEGIN(Pic)

    /*
        Initializes the PIC (Programmable Interrupt Controller)
        uint32_t base - at what interrupt index do you want the PIC to begin?
            (Example: A base of 32 will map the PIC interrupts from 32 - 47) 
    */
    void Initialize(uint32_t base);

    /*
        Enables devices in the PIC
        uint32_t master_dev - devices to be enabled in the master PIC
        uint32_t slave_dev - devices to be enabled in the slave PIC
    */
     inline void _EnableDevices(uint8_t master_dev, uint8_t slave_dev);

    /*
        Maps the PIC to certain offsets in the interrupt table
        uint8_t master_base - the first address that the master PIc will be mapped to in 
            the interrupt table
        uint8_5 slave_base - same but for the base
    */
     inline void _PicRemap(uint8_t master_base, uint8_t slave_base);  

     inline void _AddToInterruptTable();  

NAMESPACE_END(Pic)

// will be written in a seperate .S file
__NO_MANGELING void irq_0();
__NO_MANGELING void irq_1();
__NO_MANGELING void irq_2();
__NO_MANGELING void irq_3();
__NO_MANGELING void irq_4();
__NO_MANGELING void irq_5();
__NO_MANGELING void irq_6();
__NO_MANGELING void irq_7();
__NO_MANGELING void irq_8();
__NO_MANGELING void irq_9();
__NO_MANGELING void irq_10();
__NO_MANGELING void irq_11();
__NO_MANGELING void irq_12();
__NO_MANGELING void irq_13();
__NO_MANGELING void irq_14();
__NO_MANGELING void irq_15();

#endif //KERNEL_PIC_I386

//source file: ./kernel/include/arch/i386/gdt.h
#ifndef KERNEL_GDT_I386
#define KERNEL_GDT_I386

#include <stdint.h>

#include <kernel/dtables.h>
#include <kernel/kdef.h>

NAMESPACE_BEGIN(Gdt)

    enum class Segment_Access_Type : uint8_t
    {
        Kernel_Code  = 0x9A,
        Kernel_Data  = 0x92,
        User_Code    = 0xFA,
        User_Data    = 0XF2
    };

    struct EntryStruct
    {
        uint16_t    limit_low; //lower 16 bits of base
        uint16_t    base_low; //The base address of the segment 
        uint8_t     base_middle; //middle 8 bits of base
        uint8_t     access; // access flags specifiying protection level
        uint8_t     granularity;
        uint8_t     base_high;
    } __PACKED;

    struct PtrStruct
    {
        uint16_t    size; 
        uint32_t    base;
    } __PACKED;

    typedef EntryStruct entry_t;
    typedef Segment_Access_Type Gsat;

    /*
        This function initializes the GDT table and writes it to the CPU
    */
    static void _Init();

    /*
        This function configures the GDT
        Input:
            int32_t     entry   - The index of the GDT entry
            uint32_t    base    - The base address of the GDT entry
            uint32_t    limit   - The upper limit of the GDT entry
            Gsat        access  - Entry access flags
            uint8_t     gran    - granuary byte
    */
    static void EditEntry(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);               


NAMESPACE_END(Gdt)

/*
    Written in a different assembly file.
    Writes a given GDT into the CPU.
*/
__NO_MANGELING void gdt_dump(uint32_t table);


#endif //KERNEL_GDT_I386

//source file: ./kernel/include/arch/i386/dtables_structs.h
#ifndef KERNEL_DESCRIPTOR_TABLES_STRUCTS_I386
#define KERNEL_DESCRIPTOR_TABLES_STRUCTS_I386

#include <kernel/dtables.h>
#include <kernel/kdef.h>
#include <kernel/kstructs.h>
#include <stdint.h>



#endif //KERNEL_DESCRIPTOR_STRUCTS

//source file: ./kernel/include/arch/i386/pit.h
#ifndef KERNEL_PIT_I386
#define KERNEL_PIT_I386

#include <stdint.h>

#include <kernel/timer.h>
#include <kernel/kdef.h>
#include <kernel/kcommon.h>

NAMESPACE_BEGIN(Pit)

    #define PIT_DPORT_1         0x40
    #define PIT_DPORT_2         0x41
    #define PIT_DPORT_3         0x42
    #define PIT_COMMAND_PORT    0x43

    #define PIT_FREQ            1193180
    #define PIT_COMMAND_BYTE    0x36

    static void _Init(uint32_t clock_freq);

NAMESPACE_END(Pit)

#endif // KERNEL_PIT_I386

//source file: ./kernel/include/drivers


//source file: ./kernel/include/drivers/ext2.h
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

        virtual void CreateFile(const char* filename) override;

        virtual void DeleteFile(const char* filename) override {}

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
         * and Write it to block _sb_block_idx
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

//source file: ./kernel/include/drivers/ata.h
#ifndef DRIVER_ATA_H_
#define DRIVER_ATA_H_

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/timer.h>
#include <kernel/kuseful.h>
#include <kernel/storage.h>

#include <stdint.h>
#include <string.h>

NAMESPACE_BEGIN(ata)

    MACRO(PRIMARY_IO_BASE,       0x1F0); 
    MACRO(PRIMARY_COMMAND_BASE,  0x3F7);
    MACRO(PRIMARY_SELECT,        0xA0);

    MACRO(SECONDARY_IO_BASE,        0x170); 
    MACRO(SECONDARY_COMMAND_BASE,   0x376);
    MACRO(SECONDARY_SELECT,         0xB0);

    MACRO(SECTOR_SIZE_BYTES,         512);
    MACRO(SECTOR_SIZE_WORDS,        SECTOR_SIZE_BYTES/2);  

    MACRO(INTERRUPT_REGISTER,       0x3F6);

    // same as Controller with Master/Slave
    enum class Bus : bool {
        Primary = true,
        Secondary = false
    };

    // saves which controller was last selected
    extern Bus selected_controller;

    enum class IoRegister : uint32_t {
        // name     // offset from
        Data            = 0,
        Error           = 1,
        Features        = 1,
        SectorCount     = 2,

        // different names for same ports
        SectorNumber    = 3, LBAlow  = 3,
        CylinderLow     = 4, LBAmid  = 4, 
        CylinderHigh    = 5, LBAhigh = 5,
        Head            = 6, Drive   = 6,

        Status          = 7,
        Command         = 7
    };

    enum class StatusRegisterState : uint8_t {
        ERR  = 0,   	// Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
        IDX  = 1, 		// Index. Always set to zero.
        CORR = 2, 	 	// Corrected data. Always set to zero.
        DRQ  = 3, 		// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
        SRV  = 4, 		// Overlapped Mode Service Request.
        DF 	 = 5, 	    // Drive Fault Error (does not set ERR).
        RDY  = 6, 		// Bit is clear when drive is spun down, or after an error. Set otherwise.
        BSY  = 7 		// Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset. 
    };

    enum class CommandRegister : uint32_t {
        AlternateStatus = 0,
        DeviceControl = 0,
        DriveAddress = 1
    };

    enum class DeviceType : uint8_t {
        Master = 0,
        Slave = 1
    };

    enum class Command : uint8_t {
        Identify = 0xEC,
        ReadBuffer = 0xE4,
        WriteBuffer = 0xE8,
        ReadSectorsWithRetry = 0x20,
        WriteSectorsWithRetry = 0x30,
        ReadDMA28 = 0xC8,
        ReadDMA48 = 0x25,
        WriteDMA28 = 0xCA,
        WriteDMA48 = 0x35,
        CacheFlush = 0xE7,
        Standby = 0xE0,
        StandbyLBA28 = (Standby | 0x40), // bit 6 enables for LBA 28 bit
    };

    union StatusRegister
    {
        struct StatusFlags {
            uint8_t Err  : 1;   	// Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
            uint8_t Idx  : 1; 		// Index. Always set to zero.
            uint8_t Corr : 1; 	 	// Corrected data. Always set to zero.
            uint8_t Drq  : 1; 		// Set when the drive has PIO data to transfer, or is ready to accept PIO data.
            uint8_t Srv  : 1; 		// Overlapped Mode Service Request.
            uint8_t Df 	 : 1; 	    // Drive Fault Error (does not set ERR).
            uint8_t Rdy  : 1; 		// Bit is clear when drive is spun down, or after an error. Set otherwise.
            uint8_t Bsy  : 1;		// Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset. 
        } flags;

        uint8_t value;
    };

    struct PRDT {
        uint32_t buffer_phys;
        uint16_t transfer_size;
        uint16_t mark_end;
    } __PACKED;

    struct DeviceInfoResult
    {
        bool        found                   = false;
        bool        not_ata                 = false;
        bool        is_hard_disk            = false;
        bool        LBA48_supported         = false;
        uint16_t    supported_UDMA_modes    = 0;
        bool        conductor_cable_found   = false;
        union
        {
            struct __PACKED {
                uint16_t low;
                uint16_t high;
            }  sectors                     = { 0, 0 };
            uint32_t value;
        }   LBA28_sectors;
        
        union 
        {
            struct __PACKED {
                uint16_t low;
                uint16_t low_mid;
                uint16_t mid_high;
                uint16_t high;
            }  sectors                  = { 0, 0, 0, 0 };
            uint64_t value;
        } LBA48_sectors;

    };

    struct FindDeviceResult
    {
        DeviceInfoResult master;
        DeviceInfoResult slave;
    };
 
    class Device : public kernel::StorageDeviceHandler
    {
    private:
        struct Request {
            uint8_t* buffer;
            uint32_t lba;
            uint32_t sectors;
        };

    public:
        using ReadRequest = Request;
        using WriteRequest = Request;

        Device(DeviceType type, const DeviceInfoResult& info);

        Device(const Device&) = delete;
        Device(Device&) = delete;
        void operator=(Device&) = delete;
        void operator=(const Device&) = delete;

        /**
         * @brief DEPRECATED - use read_sectors instead
         * 
         * @param buffer 
         * @param loc 
         * @param count 
         * @return true 
         * @return false 
         */
        bool read_bytes(uint8_t* buffer, uint32_t loc, uint32_t count);

        /**
         * @brief DEPRECATED - user write_sectors instead
         * 
         * @param buffer 
         * @param loc 
         * @param count 
         * @return true 
         * @return false 
         */
        bool write_bytes(const uint8_t* buffer, uint32_t loc, uint32_t count);

        /**
         * @brief Read a _sectors_ count of sectors from LBA _lba_ to _buffer_
         * 
         * @param buffer - buffer to read into
         * @param lba - logical base address of sectors to read from
         * @param sectors - amount of sectors
         * @return true if read is successful (not really working though)
         * @return false if read is unsuccessful
         */
        virtual bool ReadSectors(uint8_t* buffer, uint32_t lba, uint32_t sectors) override;

         /**
         * @brief Write a _sectors_ count of sectors to LBA _lba_ from _buffer_
         * 
         * @param buffer - buffer to Write from
         * @param lba - logical base address of sectors to Write from
         * @param sectors - amount of sectors
         * @return true if Write is successful(not really working though)
         * @return false if Write is unsuccessful
         */
        virtual bool WriteSectors(const uint8_t* buffer, uint32_t lba, uint32_t sectors) override;

    private:
        //uint32_t    _int_buffer[512] = { 0 };
        
        const DeviceType  _device_type;
        const Bus         _bus_type;  
        const uint32_t    _lba_28_sectors;
        const uint64_t    _lba_48_sectors;
    };

    /**
     * @brief Initialize the ATA driver
     * 
     */
    [[nodiscard]] Device* create_device();

    /**
     * @brief Will poll the buses to see if there are any ata devices
     * 
     */
    FindDeviceResult find_devices();

NAMESPACE_END(ata)

#endif //DRIVER_ATA_H_

//source file: ./kernel/include/drivers/ps2.h
#ifndef KERNEL_DRIVERS_PS2_H
#define KERNEL_DRIVERS_PS2_H

#include <kernel/kdef.h>
#include <kernel/kcommon.h>
#include <kernel/keyboard.h>

#include <stdint.h>

NAMESPACE_BEGIN(drivers)

    NAMESPACE_BEGIN(ps2)

        constexpr inline uint32_t DATA_PORT = 0x60;
        
        NAMESPACE_BEGIN(keyboard)

            /**
             * @brief PS/2 Keyboard handler function
             * 
             * @param args 
             * @return kernel::keyboard::InputKeyType 
             */
            kernel::keyboard::InputKeyType KeyboardSource(void* args);

        NAMESPACE_END(keyboard)

    NAMESPACE_END(ps2)

NAMESPACE_END(drivers)

#endif // KERNEL_DRIVERS_PS2_H

//source file: ./kernel/include/kernel


//source file: ./kernel/include/kernel/interrupts.h
/**
 * @file interrupts.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and structs related to interrupts
 * in the kernel
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_INTERRUPTS_H_
#define KERNEL_INTERRUPTS_H_

#include <stdint.h>
#include "kstructs.h"

enum class InterruptList : uint32_t
{
    PageFault = 14
};

NAMESPACE_BEGIN(Interrupts)

    typedef void (*handler)(void*);

    /**
     * @brief Iniaitlize the interrupt system
     * 
     */
    void Initialize();
 
    /**
     * @brief Set the handler _handler_ for interrupt # _interrupt_num_
     * 
     * @param interrupt_num 
     * @param handler 
     */
    void set_handler(uint32_t interrupt_num, handler handler);

NAMESPACE_END(Interrupts)

#endif //KERNEL_INTERRUPTS_H_

//source file: ./kernel/include/kernel/kconstants.h
/**
 * @file kconstants.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing kernel constants, so it will be easy to modify
 * them later
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_CONSTANTS_H_
#define KERNEL_CONSTANTS_H_

// this value represents the frequency (in hrtz)
// of the kernel's clock
#define K_INTERNAL_CLOCK_TICK_RATE 20 

// 4kb of memory
#define K_PAGE_SIZE             0x1000 
// we assume 16mb of memory
#define K_PHYSICAL_MEM_SIZE     0x1000000

// the first gigabyte of the memory space is reserved for the kernel
#define K_MAPPED_REGION         2 << 20

#endif //KERNEL_CONSTANTS_H_


//source file: ./kernel/include/kernel/kstdcxx.hpp
/**
 * @file kstdcxx.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing things related to the C++ Standard Library
 * @version 0.1
 * @date 12-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef _KERNEL_STDCXX_H
#define _KERNEL_STDCXX_H

// template <class T>
// struct Less
// {
// 	bool operator()(const T& x, const T& y) const
// 	{
// 		return x < y;
// 	}

// 	typedef bool result_type;
// };
#endif //   _KERNEL_STDCXX_H

//source file: ./kernel/include/kernel/schedualer.h
#ifndef KERNEL_PROC_SCHEDUALER_H
#define KERNEL_PROC_SCHEDUALER_H

#include <kernel/processing.h>
#include <kernel/scheduler_interface.h>
#include <kernel/data_structures/queue.hpp>

using namespace kernel;
using data_structures::Queue;
using Processing::KernelProcess;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        class ProcessScheduler : public IScheduler<KernelProcess>
        {
        public:
            ProcessScheduler()  {}  
  
            /**
             * @brief Add a process to the queue
             * 
             * @param process 
             */
            virtual void AddItem(KernelProcess* process);

            virtual KernelProcess* GetNext();

            virtual void CalculateNext(RegistersStruct_x86_32* regs, void* args);

            virtual void SignalEnd(uint32_t pid);

            /**
             * @brief Make the schedualer stop
             * 
             * @param args 
             */
            void Stop(void* args);

        private:

            KernelProcess* _FindProcess(Queue<KernelProcess*>& queue);
            
            bool _IsCanceled(KernelProcess::ID proc_id)
            {
                uint32_t index = 0;
                for (auto canceled_id : _canceled_processes)
                {
                    if (proc_id == canceled_id)
                    {
                        return true;
                    }
                    index++;
                }
                return false;
            }

            /**
             * @brief This function executes a process
             * 
             * @param process 
             */
            void _ExecuteProcess(KernelProcess* process);

            Queue<KernelProcess*> _system_processes;
            Queue<KernelProcess*> _regular_processes;

            Vector<KernelProcess::ID> _canceled_processes; 

            bool _keep_running = true;
        };



        KernelProcess* GetCurrentProcess();

        void run_process_scheduler(RegistersStruct_x86_32* regs, void* args);
        
    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_H

//source file: ./kernel/include/kernel/kallocators.hpp
#ifndef KERNEL_NEW_H_
#define KERNEL_NEW_H_

#include "kdef.h"
#include "kuseful.h"
#include "klog.h"

#include <kernel/mem_operators.hpp>

NAMESPACE_BEGIN(kernel)

    /**
     * @brief Custome memory allocator, that uses __primitive_heap
     * as an allocation space. Has no ability to deconstruct an object,
     * since you cannot deallocate on __primitive heap 
     * More documentation inside of IAllocator (libstdcxx/allocator.hpp)
     * 
     * @tparam Type - Allocator will alocate this type 
     */
    template <typename Type>
    struct PrimitiveAllocator
    {
        static constexpr uint32_t     object_size = sizeof(Type);     

        typedef Type                value_type;
        typedef Type&               reference;
        typedef const Type&         const_reference;
        typedef Type*               pointer; 

        static pointer allocate(uint32_t n)
        {
            return (pointer)kernel::heap::Allocate(n * object_size);
        }

        static void construct(pointer p, const_reference v)
        {
            *p = Type(v);
        }

        static void destroy(pointer p)
        {
            p->~Type();
        }
    };

    /**
     * @brief Custome memory allocator, that uses __mapped_heap
     * as an allocation space. Has the ability to deallocate memory.
     * More documentation inside of IAllocator (libstdcxx/allocator.hpp)
     * 
     * @tparam Type - Allocator will alocate this type 
     */
    template <typename Type>
    struct AdvancedAllocator
    {
        static constexpr uint32_t     object_size = sizeof(Type);     

        typedef Type                value_type;
        typedef Type&               reference;
        typedef const Type&         const_reference;
        typedef Type*               pointer; 

        /**
         * @brief Allocate _n_ objects of size Type
         * 
         * @param n 
         * @return pointer 
         */
        static pointer allocate(uint32_t n)
        {
            return new Type[n];
        }

        /**
         * @brief Deallocate a pointer pointing to _n_ objects of type Type
         * 
         * @param p 
         * @param n 
         */
        static void deallocate(pointer p, size_t n)
        {
            delete p;
        }

        /**
         * @brief construct an object of type Type in pointer _p_ with args _v_
         * 
         * @param p 
         * @param v 
         */
        static void construct(pointer p, const_reference v)
        {
            *p = Type(v);
        }

        /**
         * @brief Destroy an object of type Type in pointer _p_
         * 
         * @param p 
         */
        static void destroy(pointer p)
        {
            p->~Type();
        }
    };

NAMESPACE_END(kernel)



#endif // KERNEL_NEW_H_

//source file: ./kernel/include/kernel/mem_operators.hpp
/**
 * @file mem_operators.hpp
 * @author Jonathan Uklisty
 * @brief This file defined operators new and delete for c++. 
 * The operators use kmm's malloc and free
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_MEM_OPS_H_
#define KERNEL_MEM_OPS_H_

#include <kernel/kmm.h>

void* operator new(size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object the size of %d\n", size);
#endif

    return kernel::MemoryManager::malloc(size);
}

void* operator new[](size_t size)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Allocating object[] the size of %d\n", size);
#endif

    return kernel::MemoryManager::malloc(size);
}

void operator delete(void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete[](void* ptr, long unsigned int)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

void operator delete[](void* ptr)
{
#ifdef K_LOG_MALLOC
    LOG_SA("NEW: ", "Deleting object[] the at %p\n", ptr);
#endif

    kernel::MemoryManager::free(ptr);
}

#endif // KERNEL_MEM_OPS_H_

//source file: ./kernel/include/kernel/vfs.h
#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

/** 
 * @brief All implementations of a file system must provide a class
 * that implements this interface, as it is used by the VFS.
 */
class FsHandler
{
public:
    virtual void CreateFile(const char* filename) = 0;

    virtual void DeleteFile(const char* filename) = 0;

    virtual ~FsHandler() = default;
};

NAMESPACE_END(kernel)

#endif // KERNEL_VFS_H_

//source file: ./kernel/include/kernel/data_structures


//source file: ./kernel/include/kernel/data_structures/string.hpp
#pragma once

#include <kernel/kdef.h>
#include <string.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(data_structures)

        template<typename T>
        class It 
        {
            private:
                T* data;
            public:
                It(T* data) {
                    this->data = data;
                }

                T &operator*() {
                    return *data;
                }

                It<T> &operator++() {
                    data++;
                    return *this;
                }

                bool operator==(const It<T> &a) {
                    return data == a.data;
                }

                bool operator!=(const It<T> &a) {
                    return data != a.data;
                }
        };

        class String 
        {
            private:
                char* data;
                size_t size;
            public:
                It<char> begin() {
                    It<char> it(data);
                    return it;
                }

                const char* c_str() const
                {
                    return data;
                }

                It<char> end()
                {
                    It<char> it(data + size);
                    return it;
                }

        String(size_t init_size) {
            this->data = new char[init_size];
            this->size = init_size;

            if (this->data != NULL) {
                for (size_t i = 0; i < init_size; i++) {
                    data[i] = '\0';
                }
            }
        }

        /**
        * Constructor with char*
        */
        String(const char* data) {
            size_t size = strlen(data);

            this->data = new char[size];
            this->size = size;

            if (this->data != NULL) {
                strncpy(this->data, data, size);
            }
        }

        /**
        * Copy Constructor
        */
        String(const String &string) {
            this->size = string.getLength();
            this->data = new char[this->size];

            if (this->data != NULL) {
                strncpy(this->data, string.data, this->size);
            }
        }

        String(String&& string)
        {
            this->data = string.data;
            this->size = string.size;
            string.data = nullptr; 
        }

        /**
        * Destructor
        */
        ~String() {
            if (this->data != NULL)
                delete[] this->data;
        }


        /** Methods **/


        void operator=(String&& string)
        {
            this->data = string.data;
            this->size = string.size;
            string.data = nullptr;
        }

        void operator=(const String& string)
        {
            this->size = string.getLength();
            this->data = new char[this->size];

            if (this->data != NULL) {
                strncpy(this->data, string.data, this->size);
            }
        }

        char* getText() {
            return data;
        }

        void setText(const char* text) {
            delete[] this->data;

            this->size = strlen(text);
            this->data = new char[this->size];

            strncpy(this->data, text, this->size);
        }

        size_t getLength() const {
            return strlen(this->data);
        }

        void setLength(size_t size) {
            size_t old_length = this->getLength();
            char* old_data = this->data;

            this->size = size;
            this->data = new char[size];

            // Copy the characters (or add \0 if there are no characters anymore).
            for (size_t i = 0; i < size; i++) {
                if (i < old_length) {
                    this->data[i] = old_data[i];
                } else {
                    this->data[i] = '\0';
                }
            }

            // Destroy the old data
            delete[] old_data;

            // Ensure that the last element is \0
            this->data[size] = '\0';
        }

        void add(const String &text) {
            size_t new_size = this->size + text.size;
            setLength(new_size);

            size_t length = getLength();
            for (size_t i = length; i < new_size; i++) {
                this->data[i] = text.data[i - length];
            }
        }

        char get(size_t pos) const {
            if (pos > getLength()) {
                return '\0';
            }
            return data[pos];
        }

        bool compare(const String &string) const {
            return strcmp(this->data, string.data) == 0;
        }

        char operator[](size_t pos) {
            return this->get(pos);
        }
    };

    NAMESPACE_END(data_structures)

NAMESPACE_END(kernel)

using kernel::data_structures::String;

/** Free Operator Methods **/

// String operator+(const String &string1, const String &string2) {
//     String string = String(string1);
//     string.add(string2);

//     return string;
// }

// bool operator==(const String &string1, const String &string2) {
//     return string1.compare(string2);
// }

//source file: ./kernel/include/kernel/data_structures/vector.hpp
#ifndef KERNEL_DATA_STRCTURES_VECTOR_H_
#define KERNEL_DATA_STRCTURES_VECTOR_H_

#include <kernel/kdef.h>

unsigned int log(unsigned int n);
// { 
//     return (n > 1) ? 1 + log(n / 2) : 0; 
// }

int ceil(float num);
//{
//     int inum = (int)num;
//     if (num == (float)inum) {
//         return inum;
//     }
//     return inum + 1;
// }

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(data_structures)

        template <class T>
        class  Vector {
        public:

            typedef T* Iterator;

            Vector();
            Vector(unsigned int size);
            Vector(unsigned int size, const T & initial);
            Vector(const Vector<T>& v);
            ~Vector();

            unsigned int capacity() const;
            unsigned int size() const;
            bool empty() const;
            Iterator begin();
            Iterator end();
            T& front();
            T& back();
            void push_back(const T& value);
            void pop_back();

            void reserve(unsigned int capacity);
            void resize(unsigned int size);

            T & operator[](unsigned int index);
            Vector<T> & operator = (const Vector<T> &);
            void clear();
        private:
            unsigned int _size;
            unsigned int _capacity;
            unsigned int Log;
            T* buffer;
        };

        template<class T>
        Vector<T>::Vector() {
            _capacity = 0;
            _size = 0;
            buffer = 0;
            Log = 0;
        }

        template<class T>
        Vector<T>::Vector(const Vector<T> & v) {
            _size = v._size;
            Log = v.Log;
            _capacity = v._capacity;
            buffer = new T[_size];
            for (unsigned int i = 0; i < _size; i++)
                buffer[i] = v.buffer[i];
        }

        template<class T>
        Vector<T>::Vector(unsigned int size) {
            _size = size;
            Log = ceil(log((double) size) / log(2.0));
            _capacity = 1 << Log;
            buffer = new T[_capacity];
        }

        template <class T>
        bool Vector<T>:: empty() const {
            return _size == 0;
        }

        template<class T>
        Vector<T>::Vector(unsigned int size, const T& initial) {
            _size = size;
            Log = ceil(log((double) size) / log(2.0));
            _capacity = 1 << Log;
            buffer = new T [_capacity];
            for (unsigned int i = 0; i < size; i++)
                buffer[i] = initial;
        }

        template<class T>
        Vector<T>& Vector<T>::operator = (const Vector<T> & v) {
            delete[] buffer;
            _size = v._size;
            Log = v.Log;
            _capacity = v._capacity;
            buffer = new T [_capacity];
            for (unsigned int i = 0; i < _size; i++)
                buffer[i] = v.buffer[i];
            return *this;
        }

        template<class T>
        typename Vector<T>::Iterator Vector<T>::begin() {
            return buffer;
        }

        template<class T>
        typename Vector<T>::Iterator Vector<T>::end() {
            return buffer + size();
        }

        template<class T>
        T& Vector<T>::front() {
            return buffer[0];
        }

        template<class T>
        T& Vector<T>::back() {
            return buffer[_size - 1];
        }

        template<class T>
        void Vector<T>::push_back(const T & v) {
            /*
                Incidentally, one common way of regrowing an array is to double the size as needed.
                This is so that if you are inserting n items at most only O(log n) regrowths are performed
                and at most O(n) space is wasted.
            */
            if (_size >= _capacity) {
                reserve(1 << Log);
                Log++;
            }
            buffer [_size++] = v;
        }

        template<class T>
        void Vector<T>::pop_back() {
            _size--;
        }

        template<class T>
        void Vector<T>::reserve(unsigned int capacity) {
            T * newBuffer = new T[capacity];

            for (unsigned int i = 0; i < _size; i++)
                newBuffer[i] = buffer[i];

            _capacity = capacity;
            delete[] buffer;
            buffer = newBuffer;
        }

        template<class T>
        unsigned int Vector<T>::size() const {
            return _size;
        }

        template<class T>
        void Vector<T>::resize(unsigned int size) {
            Log = ceil(log((double) size) / log(2.0));
            reserve(1 << Log);
            _size = size;
        }

        template<class T>
        T& Vector<T>::operator[](unsigned int index) {
            return buffer[index];
        }

        template<class T>
        unsigned int Vector<T>::capacity()const {
            return _capacity;
        }

        template<class T>
        Vector<T>::~Vector() {
            if (_size > 0)
            {
                delete[] buffer;
            }
        }

        template <class T>
        void Vector<T>::clear() {
            _capacity = 0;
            _size = 0;
            buffer = 0;
            Log = 0;
        }

    NAMESPACE_END(data_structures)

NAMESPACE_END(kernel)

#endif // KERNEL_DATA_STRCTURES_VECTOR_H_

//source file: ./kernel/include/kernel/data_structures/queue.hpp
#pragma once

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(data_structures)

        template <class etype>
        class Queue
        {
            class qnode
            {
            public:

                etype element;

                qnode *next;

                qnode( etype e = 0 ): element( e ), next( NULL )
                { }

            };

            qnode *first;
            qnode *last;

        public:

            Queue( ): first( NULL ), last( NULL )
            { }

            inline bool isempty( ) const
            {
                return first == NULL;
            }   

            void enqueue( const etype x );
            etype dequeue( );
            void makeempty( );

            etype top() const 
            {
                return first->element;
            }
        };

        template <class etype>
        void Queue <etype> :: enqueue( const etype x )
        {
            if ( isempty( ) )
            {
                first = new qnode( x );
                last = first;
            }

            else
            {
                qnode *p = new qnode( x );

                last->next = p;
                last = last->next;
            }
        }

        template <class etype>
        etype Queue<etype>::dequeue( )
        {
            etype x;
            qnode *p;
            if ( !isempty( ) )
            {
                x = first->element;

                p = first;
                first = first->next;

                delete p;

                return x;
            }

            return nullptr;
        }

        template <class etype>
        void Queue<etype>::makeempty( )
        {
        while ( !isempty( ) )
            dequeue( );
        }

    NAMESPACE_END(data_structures)

NAMESPACE_END(kernel)

//source file: ./kernel/include/kernel/klog.h
/**
 * @file klog.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file that contains logging functions
 * @version 0.1
 * @date 12-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_LOGGING_H_
#define KERNEL_LOGGING_H_

#include <stdio.h>

// Make the kernel log stuff by compiling with -DK_LOG
#ifdef K_LOG_GENERAL
    #define LOG(msg)                printf(msg);
    #define LOG_A(msg, ...)         printf(msg, __VA_ARGS__);
    #define LOG_SA(subject, msg, ...)                                       \
                                    printf("$&\x4\x0" subject "$&\xF\x0");  \
                                    printf(msg, __VA_ARGS__);
    #define LOG_S(subject, msg)                                             \
                                    LOG_SA(subject, msg, "")
#else       
    #define LOG(msg)            
    #define LOG_A(msg, ...)
    #define LOG_S(heading, msg)
    #define LOG_SA(heading, msg, ...)
#endif

#endif 

//source file: ./kernel/include/kernel/kuseful.h
/**
 * @file kuseful.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing useful, common operations
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef K_USEFUL_H_
#define K_USEFUL_H_

inline void* PTR_ADVANCE(void* ptr, uint32_t n) {
    return (void*)((uint8_t*)ptr + n);
}

inline void* PTR_ADD(void* a, void* b) {
    return (void*)((uintptr_t)a + (uintptr_t)b);
}

inline void* PTR_SUB(void* a, void* b) {
    return (void*)((uintptr_t)a - (uintptr_t)b);
}

inline uint32_t GET_BIT_OF(uint32_t value)
{
    return ((2 << (value - 1)) == 0);
}

#define BIT_IS_CLEAR(n, k) (((((uint64_t)n) & (1 << (((uint64_t)k) - 1)))) != 0)

#define BIT_IS_SET(n,k) ((NOT BIT_IS_CLEAR(n, k)))

inline uint64_t ALIGN_VAL(uint64_t value, uint64_t alignment) {
    return value + (-value & (alignment - 1));
}

#define IN_RANGE_X(n, min, max) ((n > min) && (n < max))
#define IN_RANGE_C(n, min, max) ((n >= min) && (n <= max))

// bitset
#define BIT_MASK(b)             (1 << ((b)))
#define BIT_SET(a, b, off)      ((a)[b] |= BIT_MASK(off))
#define BIT_CLEAR(a, b, off)    ((a)[b] &= ~BIT_MASK(off))
#define BIT_TEST(a, b, off)     ((a)[b] & BIT_MASK(off))

#define OFFSET_OF(struct, field) ((int)(&( ((struct*)(0))->field )))

void BREAKPOINT(void);

//#define BIT_NSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

#endif //K_USEFUL_H_

//source file: ./kernel/include/kernel/storage.h
#ifndef KERNEL_STORAGE_H_
#define KERNEL_STORAGE_H_

#include <stdint.h>

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

        /**
         * @brief All drivers handling storage (such as an ATA driver for the HDD),
         * must implement this interface, as it is used by the FS implementation to read/Write
         * sectors to the storage medium.
         * 
         */
        struct StorageDeviceHandler
        {
                /**
                 * @brief Read a _sectors_ count of sectors from LBA _lba_ to _buffer_
                 * 
                 * @param buffer - buffer to read into
                 * @param lba - logical base address of sectors to read from
                 * @param sectors - amount of sectors
                 * @return true if read is successful (not really working though)
                 * @return false if read is unsuccessful
                 */
                virtual bool ReadSectors(uint8_t* buffer, uint32_t lba, uint32_t sectors) = 0;

                /**
                 * @brief Write a _sectors_ count of sectors to LBA _lba_ from _buffer_
                 * 
                 * @param buffer - buffer to Write from
                 * @param lba - logical base address of sectors to Write from
                 * @param sectors - amount of sectors
                 * @return true if Write is successful(not really working though)
                 * @return false if Write is unsuccessful
                 */
                virtual bool WriteSectors(const uint8_t* buffer, uint32_t lba, uint32_t sectors) = 0;

                virtual ~StorageDeviceHandler() = default;
        };

NAMESPACE_END(kernel)

#endif // KERNEL_STORAGE_H_

//source file: ./kernel/include/kernel/icxxabi.h
/**
 * @brief This file handler the C++ ABI
 * 
 */



#ifndef _ICXXABI_H
	#define _ICXXABI_H
 
	#define ATEXIT_MAX_FUNCS	128
 
	#ifdef __cplusplus
	extern "C" {
	#endif
 
typedef unsigned uarch_t;
 
struct atexit_func_entry_t
{
	/*
	* Each member is at least 4 bytes large. Such that each entry is 12bytes.
	* 128 * 12 = 1.5KB exact.
	**/
	void (*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};
 
int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
void __cxa_finalize(void *f);
 
	#ifdef __cplusplus
	}
	#endif
 
#endif

//source file: ./kernel/include/kernel/kio.h
/**
 * @file kio.h
 * @author Jonathan Uklisty
 * @brief Header file for IO operations
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <kernel/kdef.h>
#include <kernel/lock.h>

#include <stdint.h>
#include <stdio.h>

static DECLARE_LOCK(__print_lock);

#define LOCKED_PRINTF(fmt) \
    LOCK(__print_lock); \
    printf(fmt); \
    UNLOCK(__print_lock);


#define LOCKED_PRINTF_ARGS(fmt, ...) \
    LOCK(__print_lock); \
    printf(fmt, __VA_ARGS__); \
    UNLOCK(__print_lock);

#define SYNCED_PRINTF(fmt) \
    asm volatile("cli;"); \
    printf(fmt); \
    asm volatile("sti;"); \


#define SYNCED_PRINTF_ARGS(fmt, ...) \
    asm volatile("cli;"); \
    printf(fmt, __VA_ARGS__); \
    asm volatile("sti;"); \

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(IO)

        /**
         * @brief Get the character from stdio
         * 
         * @return char 
         */
        char GetChar();

        /**
         * @brief Get a string from stdio into _buffer_
         * 
         * @param buffer 
         * @param max_len - max size of the buffer
         */
        void GetString(char* buffer, uint32_t max_len);

    NAMESPACE_END(IO)

NAMESPACE_END(kernel)

#endif //

//source file: ./kernel/include/kernel/scheduler_interface.h
#ifndef KERNEL_SCHEDUALER_INTERFACE_H
#define KERNEL_SCHEDUALER_INTERFACE_H

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(scheduler)

        template <class ItemType>
        class IScheduler
        {
        public: 
            /**
             * @brief This function should be called when context switching. 
             * Upon calling it, the scheduler should set it's next process. 
             * 
             * @param regs - registers when context switching
             * @param args - optional arguments
             */
            virtual void CalculateNext(RegistersStruct_x86_32* regs, void* args) = 0;

            /**
             * @brief Get the Next process
             * 
             * @return ItemType* 
             */
            virtual ItemType* GetNext() = 0;

            /**
             * @brief Add a process to the process list
             * 
             * @param item 
             */
            virtual void AddItem(ItemType* item) = 0;

            /**
             * @brief Tell the scheduler that process _pid_ has finished.
             * 
             * @param pid 
             */
            virtual void SignalEnd(uint32_t pid) = 0;
        };

    NAMESPACE_END(scheduler)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H

//source file: ./kernel/include/kernel/timer.h
/**
 * @file timer.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing all time related functions and structs
 * in the kernel 
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

#include <stdint.h>
#include <stdio.h>

#include <kernel/data_structures/vector.hpp>
using kernel::data_structures::Vector;

#include <kernel/schedualer.h>
#include "klog.h"
#include "interrupts.h"
#include "kdef.h"

// the hardware clock will call this interrupt on every tick
#define TIMER_PORT 32

NAMESPACE_BEGIN(Timer)


    /**
     * @brief this function initializes the clock, implemented by arch
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void Initialize(uint32_t clock_freq);

    /**
     * @brief this function calls the arch's implenetation of "Initialize",
     * and sets up the kernel internal clock (software instead of arch) 
     * 
     * @param clock_freq - the frequency of the clock's updates
     */
    void start(uint32_t clock_freq);

    /**
     * @brief Make the current thread sleep until n time slices have passed.
     * A time slice is defined by the clock frequency passed to timer::start 
     * 
     * TODO: Add multithread support
     * 
     * @param slices - n of time slices to sleep
     */
    void sleep(uint32_t slices);

    /**
     * @brief Get the current time
     * 
     * @return uint64_t 
     */
    uint64_t current_time();

    /**
     * @brief this function is called upon every tick. it updates the kernel's
     * internal tick count
     * 
     */
    void __tick_handler(void*);

    // first is registers, second is function args
    using CallableFunc = void (*)(RegistersStruct_x86_32*, void*);

    /**
     * @brief Add a function to be called every tick
     * 
     * @param func 
     */
    void add_callable_function(CallableFunc func, void* args);

    // internal tick counter 
    // defined in timer.cpp
    extern uint64_t __tick_counter;
    

NAMESPACE_END(Timer)

#endif //KERNEL_TIMER_H_

//source file: ./kernel/include/kernel/kheap.h
/**
 * @file kheap.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Internal implementation of the heap
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_HEAP_H_
#define KERNEL_HEAP_H_

#include "kdef.h"
#include "klog.h"

#include <allocator.hpp>
#include <stdint.h>

#define HEAP_ALIGNMENT 0xFFFFF000
#define PAGE_SIZE 0x1000

#define IS_ALIGNED(addr) ((addr & ~HEAP_ALIGNMENT) == 0)

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(heap)

        /**
         * @brief allocate _n_ bytes on the heap
         * 
         * @param n - number of bytes to allocate
         * @param alligned - whether or not the memory should be alligned to a 
         * page size - trye by default
         * @return uint32_t - pointer to allocated memory
         */
        uint32_t Allocate(size_t n, bool alligned = true);

        /**
         * @brief allocate _n_ bytes on the heap
         * 
         * @param n - number of bytes to allocate
         * @param alligned - whether or not the memory should be alligned to a 
         * @param phys - pointer to the physical address 
         * page size - trye by default
         * @return uint32_t - pointer to allocated memory
         */
        uint32_t Allocate_WPointer(size_t n, uint32_t* phys, bool alligned = true);

    NAMESPACE_END(heap)

NAMESPACE_END(kernel)

#endif //KERNEL_HEAP_H_



//source file: ./kernel/include/kernel/dtables.h
/**
 * @file dtables.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and operations related to 
 * descriptor tables in the kernel (i.e. GDT and IDT)
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_DESCRIPTOR_TABLES_H
#define KERNEL_DESCRIPTOR_TABLES_H

#include <kernel/kdef.h>
#include <kernel/klog.h>

NAMESPACE_BEGIN(Gdt)

    /*
        This function is used to initilalize the global descriptor table
    */
    void Initialize();

NAMESPACE_END(Gdt)


NAMESPACE_BEGIN(Idt)

    /*
        This fucntion is used to Initialize the interrupt descriptor table
    */
    void Initialize();

NAMESPACE_END(Idt)

#endif //KERNEL_DESCRIPTORS_H

//source file: ./kernel/include/kernel/kcommon.h
/**
 * @file kcommon.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing common functions for the kernel
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_COMMON_H_
#define KERNEL_COMMON_H_

#include "kdef.h"
#include <stdint.h>

/*
    Common port IO tasks
*/
__NO_MANGELING inline void outb(short port, char value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

__NO_MANGELING inline uint8_t inb(short port) {
   uint8_t ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

__NO_MANGELING inline uint16_t inw(short port) {
   uint16_t ret;
   asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}


__NO_MANGELING inline void rep_insw(unsigned short port, void *addr, size_t cnt)
{
   __asm volatile("rep; insw"
       : "+D" (addr), "+c" (cnt)
       : "d" (port)
       : "memory");
}


__NO_MANGELING inline void rep_outsw(unsigned short port, const void *addr, size_t cnt){
   __asm volatile("rep; outsw" : "+S" (addr), "+c" (cnt) : "d" (port));
}

__NO_MANGELING inline void insw(unsigned short port, void *addr)
{
   rep_insw(port, addr, 1);
}

__NO_MANGELING inline void outsw(unsigned short port, const void *addr){
   rep_outsw(port, addr, 1);
}
#endif // KERNEL_COMMON_H_

//source file: ./kernel/include/kernel/keyboard.h
#ifndef KERNEL_KEYBOARD_INTERFACE_H
#define KERNEL_KEYBOARD_INTERFACE_H

#include <kernel/interrupts.h>
#include <kernel/kdef.h>

#include <stdint.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(keyboard)

        struct InputKeyType
        {
            // std::queue is mad becuase it expects the type to be pointerish
            // we create a constructor that takes a pointer to fix that
            InputKeyType(void*) {}
            InputKeyType() {}

            uint32_t character;
            bool error = false;
        };

        using KeyboardCallback = InputKeyType (*)(void*);

        /**
         * @brief Initialize the keyboard
         * 
         * @param source 
         */
        void InitializeKeyboard(KeyboardCallback source);

        /**
         * @brief Keyboard interrupt handler
         * 
         */
        void KeyboardHandler(void*);

    NAMESPACE_END(keyboard)

NAMESPACE_END(kernel)

#endif // KERNEL_SCHEDUALER_INTERFACE_H

//source file: ./kernel/include/kernel/processing.h
/**
 * @file process.h
 * @author your name (you@domain.com)
 * @brief Header file for things regarding processing and processes
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_PROCESS_H_
#define KERNEL_PROCESS_H_

#include <stdint.h>
#include <kernel/kdef.h>
#include <kernel/kstructs.h>

#include <kernel/scheduler_interface.h>
#include <kernel/keyboard.h>
#include <kernel/paging.h>
#include <kernel/lock.h>
#include <kernel/kmm.h>

#include <kernel/data_structures/string.hpp>
#include <kernel/data_structures/vector.hpp>
#include <kernel/data_structures/queue.hpp>

#include <move.hpp>
#include <algorithms.hpp>


using kernel::data_structures::Vector;
using kernel::data_structures::Queue;

#define KERNEL_STACK_BEGIN      0xA0000000
#define KERNEL_STACK_SIZE       0x01000000

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(Processing)

        template <typename T>
        struct InterruptGuard
        {
            InterruptGuard() {
                DISABLE_HARDWARE_INTERRUPTS();
            }

            InterruptGuard(T&& func) {
                DISABLE_HARDWARE_INTERRUPTS();
                func();
            }

            ~InterruptGuard()
            {
                ENABLE_HARDWARE_INTERRUPTS();
            }
        };

        struct RegisteredProcess
        {
            RegisteredProcess() : name(""), func_ptr(nullptr) {}
            RegisteredProcess(const String& _name, const void* _ptr) : name(_name), func_ptr(_ptr) {}

            String          name;
            const void*     func_ptr; // pointer to some code

            void operator=(const RegisteredProcess& p)
            {
                this->name      = p.name;
                this->func_ptr  = p.func_ptr;
            }
        };

        struct Thread
        {
            const void* current_address;
        };

        struct Context
        {
            uint32_t eip, esp, ebp;
            paging::PageDirectory directory;    
        };
        

        struct KernelProcess
        {
            using ID        = uint32_t;
            // function to be called after the process ends
            // recieves the process itself, and arguments
            using ProcessResolver = void (*)(ID proc_id, void*); 

            enum class Priority
            {
                Low,
                Regular,
                High,
                Critical,
                System
            };

            // implemented in processing.cpp
            KernelProcess(const void* func_ptr, Priority priority);
            ~KernelProcess();

            /**
             * @brief Depracated 
             * 
             * @param context 
             */
            void ApplyContext(const Context* context);

        
            ID                              pid = (_pid_seq++);
            uint64_t                        start_time = 0;
            uint64_t                        slice_size;
            Priority                        priority;
            kernel::paging::PageDirectory*  directory = nullptr;
            // stack and heap pointers
            void*                           stack_begin = nullptr;
            void*                           heap_begin = nullptr;
            void*                           data_begin = nullptr;
            // counter for how many times the process has been task switched
            uint64_t                        times_ran = 0;
 
            #if 1 // implement some way to detect architecture
            RegistersStruct_x86_32          registers;
            bool                            registers_set = false;
            #endif 

            // needed to release pages later when process is over
            Vector<paging::_HeapMappingSettings>* self_mappings;

            // function that is run on the end of the process
            ProcessResolver on_end = nullptr; 

            // in contrast with other propreties, the input buffer will be part of 
            // of the processes's address space
            Queue<keyboard::InputKeyType>* input_buffer = nullptr; 
            DECLARE_LOCK(input_buffer_lock);

        private:
            bool        _is_finished; 
            static ID   _pid_seq;
        };

        struct CachedProcessInfo
        {
            KernelProcess::ID               pid;
            Queue<keyboard::InputKeyType>** input_buffer_ptr = nullptr;
            volatile int*                   input_buffer_lock_ptr = nullptr;
        };


        /**
         * @brief Add a process to the known processes list
         * 
         * @param name - name of the process
         * @param func - pointer to the process code
         */
        void RegisterProcess(const String& name, const void* func);

        /**
         * @brief Get the Current execution context
         * 
         * @param context 
         */
        inline void GetCurrentContext(Context* context)
        {
            asm volatile("mov %%esp, %0": "=r"(context->esp));
            asm volatile("mov %%ebp, %0": "=r"(context->ebp));
            memcpy(&context->directory, paging_current_directory, sizeof(paging::PageDirectory));
        }

        /**
         * @brief Get the Pid of the current running processs
         * 
         * @return KernelProcess::ID 
         */
        KernelProcess::ID GetPid();

        NAMESPACE_BEGIN(Start)

            /**
             * @brief This function starts executing code from _code_ptr_
             * 
             * @param code_ptr - pointer to code
             */
            void Code(const void* code_ptr);

            /**
             * @brief This function starts a process with name _name_
             * @param name 
             * @return observer pointer to the process struct
             */
            KernelProcess::ID Process(const String& name, KernelProcess::Priority priority);

        NAMESPACE_END(Start)
            
        NAMESPACE_BEGIN(End)

            /**
             * @brief Terminate a task with id _id_
             * 
             * @param id 
             */
            void Process(KernelProcess::ID id);

        NAMESPACE_END(End)
        

        using ProcessScheduler = scheduler::IScheduler<KernelProcess>*;
        using SchedulerCallback = void (*)(RegistersStruct_x86_32*, void*);
        using KernelStart = void (*)();
        /**
         * @brief Initializes the paging system
         * 
         * @param scheduler a pointer to an object that implements the IScheduler interface
         */
        void Initialize(KernelStart start, SchedulerCallback, ProcessScheduler scheduler);

        /**
         * @brief Get the Scheduler object
         * 
         * @return ProcessScheduler 
         */
        ProcessScheduler GetScheduler();

        /**
         * @brief Get the Input Buffer of the current process
         * 
         * @return Queue<keyboard::InputKeyType>* 
         */
        Queue<keyboard::InputKeyType>* GetInputBuffer();

        /**
         * @brief Locks the current processes's input buffer with a mutex
         * 
         */
        void LockInputBuffer();


        /**
         * @brief Unlocks the current processes's input buffer with a mutex
         * 
         */
        void UnlockInputBuffer();

        /**
         * @brief Initiates the process, from it's own address space perspective.
         * 
         */
        void SelfProcessInit();

    NAMESPACE_END(Processing)

NAMESPACE_END(kernel)

/**
 * @brief Set the current instruction pointer to _ptr_
 * 
 * @param ptr 
 * @param eip 
 */
__NO_MANGELING void _set_instruction_ptr(uint32_t* ptr);

#endif // KERNEL_PROCESS_H_

//source file: ./kernel/include/kernel/lock.h
#ifndef KERNEL_LOCK_H
#define KERNEL_LOCK_H

#include <kernel/kdef.h>

#define DECLARE_LOCK(name) volatile int name 
#define LOCK(name) \
	while (!__sync_bool_compare_and_swap(& name, 0, 1)); \
	__sync_synchronize();
#define UNLOCK(name) \
	__sync_synchronize(); \
	name = 0;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(Concurrency)

    // Mutex object
    class Lock
    {
    public:
        /**
         * @brief Aquire the mutex object
         * 
         */
        void Aquire()
        {
            asm volatile(
                ".intel_syntax noprefix;"
                ".aquire_lock:"
                "lock bts [%0], 1;"
                "jc .spin_wait;"
                ".spin_wait:"
                "test dword [%0], 1;"
                "jnz .spin_wait;"
                "jmp .aquire_lock;"
            
                : "=r"(_lock)
            );        
        }

        /**
         * @brief Release the mutex object
         * 
         */
        void Release()
        {
            asm volatile(
                "lock bts [%0], 0;"
                : "=r"(_lock)
            );
        }

    private:
        volatile bool _lock;
    };

    NAMESPACE_END(Concurrency)

NAMESPACE_END(kernel)

#endif // KERNEL_LOCK_H

//source file: ./kernel/include/kernel/kstructs.h
/**
 * @file kstructs.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file for general kernel structs, that don't have a specific place
 * to be at. If you're looking for a certain struct, you're likely to find it in a header
 * file thats more closely related to the subject of the struct.
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_STRUCTS_H_
#define KERNEL_STRUCTS_H_

#include <stdint.h>
#include "kdef.h"

struct RegistersStruct_x86_32
{
    uint32_t ds; // Data segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // all the registers at time of interrupt
    uint32_t int_no, err_code; // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss; // pushed by processor
} __PACKED;

typedef RegistersStruct_x86_32 registers32_t;

union Bits32 
{
    struct 
    {
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
        uint8_t bit8 : 1;
        uint8_t bit9 : 1;
        uint8_t bit10 : 1;
        uint8_t bit11 : 1;
        uint8_t bit12 : 1;
        uint8_t bit13 : 1;
        uint8_t bit14 : 1;
        uint8_t bit15 : 1;
        uint8_t bit16 : 1;
        uint8_t bit17 : 1;
        uint8_t bit18 : 1;
        uint8_t bit19 : 1;
        uint8_t bit20 : 1;
        uint8_t bit21 : 1;
        uint8_t bit22 : 1;
        uint8_t bit23 : 1;
        uint8_t bit24 : 1;
        uint8_t bit25 : 1;
        uint8_t bit26 : 1;
        uint8_t bit27 : 1;
        uint8_t bit28 : 1;
        uint8_t bit29 : 1;
        uint8_t bit30 : 1;
        uint8_t bit31 : 1;
        uint8_t bit32 : 1;
    } bits;

    uint32_t value;
};

#endif //KERNEL_STRUCTS_H

//source file: ./kernel/include/kernel/kmm.h
#ifndef KERNEL_MEMORY_MANAGER_H
#define KERNEL_MEMORY_MANAGER_H

/**
 * @file kmm.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Kernel memory managment - malloc and free
 * @version 0.1
 * @date 12-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#include <stdint.h>
#include <string.h>
#include <allocator.hpp>
#include <static_stack.hpp>

#include "klog.h"
#include "kdef.h"
#include "kheap.h"
#include "paging.h"
#include "kbitset.h"
#include "kuseful.h"


#define MM_LOG_ALLOCATIONS          2
#define MM_LOG_CONSOLIDATION        3
#define MM_LOG_DUMPS                4

#define K_HEAP_START         0xC0000000
#define K_HEAP_INITIAL_SIZE  0x1000000
#define K_HEAP_INDEX_SIZE    0x20000
#define K_HEAP_MIN_SIZE      0x70000

// Malloc requests below this size will be fast bins
#define FASTBIN_THRESHOLD       0x40
// Number of fast bins available 
#define FASTBIN_MAX_SIZE        0x64

#define IS_FASTBIN_BIT          1 << 0

//TODO: Implement better heap after I get this to work

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(MemoryManager)

        struct FastChunk
        {
            uint32_t* ptr_to_heap;
        } __PACKED;

        struct FastBin
        {
            FastChunk*  chunks;
            uint32_t    chunk_size;
            uint32_t    bin_size; 
            uint32_t    start_addr;
            uint32_t    end_addr;
        } __PACKED;

        struct BigChunk
        {
            BigChunk*       prev;
            BigChunk*       next;
            uint32_t        size;
            bool            used;
        } __PACKED;

        struct Heap
        {
            uint32_t*       fast_bins;
            BigChunk*       big_chunks;
            uint32_t        allocated_chunks;
            uint32_t        start_address; // The start of our allocated space.
            uint32_t        end_address;   // The end of our allocated space. May be expanded up to max_address.
            uint32_t        max_address;   // The maximum address the heap can be expanded to.
            bool            is_kernel;     // Should extra pages requested by us be mapped as supervisor-only?
            bool            rw;             // Should extra pages requested by us be mapped as read-only?
        } __PACKED;

        typedef FastChunk   fast_chunk_t;
        typedef FastBin     fast_bin_t;
        typedef BigChunk    big_chunk_t;
        typedef Heap        heap_t;

        /**
         * @brief Initialize the  memory manager to use the heap from _start_ to _end_,
         * with it's max size begin _size_
         * 
         * @param start 
         * @param end 
         * @param size 
         * @param is_kernel - is this memory manager the kernel's?
         * @param rw - is the heap both read and write?
         */
        void Initialize(uint32_t start, uint32_t end, uint32_t size, bool is_kernel, bool rw);

        /**
         * @brief Allocate a memory chunk of size _size_
         * 
         * @param size 
         * @return void* 
         */
        void* malloc(uint32_t size);

        /**
         * @brief Free a memory chunk allocated by malloc
         * 
         * @param p 
         */
        void free(void* p);

    NAMESPACE_END(MemoryManager)

NAMESPACE_END(kernel)

#endif // KERNEL_MEMORY_MANAGER_H

//source file: ./kernel/include/kernel/tty.h
/**
 * @file Tty.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing functions and structs related to output
 * to the Tty
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H
 
#include <stddef.h>
#include <kernel/kdef.h> 

NAMESPACE_BEGIN(Tty)

    /**
     * @brief this function initializes the Tty (output screen)
     * 
     */
    void Initialize(void);

    /**
     * @brief this function outputs a single char into the Tty
     * 
     * @param c - char to output
     */
    void Putchar(char c);

    /**
     * @brief this function writes _size_ bytes of _data_ to Tty
     * 
     * @param data - data to Write
     * @param size - number of bytes to Write from data
     */
    void Write(const char* data, size_t size);

    /**
     * @brief this function writes a null terminated string to Tty
     * 
     * @param data - null terminated string
     */
    void WriteString(const char* data);

    /**
     * @brief this function cleans the Tty, leaving blank chars 
     * 
     */
    void Clean(); 


    /**
     * @brief Set the printing color scheme
     * 
     * @param fg - fore ground color
     * @param bg - back ground color
     */
    void SetColor(unsigned int fg, unsigned int bg);

NAMESPACE_END(Tty)

#endif

//source file: ./kernel/include/kernel/kbitset.h
#ifndef KERNEL_BITSET_H
#define KERNEL_BITSET_H

#define CHAR_BIT sizeof(uint32_t)

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a) |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

// bitset operations
namespace bitset
{
    /**
     * @brief Set bit _idx_ in _val_
     * 
     * @param val 
     * @param idx 
     */
    inline void set(uint32_t* val, uint32_t idx) {
        *val |= (1 << (idx - 1));
    }

    /**
     * @brief check if bit _idx_ in _val_ is 1
     * 
     * @param val 
     * @param idx 
     * @return true 
     * @return false 
     */
    inline bool test(uint32_t* val, uint32_t idx) {
        return (*val & (1 << (idx - 1)));
    }
}

#endif // KERNEL_BITSET_H

//source file: ./kernel/include/kernel/paging.h
/**
 * @file paging.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing declerations and definitions
 * of the paging interface
 * @version 0.1
 * @date 12-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_

#include <stdint.h>
#include <string.h>
#include <kernel/klog.h>
#include <kernel/hardware.h>

#include "interrupts.h"
#include "kconstants.h"
#include "kheap.h"
#include "kuseful.h"
#include "kdef.h"

#include <kernel/data_structures/vector.hpp>
using kernel::data_structures::Vector;

#define PAGING_LOG_CREATION         1
#define PAGING_LOG_MAPPING          1

#define PAGE_SIZE_FACTOR            11 // Order of magnitude dictating the page size (2 << PAGE_SIZE_FACTOR)
//#define PAGE_SIZE                   (2 << PAGE_SIZE_FACTOR) // The size of a page and frame
#define FRAME_SIZE                  PAGE_SIZE // frame and page must be of the same size
#define PAGE_TABLE_SIZE             1024
#define PAGE_DIRECTORY_SIZE         1024
#define CHUNK_SIZE_ALIGN            8

//Errors
#define PAGE_NOT_FOUND              -1
#define FRAME_NOT_FOUND             -1
#define PAGE_CREATION_FAILED        -2

#define BIT_FIELD_TYPE              uint32_t
#define BIT_FIELD_SIZE              sizeof(BIT_FIELD_TYPE)

#define INDEX_FROM_BIT(a)           a / (8*4)
#define OFFSET_FROM_BIT(a)          a % (8*4)


#define SET_DIRECTORY(directory)    asm volatile("mov %0, %%cr3" :: "r"(directory->table_addresses))

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(paging)

        /**
         * @brief This struct is passed into paging::Initialize
         * in order to specify which regions to map for the heap
         * 
         * begin    - beginning address of heap
         * end      - end address of heap 
         */
        struct _HeapMappingSettings
        {
            uint32_t begin;
            uint32_t end;
        };

        /**
         * @brief Structure representing a Page
         * 
         * is_present - whether or not the page exists
         * rw - if bit clear - read only; else, read-Write
         * is_user - whether or not the Page is non kernel owned
         * was_accessed - set if the page was accessed since the last refresh
         * was_written - set if the page as written to sicne last refresh
         * reserved - bits reserved for the CPU
         * flags - 3 bits available for kernel use
         * frame_addr - the frame address 
         */
        struct Page
        {
            BIT_FIELD_TYPE is_present     : 1;
            BIT_FIELD_TYPE rw             : 1;
            BIT_FIELD_TYPE is_user        : 1;
            BIT_FIELD_TYPE was_accessed   : 1;
            BIT_FIELD_TYPE was_written    : 1;
            BIT_FIELD_TYPE reserved       : 4;
            BIT_FIELD_TYPE flags          : 3;
            BIT_FIELD_TYPE frame_addr     : 20;
        };

        struct PageTable
        {
            // void set_page_idx(
            //     uint32_t    idx,
            //     bool        is_present,
            //     bool        rw,
            //     bool        is_user,
            //     bool        was_accessed,
            //     bool        was_written,
            //     uint32_t    frame_addr
            // );

            Page enteries[PAGE_TABLE_SIZE];
        };

        struct PageDirectory
        {
            PageTable*  tables[PAGE_TABLE_SIZE] = { 0 };
            uint32_t    table_addresses[PAGE_TABLE_SIZE] = { 0 };

            // defined in paging.cpp
            ~PageDirectory();
        };

        struct Frame
        {
            bool is_taken = false;
        };

        /**
         * @brief Struct representing a table of frames
         * 
         * frames - pointer to an array of frames
         * length - length of the array
         */
        struct FrameTable
        {
            struct FrameTableResult
            {
                uint32_t idx    = 0;
                bool    error   = false;
            };

            typedef FrameTableResult frame_table_result_t;

            FrameTable(uint32_t length);

            /**
             * @brief Dont use this constructor
             * 
             */
            FrameTable() {};

            frame_table_result_t FindFirst();

            void SetAtIndex(uint32_t idx);
            void FreeAtIndex(uint32_t idx);

            void SetAtAddress(uint32_t addr);

            Frame*      frames = nullptr;
            uint32_t    length = 0;
        };

        typedef Page            page_t;
        typedef PageTable       page_table_t;
        typedef PageDirectory   page_directory_t;
        typedef Frame           frame_t; 
        typedef FrameTable      frame_table_t;

        using MemoryAlloctor = void* (*)(uint32_t, void*);

        /**
         * @brief Initialize paging in the kernel
         * 
         * _heap_mapping - if not null, map the heap according
         * to _heap_mapping. If null, do not map the heap.
         */
        void Initialize(_HeapMappingSettings* _heap_mapping);

        /**
         * @brief Map the region between _start_ and _end_ in the paging directory _dir_
         * using the memory allocator _allocator_
         * 
         * @param start 
         * @param end 
         * @param allocator 
         * @param dir 
         * @return uint32_t 
         */
        uint32_t map_region(uint32_t start, uint32_t end, MemoryAlloctor allocator,  page_directory_t* dir = nullptr);

        /**
         * @brief Set the current_directory object
         * 
         * @param directory 
         */
        void SetDirectory(PageDirectory* directory);

        // creates a new page directory with mappings _mappings_
        // meant to be used only once the memory manager is initialized
        PageDirectory* create_directory(Vector<_HeapMappingSettings>& mappings);

        /**
         * @brief Get the Kernel Directory object
         * 
         * @return PageDirectory* 
         */
        PageDirectory* GetKernelDirectory();

        /**
         * @brief Function to handle page faults
         * 
         */
        void page_fault_handler(void*);

        auto StandartAllocator = [](uint32_t size, void* args) {
            return (void*)kernel::heap::Allocate_WPointer(size, (uint32_t*)args); 
        };


    NAMESPACE_END(paging)

NAMESPACE_END(kernel)

// the current paging directory
extern kernel::paging::PageDirectory* paging_current_directory;

// the kernel's paging directory
extern kernel::paging::PageDirectory* paging_kernel_directory;

// implemented by arch
__NO_MANGELING void _load_page_directory(uint32_t*);
__NO_MANGELING void _enable_paging();
//__NO_MANGELING void _disable_paging();

#endif //KERNEL_PAGING_H_

//source file: ./kernel/include/kernel/kdef.h
/**
 * @file kdef.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing kernel definitions
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef KERNEL_DEFINES_H
#define KERNEL_DEFINES_H

#include <stddef.h>
#include <stdio.h>

#define NOT     !
#define OR      ||
#define AND     &&  

#define MACRO(name, val)            constexpr uint32_t name = val
#define MACRO_T(type, name, val)    consexptr type name = val

#define __NO_MANGELING          extern "C"



#define GO_PANIC(message, ...)                  \
        printf("KERNEL PANIC!\n"); \
        printf(message, __VA_ARGS__);                       \
        printf("File: " __FILE__ "\n");                       \
        printf("Line: %d\n", __LINE__);                       \
        while(1) { asm volatile (""); }         \

#define ASSERT(condition)   \
    if (!(condition)) {GO_PANIC("Failed Assertion: " #condition "\n", "")}

#define NAMESPACE_BEGIN(name)   namespace name {
#define NAMESPACE_END(name)     }

#define CHECK_LOG_LEVEL(flag, level) (flag >= level)

// Set up arch flags
#ifdef __i386__
#define ARCH _i386_
#endif 

/*
    Compiler specific
*/
#ifdef __GNUC__ 
    #pragma GCC diagnostic ignored "-Wattributes"
    #define __PACKED        __attribute__((packed))
    #define __IS_USED       __attribute__((used))
    #define __ALIGNED(x)    __attribute__((aligned(x)))
    #define __MAYBE_UNUSED  __attribute__((unused))
    #define __IGNORE(warning)   GCC diagnostic ignored "warning"
#else
    // add flags from different compilers
#endif


#endif

//source file: ./kernel/include/kernel/hardware.h
#ifndef KERNEL_HARDWARE_H
#define KERNEL_HARDWARE_H

#include <kernel/kdef.h>

// implemented in arch
#define DISABLE_HARDWARE_INTERRUPTS() \
    asm volatile("cli;")

// implemented in arch
#define ENABLE_HARDWARE_INTERRUPTS() \
    asm volatile("sti;")

#endif // KERNEL_HARDWARE_H

