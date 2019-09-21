#ifndef KERNEL_DEFINES_H
#define KERNEL_DEFINES_H

#define __NO_MANGELING          extern "C"
#define NAMESPACE_BEGIN(name)   namespace name {
#define NAMESPACE_END(name)     }

/*
    Compiler specific
*/
#ifdef __GNUC__
    #define __PACKED __attribute__((packed))
#endif

#endif