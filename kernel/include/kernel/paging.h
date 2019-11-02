#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_

#include <stdint.h>

#include "kuseful.h"
#include "kdef.h"

#define PAGE_SIZE_FACTOR            11 // Order of magnitude dictating the page size (2 << PAGE_SIZE_FACTOR)
#define PAGE_SIZE                   (2 << PAGE_SIZE_FACTOR) // The size of a page and frame
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
        } __PACKED;


        /**
         * @brief A table containing pages
         * 
         * pages - the pages of the table
         */
        struct PageTable
        {
            Page pages[PAGE_TABLE_SIZE];
        };

        /**
         * @brief A directory of pages, containing page tables
         * 
         * tables - an array of pointers to tables
         * tables_physical - an array of pointers to the physical address of each
         * corresponding table
         * tables_physical_addr - address of _tables_physical_
         */
        struct PageDirectory
        {
            PageTable*  tables[PAGE_DIRECTORY_SIZE];
            uint32_t    tables_physical[PAGE_DIRECTORY_SIZE];
            uint32_t    tables_physical_addr;
        };

        typedef struct Page             page_t;
        typedef struct PageTable        page_table_t;
        typedef struct PageDirectory    page_directory_t;

        /**
         * @brief Initates paging in the hardware.
         * Implemented by arch
         * 
         */
        void initiate();

        /**
         * @brief Initates paging in the kernel, setting up all the 
         * nessecary things 
         * 
         */
        void start();

        /**
         * @brief Change the current page directory to _new_dir_
         * 
         * @param dir - new page directory
         */
        void switch_page_dir(page_directory_t* new_dir);


        /**
         * @brief Get the page object
         * 
         * @param address - address of the page to get 
         * @param page_dir - pointer to the page directory of the required page
         * @param create - if true, new page will be created if 
         * @return page_t* - if page is found or create is true, pointer to the
         * required page. 
         * If page is not found while _create_ is false, PAGE_NOT_FOUND 
         * is returned.
         * If page is not found while _create_ is true, and the function 
         * could not create a new page, PAGE_CREATION_FAILED is returned.
         */
        page_t* get_page(uint32_t address, page_directory_t* page_dir, bool create = false);


        /**
         * @brief Handles a page fault exception raised by the CPU.
         * 
         * @param reg - current state of registers.
         */
        void __page_fault_handler(void* reg);

        NAMESPACE_BEGIN(frame)

            struct _FrameInfo
            {
                uint32_t* frame;
                uint32_t idx;
                uint32_t offset;
            };

            struct _FrameInfo get_info(uint32_t frame_addr);

            void set(uint32_t frame_addr);

            void clear(uint32_t frame_addr);

            bool test(uint32_t frame_addr);

            uint32_t frame::find_first();

        NAMESPACE_END(frame)

    NAMESPACE_END(paging)

NAMESPACE_END(kernel)

#endif //KERNEL_PAGING_H_