#ifndef _CXX_MOVE_H
#define _CXX_MOVE_H

#include "libcxx_def.hpp"

NAMESPACE_BEGIN(std)

    template <typename T>
    T&& move(T& obj)
    {
        return obj;
    }

NAMESPACE_END(std)

#endif //_CXX_MOVE_H