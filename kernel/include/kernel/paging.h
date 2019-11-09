#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_

#include <stdint.h>
#include <string.h>
#include <kernel/klog.h>

#include "interrupts.h"
#include "kconstants.h"
#include "kheap.h"
#include "kuseful.h"
#include "kdef.h"

#define PAGE_SIZE_FACTOR            11 // Order of magnitude dictating the page size (2 << PAGE_SIZE_FACTOR)
#define PAGE_SIZE                   (2 << PAGE_SIZE_FACTOR) // The size of a page and frame
#define FRAME_SIZE                  PAGE_SIZE // frame and page must be of the same size
#define PAGE_TABLE_SIZE             1024
#define PAGE_DIRECTORY_SIZE         1024

//Errors
#define PAGE_NOT_FOUND              -1
#define FRAME_NOT_FOUND             -1
#define PAGE_CREATION_FAILED        -2

#define BIT_FIELD_TYPE              uint32_t
#define BIT_FIELD_SIZE              sizeof(BIT_FIELD_TYPE)

#define INDEX_FROM_BIT(a)           a / (8*4)
#define OFFSET_FROM_BIT(a)          a % (8*4)

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(paging)

        /**
         * @brief Structure representing a Page
         * 
         * is_present - whether or not the page exists
         * rw - if bit clear - read only; else, read-write
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
            PageTable*  tables[PAGE_TABLE_SIZE];
            uint32_t    table_addresses[PAGE_TABLE_SIZE];
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

            // Dont use this constructor
            FrameTable() {};

            frame_table_result_t find_first();
            void set_at_addr(uint32_t addr);

            Frame*      frames = nullptr;
            uint32_t    length = 0;
        };

        typedef Page            page_t;
        typedef PageTable       page_table_t;
        typedef PageDirectory   page_directory_t;
        typedef Frame           frame_t; 
        typedef FrameTable      frame_table_t;

        /**
         * @brief Initialize paging in the kernel
         * 
         */
        void initialize();

        void page_fault_handler(void*);

    NAMESPACE_END(paging)

NAMESPACE_END(kernel)


// implemented by arch
__NO_MANGELING void _load_page_directory(uint32_t*);
__NO_MANGELING void _enable_paging();

#endif //KERNEL_PAGING_H_