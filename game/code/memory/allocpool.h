//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     23/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef ALLOCPOOL_H
#define ALLOCPOOL_H

//========================================
// Nested Includes
//========================================

#include <memory/srrmemory.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

template <class T> class AllocPool
{
    public:
        AllocPool( GameMemoryAllocator heap, unsigned int iPoolSize );
        virtual ~AllocPool();

        T* AllocateFromPool();
        void ReturnToPool( unsigned int pItem );
        
    protected:
        // override initialize if you have a different constructor
        virtual void Initialize();
        void Finalize();

        GameMemoryAllocator GetHeap() { return( mHeap ); }

    private:

        //Prevent wasteful constructor creation.
        AllocPool( const AllocPool& pAllocPool );
        AllocPool& operator=( const AllocPool& pAllocPool );

        GameMemoryAllocator mHeap;

        unsigned int mPoolIndex;
        unsigned int miPoolSize;

        T* mPool;
        bool* mUsed;
};

template <class T>
AllocPool<T>::AllocPool( GameMemoryAllocator heap, unsigned int iPoolSize )
{
    mHeap = heap;
    miPoolSize = iPoolSize;
    mPoolIndex = 0;

    mPool = new(mHeap) T[ miPoolSize ];
    mUsed = new(mHeap) bool[ miPoolSize ];
    
    Initialize();
   
}

template <class T>
AllocPool<T>::~AllocPool()
{
    Finalize();
    
    delete[] ( mHeap, mPool );
    delete[] ( mHeap, mUsed );
}

template <class T>
T* AllocPool<T>::AllocateFromPool()
{
    // if you hit this assert then your pool is too small
    rAssert( mPoolIndex < miPoolSize );

    T* pItem = &mPool[ mPoolIndex ];
    mUsed[ mPoolIndex ] = true;

    while(( mPoolIndex < miPoolSize ) && ( mUsed[ mPoolIndex ] ))
    {
        mPoolIndex++;
    }

    return( pItem );
}

template <class T>
void AllocPool<T>::ReturnToPool( unsigned int pItem )
{
    for( unsigned int i = 0; i < miPoolSize; i++ )
    {
        if( (unsigned int)&mPool[ i ] == pItem )
        {
            mUsed[ i ] = false;
            if( i < mPoolIndex )
            {
                mPoolIndex = i;
                return;
            }
        }
    }

}

template <class T>
void AllocPool<T>::Initialize()
{
    for( unsigned int i = 0; i < miPoolSize; i++ )
    {
        mUsed[ i ] = false;
    }
}

template <class T>
void AllocPool<T>::Finalize()
{
/*    for( unsigned int i = 0; i < miPoolSize; i++ )
    {
        if( mPool[ i ] == NULL )
        {
            delete( mHeap, mPool[ i ] );
            mPool[ i ] = NULL;
        }
    }*/
}

#endif // ALLOCPOOL_H

