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
#include <kernel/kio.h>

// Make the kernel log stuff by compiling with -DK_LOG
#ifdef K_LOG_GENERAL
    #define LOG(msg)                SYNCED_PRINTF(msg);
    #define LOG_A(msg, ...)         SYNCED_PRINTF_ARGS(msg, __VA_ARGS__);
    #define LOG_SA(subject, msg, ...)                                       \
                                    SYNCED_PRINTF("$&\x4\x0" subject "$&\xF\x0");  \
                                    SYNCED_PRINTF_ARGS(msg, __VA_ARGS__);
    #define LOG_S(subject, msg)                                             \
                                    LOG_SA(subject, msg, "")
#else       
    #define LOG(msg)            
    #define LOG_A(msg, ...)
    #define LOG_S(heading, msg)
    #define LOG_SA(heading, msg, ...)
#endif

#endif 