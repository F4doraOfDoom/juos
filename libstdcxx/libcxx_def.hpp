#ifndef _LIBCXX_DEF_H
#define _LIBCXX_DEF_H

#ifdef __cplusplus
    #define NAMESPACE_BEGIN(name)   namespace name {
    #define NAMESPACE_END(name)     }
#else
    #define NAMESPACE_BEGIN(name)
    #define NAMESPACE_END(name)
#endif


#endif //_LIBCXX_DEF_H