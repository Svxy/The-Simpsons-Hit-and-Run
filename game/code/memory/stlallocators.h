//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        stlallocators.h
//
// Description: Use these custom allocators to route STL memory management
//              through our memory system.
//
// History:     3/20/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef STLALLOCATORS_H
#define STLALLOCATORS_H


//========================================
// Nested Includes
//========================================
#include <stddef.h>



template <class T> class s2alloc
{ 
    public: 
        
        // type definitions
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef T           value_type;
    
        // rebind allocator type to U
        template<class U>
        struct rebind 
        {
            typedef s2alloc<U> other;
        };
       
        // return address of values
        pointer address( reference x ) const
        {
            return &x;
        }
        
        const_pointer address( const_reference x ) const
        {
            return &x;
        }
        
        // constructors and destructor - nothing to do cause the allocator
        // has no state
        s2alloc() {}
        s2alloc( const s2alloc& ) {}
#ifndef TOOLS
        template<class U> s2alloc( const s2alloc<U>&) {}
#endif
        ~s2alloc() {} 

        // return maximum number of elements that can be allocated
        size_type max_size() const 
        {
            size_type count = (size_type)(-1) / sizeof(T);
		    return (0 < count ? count : 1);
        }

        // allocate but don't initialize num elemetns of type T
        pointer allocate( size_type n, const void* hint = 0 )
        {
            return (pointer) ::operator new(( n*sizeof(T) ));
        }
        
        // initialize elements of allocated storage p with value val
        void construct( pointer p, const T& val )
        {
            // Placement new.  Don't have to override this new cause it
            // doesn't actually alloctate memory.
            //
            new((void*)p) T(val);
        }


        // destroy elemetns of initialized storage p
        void destroy( pointer p )
        {
            // Placement delete.  Must explicitly invoke destructor
            // but it doesn't actually free memory.
            //
            p->~T();
        }

        // deallocate storage p of deleted items
        void deallocate( pointer p, size_type n )
        {
            ::operator delete( (void*)p );
        }

}; // end s2alloc<T> 



#endif //STLALLOCATORS_H
