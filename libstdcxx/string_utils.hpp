#ifndef _LIBCXX_STRING_UTILS_H
#define _LIBCXX_STRING_UTILS_H

#include "libcxx_def.hpp"

#include <kernel/data_structures/string.hpp>
#include <kernel/data_structures/vector.hpp>
using StringType    = kernel::data_structures::String;
using ListType      = kernel::data_structures::Vector<StringType>;

NAMESPACE_BEGIN(std)

    NAMESPACE_BEGIN(string)

        ListType split(const StringType& string, char delimiter)
        {
            ListType list;
            StringType cur_str("");

            for (auto ch : const_cast<StringType&>(string))
            {
                if (ch == delimiter)
                {
                    list.push_back(cur_str);
                    cur_str = StringType("");
                }
                else
                {
                    cur_str += ch;
                }
            }
            
            list.push_back(cur_str);

            return list;
        }

    NAMESPACE_END(string)

NAMESPACE_END(std)

#endif // _LIBCXX_STRING_UTILS_H