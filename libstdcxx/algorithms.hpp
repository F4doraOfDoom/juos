#ifndef _CXX_ALGORITHMS_H
#define _CXX_ALGORITHMS_H

#include "libcxx_def.hpp"

NAMESPACE_BEGIN(std)

    template<class InputIt, class UnaryPredicate>
    constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate q)
    {
        for (; first != last; ++first) {
            if (q(*first)) {
                return first;
            }
        }
        return last;
    }

NAMESPACE_END(std)

#endif