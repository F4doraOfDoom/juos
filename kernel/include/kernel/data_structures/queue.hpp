#pragma once

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(data_structures)

        template <class etype>
        class Queue
        {
            class qnode
            {
            public:

                etype element;

                qnode *next;

                qnode( etype e = 0 ): element( e ), next( NULL )
                { }
            };

            qnode *first;
            qnode *last;

        public:

            Queue( ): first( NULL ), last( NULL )
            { }

            inline bool isempty( ) const
            {
                return first == NULL;
            }   

            void enqueue( const etype x );
            etype dequeue( );
            void makeempty( );
        };

        template <class etype>
        void Queue <etype> :: enqueue( const etype x )
        {
            if ( isempty( ) )
            {
                first = new qnode( x );
                last = first;
            }

            else
            {
                qnode *p = new qnode( x );

                last->next = p;
                last = last->next;
            }
        }

        template <class etype>
        etype Queue<etype>::dequeue( )
        {
            etype x;
            qnode *p;
            if ( !isempty( ) )
            {
                x = first->element;

                p = first;
                first = first->next;

                delete p;

                return x;
            }

            return nullptr;
        }

        template <class etype>
        void Queue<etype>::makeempty( )
        {
        while ( !isempty( ) )
            dequeue( );
        }

    NAMESPACE_END(data_structures)

NAMESPACE_END(kernel)