#ifndef KERNEL_LOGGING_H_
#define KERNEL_LOGGING_H_

#include <stdio.h>

// Make the kernel log stuff by compiling with -DK_LOG
#ifdef K_LOG
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