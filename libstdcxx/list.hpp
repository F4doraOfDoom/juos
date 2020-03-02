#ifndef _LIBCXX_LIST_H
#define _LIBCXX_LIST_H

#include "libcxx_def.hpp"
#include "allocator.hpp"
#include "compare.hpp"

NAMESPACE_BEGIN(std)

    template <typename T>
    struct double_linked_node
    {
        typedef T                           value_type;
        typedef const T&                    const_reference;
        typedef double_linked_node<T>*      node_pointer;

        double_linked_node(const_reference v) : val(v), next(nullptr), prev(nullptr)
        {}

        double_linked_node() = default;

        value_type     val;
        node_pointer   next;
        node_pointer   prev;
    };
    
    template <typename Item, template <typename T> class Allocator = std::allocator>
    class list
    {
    public:
        typedef Item                        value_type;
        typedef Item*                       pointer;
        typedef const Item*                 const_pointer;
        typedef Item&                       reference;
        typedef const Item&                 const_reference;    
        typedef double_linked_node<Item>    node;
        typedef double_linked_node<Item>*   node_pointer;
        //typedef bool(*lessthan)(const_reference, const_reference);

        struct iterator
        {
        public:
            typedef node                container;
            typedef container*          container_pointer;
            typedef const iterator&     const_iterator_reference;

            iterator(container_pointer p) : m_item(p)
            {}

            Item& operator*()
            {
                return m_item->val;
            }

            Item* operator->()
            {
                return &(m_item->val);
            }

            iterator operator++()
            {
                m_item = m_item->next;
                return *this;
            }

            bool operator==(const_iterator_reference other) const
            {
                return m_item == other.m_item;
            }

            bool operator!=(const_iterator_reference other) const
            {
                return m_item != other.m_item;
            }

        private:
            double_linked_node<Item>* m_item = nullptr;
        };


        list() : m_size(0), m_head(nullptr), m_tail(nullptr)
        {
        }

        void push_back(const_reference item)
        {
            _insert_internal(item, 0);
        }

        iterator begin() 
        {
            return iterator(m_head);
        }

        iterator end()
        {
            return iterator(nullptr);
        }

    private:

        /**
         * @brief Internal function to insert items into the list
         * 
         * @param item - item to insert
         * @param pos - position to insert at
         * @return _Node<Type>* - new node
         */
        node_pointer    _insert_internal(const_reference item, node_pointer pos)
        {
            node_pointer n = m_allocator.allocate_single();
            m_allocator.construct(n, item);

            m_size++;

            n->next = pos;

            if (pos)
            {
                n->prev = pos->prev;
                pos->prev = n;
            }
            else
            {
                n->prev = m_tail;
                m_tail = n;
            }

            if(n->prev)
            {
                n->prev->next = n;
            }
            else
            {
                m_head = n;
            }

            return n;
        }  

        Allocator<double_linked_node<Item>>     m_allocator;
        size_t                                  m_size;    
        node_pointer                            m_head;
        node_pointer                            m_tail;  

    };

NAMESPACE_END(std)

#endif // _LIBC_CPP_STRING_H