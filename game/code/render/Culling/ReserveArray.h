#ifndef __RESERVE_ARRAY_H__
#define __RESERVE_ARRAY_H__

#ifndef TOOLS
#include <memory/srrmemory.h>
#endif

template <class T> class ReserveArray
{
public:
   
   /////////////////////////////////////////
   // Constructors/Destructors
   /////////////////////////////////////////
   ~ReserveArray()
   {
      Clear();
   }

   ReserveArray() : mUseSize(0), mpData(NULL)
   {
   }

   ReserveArray( int iSize ) : mUseSize(0), mpData(NULL)
   {
      Allocate( iSize );
   }


   /////////////////////////////////////////
   // Main Methods
   /////////////////////////////////////////
   void Init( int iIndex, T& irVal )
   {
      rAssert( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
      mpData[iIndex] = irVal;
   }

   void ClearUse()
   {
      rAssert(IsSetUp());
      mUseSize = 0;
   }

   void Use( int iIndex )
   {
      rAssert( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
   }

   void AddUse( int iCountSize )
   {
      rAssert( (iCountSize+mUseSize)<=mSize );
      mUseSize += iCountSize;
   }

   void Add( T& irVal )
   {
      rAssert(mUseSize<mSize);
      mpData[mUseSize] = irVal;
      mUseSize++;
   }

   T& operator[]( int iIndex )
   {
      rAssert( (iIndex < mUseSize) && (iIndex > -1));
      return mpData[iIndex];
   }

   void Reserve( int iCount )
   {
      // UseSize is used during the 
      // unallocated state to count the reservations
      rAssert( !IsSetUp() );
      mUseSize += iCount;
   }

   void Allocate()
   {
      rAssert( !IsSetUp() );
      if( mUseSize == 0 )
      {
         mSize = mUseSize;
         mpData = NULL;
      }
      else
      {
         mSize = mUseSize;

#ifdef RAD_GAMECUBE
         HeapMgr()->PushHeap( GMA_GC_VMM );
#endif

         mpData = new T[mSize];
         rAssert(mSize>0);
         rAssert(mpData!=NULL);

#ifdef RAD_GAMECUBE
         HeapMgr()->PopHeap( GMA_GC_VMM );
#endif

         mUseSize = 0;
      }
   }

   void Allocate( int iSize )
   {
      rAssert( mUseSize == 0 );
      //TODO: wha?
      if( !IsSetUp() )
         iSize += mUseSize;
      Clear();
      mSize = iSize;

#ifdef RAD_GAMECUBE
      HeapMgr()->PushHeap( GMA_GC_VMM );
#endif

      mpData = new T[mSize];
      rAssert(mSize>0);
      rAssert(mpData!=NULL);

#ifdef RAD_GAMECUBE
      HeapMgr()->PopHeap( GMA_GC_VMM );
#endif

   }

   void Clear()
   {
      if( mpData != NULL )
      {
         delete[] mpData;
      }
      mpData = NULL;
      mUseSize = 0;
   }

   bool IsSetUp()
   {
      if( mpData == NULL )
         return false;
      else
         return true;
   }

   /////////////////////////////////////////
   // Data
   /////////////////////////////////////////
   int mSize;
   int mUseSize;
   T*  mpData;

protected:

private:
   /////////////////////////////////////////
   // Currently Disallowed Functions 
   //    -Not implemented
   //    -Not accessible
   //    -Use will error at Compile
   /////////////////////////////////////////
   ReserveArray( const ReserveArray& iSource );
   ReserveArray& operator=( const ReserveArray& iSource );

};

#endif