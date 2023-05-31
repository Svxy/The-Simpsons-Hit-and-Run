#ifndef __FIXED_ARRAY_H__
#define __FIXED_ARRAY_H__

#include <p3d/p3dtypes.hpp>
#include <raddebug.hpp>
#include <memory/srrmemory.h>

template <class T> class FixedArray
{
public:
   
   /////////////////////////////////////////
   // Constructors/Destructors
   /////////////////////////////////////////
   ~FixedArray()
   {
      Clear();
   }

   FixedArray() : mpData(NULL)
   {
   }

   FixedArray( int iSize ) : mpData(NULL)
   {
      Allocate( iSize );
   }


   /////////////////////////////////////////
   // Main Methods
   /////////////////////////////////////////
   T& operator[]( int iIndex )
   {
      rAssert( (iIndex < mSize) && (iIndex > -1) );
      return mpData[iIndex];
   }

   void Allocate( int iSize )
   {
      Clear();
      mSize = iSize;

#ifdef RAD_GAMECUBE
      //HeapMgr()->PushHeap( GMA_GC_VMM );
#endif

      mpData = new T[mSize];

#ifdef RAD_GAMECUBE
      //HeapMgr()->PopHeap( GMA_GC_VMM );
#endif

      rAssert(mSize>0);
      rAssert(mpData!=NULL);
   }

   void Clear()
   {
      if( mpData != NULL )
      {
         delete[] mpData;
         mpData = NULL;
      }
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
   T*  mpData;

protected:

private:
   /////////////////////////////////////////
   // Currently Disallowed Functions 
   //    -Not implemented
   //    -Not accessible
   //    -Use will error at Compile
   /////////////////////////////////////////
   FixedArray( const FixedArray& iSource );
   FixedArray& operator=( const FixedArray& iSource );

};


#endif