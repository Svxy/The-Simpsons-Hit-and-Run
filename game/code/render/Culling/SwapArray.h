#ifndef __SWAP_ARRAY_H__
#define __SWAP_ARRAY_H__

#include <p3d/p3dtypes.hpp>

#ifndef TOOLS
//#include "../../memory/srrmemory.h"
#endif

#define ASSSERT(s) rTuneAssert(s)

template <class T> class SwapArray
{
public:
   
   /////////////////////////////////////////
   // Constructors/Destructors
   /////////////////////////////////////////
   ~SwapArray()
   {
      Clear();
   }

   SwapArray() : mUseSize(0), mpData(NULL)
   {
   }

   SwapArray( int iSize ) : 
    mUseSize(0),
    mpData(NULL)
   {
      Allocate( iSize );
   }


   /////////////////////////////////////////
   // Main Methods
   /////////////////////////////////////////
   void Remove( int iIndex )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex<mUseSize) );
      mSwapT = mpData[iIndex]; 
      mpData[iIndex] = mpData[mUseSize-1];
      mpData[mUseSize-1] = mSwapT;
      mUseSize--;
   }

   void RemoveKeepOrder( int iIndex )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex<mUseSize) );
      mSwapT = mpData[iIndex]; 
      while(iIndex<mUseSize-1)
      {
          mpData[iIndex] = mpData[iIndex+1];
          iIndex++;
      }
      mpData[mUseSize-1] = mSwapT;
      mUseSize--;
   }

   void Swap( int iIndex1, int iIndex2 )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex1<mUseSize) && (iIndex2<mUseSize) );
      mSwapT = mpData[iIndex1]; 
      mpData[iIndex1] = mpData[iIndex2];
      mpData[iIndex2] = mSwapT;
   }

   void Init( int iIndex, T& irVal )
   {
      ASSSERT( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
      mpData[iIndex] = irVal;
   }

   void ClearUse()
   {
      ASSSERT(IsSetUp());
      mUseSize = 0;
   }

   void Use( int iIndex )
   {
      ASSSERT( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
   }

   void AddUse( int iCountSize )
   {
      ASSSERT( (iCountSize+mUseSize)<=mSize );
      mUseSize += iCountSize;
   }

   void Add( T& irVal )
   {
      ASSSERT(mUseSize<mSize);
      mpData[mUseSize] = irVal;
      mUseSize++;
   }

   void Add( const T& irVal )
   {
      ASSSERT(mUseSize<mSize);
      mpData[mUseSize] = irVal;
      mUseSize++;
   }

   T& operator[]( int iIndex )
   {
      ASSSERT( (iIndex < mUseSize) && (iIndex > -1));
      return mpData[iIndex];
   }
   
   const T& operator[]( int iIndex )const
   {
      ASSSERT( (iIndex < mUseSize) && (iIndex > -1));
      return mpData[iIndex];
   }

   void Reserve( int iCount )
   {
      // UseSize is used during the 
      // unallocated state to count the reservations
      ASSSERT( !IsSetUp() );
      mUseSize += iCount;
   }

   void Allocate()
   {
      ASSSERT( !IsSetUp() );
      if( mUseSize == 0 )
      {
         mSize = mUseSize;
         mpData = NULL;
      }
      else
      {
         mSize = mUseSize;
         mpData = new T[mSize];
         ASSSERT(mSize>0);
         ASSSERT(mpData!=NULL);
         mUseSize = 0;
      }
   }

   void Allocate( int iSize )
   {
      ASSSERT( mUseSize == 0 );
      //TODO: wha?
      if( !IsSetUp() )
         iSize += mUseSize;
      Clear();
      mSize = iSize;
      mpData = new T[mSize];
      ASSSERT(mSize>0);
      ASSSERT(mpData!=NULL);
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
   T   mSwapT;
   
protected:

private:
   /////////////////////////////////////////
   // Currently Disallowed Functions 
   //    -Not implemented
   //    -Not accessible
   //    -Use will error at Compile
   /////////////////////////////////////////
   SwapArray( const SwapArray& iSource );
   SwapArray& operator=( const SwapArray& iSource );

};

template <class T> class NodeSwapArray
{
public:
   
   /////////////////////////////////////////
   // Constructors/Destructors
   /////////////////////////////////////////
   ~NodeSwapArray()
   {
      Clear();
   }

   NodeSwapArray() : mUseSize(0), mpData(NULL)
   {
   }

   NodeSwapArray( int iSize ) : 
    mUseSize(0),
    mpData(NULL)
   {
      Allocate( iSize );
   }


   /////////////////////////////////////////
   // Main Methods
   /////////////////////////////////////////
   void Remove( int iIndex )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex<mUseSize) );
      mSwapT = mpData[iIndex]; 
      mpData[iIndex] = mpData[mUseSize-1];
      mpData[mUseSize-1] = mSwapT;
      mUseSize--;
   }

   void RemoveKeepOrder( int iIndex )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex<mUseSize) );
      mSwapT = mpData[iIndex]; 
      while(iIndex<mUseSize-1)
      {
          mpData[iIndex] = mpData[iIndex+1];
          iIndex++;
      }
      mpData[mUseSize-1] = mSwapT;
      mUseSize--;
   }

   void Swap( int iIndex1, int iIndex2 )
   {
      //A pointer safe swap-out
      ASSSERT( (iIndex1<mUseSize) && (iIndex2<mUseSize) );
      mSwapT = mpData[iIndex1]; 
      mpData[iIndex1] = mpData[iIndex2];
      mpData[iIndex2] = mSwapT;
   }

   void Init( int iIndex, T& irVal )
   {
      ASSSERT( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
      mpData[iIndex] = irVal;
   }

   void ClearUse()
   {
      ASSSERT(IsSetUp());
      mUseSize = 0;
   }

   void Use( int iIndex )
   {
      ASSSERT( (iIndex>=mUseSize)&&(iIndex<mSize) );
      mUseSize = iIndex+1;
   }

   void AddUse( int iCountSize )
   {
      ASSSERT( (iCountSize+mUseSize)<=mSize );
      mUseSize += iCountSize;
   }

   bool Add( T& irVal )
   {
      if(mUseSize>=mSize)
          return false;
      mpData[mUseSize] = irVal;
      mUseSize++;
      return true;
   }

   T& operator[]( int iIndex )
   {
      ASSSERT( (iIndex < mUseSize) && (iIndex > -1));
      return mpData[iIndex];
   }
   
   const T& operator[]( int iIndex )const
   {
      ASSSERT( (iIndex < mUseSize) && (iIndex > -1));
      return mpData[iIndex];
   }

   void Reserve( int iCount )
   {
      // UseSize is used during the 
      // unallocated state to count the reservations
      ASSSERT( !IsSetUp() );
      mUseSize += iCount;
   }

   void Allocate()
   {
      ASSSERT( !IsSetUp() );
      if( mUseSize == 0 )
      {
         mSize = mUseSize;
         mpData = NULL;
      }
      else
      {
         mSize = mUseSize;
         mpData = new T[mSize];
         ASSSERT(mSize>0);
         ASSSERT(mpData!=NULL);
         mUseSize = 0;
      }
   }

   void Allocate( int iSize )
   {
      ASSSERT( mUseSize == 0 );
      //TODO: wha?
      if( !IsSetUp() )
         iSize += mUseSize;
      Clear();
      mSize = iSize;
      mpData = new T[mSize];
      ASSSERT(mSize>0);
      ASSSERT(mpData!=NULL);
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
   T   mSwapT;
   
protected:

private:
   /////////////////////////////////////////
   // Currently Disallowed Functions 
   //    -Not implemented
   //    -Not accessible
   //    -Use will error at Compile
   /////////////////////////////////////////
   NodeSwapArray( const NodeSwapArray& iSource );
   NodeSwapArray& operator=( const NodeSwapArray& iSource );

};
#endif