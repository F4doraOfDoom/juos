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
