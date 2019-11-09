#ifndef KERNEL_LOGGING_H_
#define KERNEL_LOGGING_H_

// Make the kernel log stuff by compiling with -DK_LOG
#ifdef K_LOG
    #define LOG_A(msg, ...)     printf(msg, __VA_ARGS__);
    #define LOG(msg)            printf(msg);
#else       
    #define LOG_A(msg, ...)
    #define LOG(msg)            printf(msg)
#endif

#endif 