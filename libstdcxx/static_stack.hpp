#ifndef _LIBCXX_STATIC_STACK_H
#define _LIBCXX_STATIC_STACK_H

#include "libcxx_def.hpp"

NAMESPACE_BEGIN(std)

    template <typename Type, unsigned int Size>
    class static_stack
    {
    public:
        static_stack() : m_top(-1)
        {}

        bool push(Type item)
        {
            if (m_top >= (Size - 1)) { 
                return false; 
            } 
            else { 
                m_size++;
                m_arr[++m_top] = item; 
                return true; 
            } 
        }

        Type pop() 
        { 
            if (m_top < 0) { 
                return Type(); 
            } 
            else { 
                m_size--;
                Type x = m_arr[m_top--]; 
                return x; 
            } 
        } 

        Type peek() 
        { 
            if (m_top < 0) { 
                return Type(); 
            } 
            else { 
                Type x = m_arr[m_top]; 
                return x; 
            } 
        } 

        bool is_empty()
        {
            return (m_top < 0);
        }

        unsigned int max_size()
        {
            return Size;
        }

        int size()
        {
            return m_size;
        }

    private:
        Type m_arr[Size];
        int m_size;
        int m_top;
    };

NAMESPACE_END(std)

#endif //_LIBCXX_STATIC_STACK_H