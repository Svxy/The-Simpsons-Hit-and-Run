#ifndef __SCRATCH_ARRAY_H__
#define __SCRATCH_ARRAY_H__

#include <render/culling/FixedArray.h>
#include <render/culling/srrRenderTypes.h>
//
// Designed for quick alloc/dealloc
//

template <class T> class SubArray
{
public:
   SubArray()
   {
      Invalidate();
   }

   SubArray( T* ipData, int iSize )
   {
      mpData = ipData;
      mSize  = iSize;
   }

   ~SubArray()
   {
      if( IsValid() )
      {
         rAssert(false);
         Invalidate();
      }
   }

   T& operator[]( int iIndex )
   {
      rAssert( IsValid() );
      rAssert( (iIndex < mSize) && (iIndex > -1) );
      return mpData[iIndex];
   }

   void SetAll( T& irDefaultValue )
   {
      rAssert( IsValid() );
      for(int i=0; i<mSize; i++ )
      {
         mpData[i] = irDefaultValue;
      }
   }

   void SplitOffEnd( SubArray<T>& orSplitArray, int iSplitSize )
   {
      rAssert( IsValid() );
      rAssert( iSplitSize <= mSize );

      orSplitArray.mSize   = iSplitSize;
      orSplitArray.mpData  = mpData + (mSize-iSplitSize);

      if( mSize == iSplitSize )
         Invalidate();
   }

   void AbsorbR( SubArray<T>& orAdjArray )
   {
      rAssert(IsValid());
      rAssert(CanAbsorbR( orAdjArray ));

      mSize += orAdjArray.mSize;
      orAdjArray.Invalidate();
   }

   void Init( int iSize, T* ipData )
   {
      rAssert(!IsValid());

      mSize    = iSize;
      mpData   = ipData;
   }
   

   bool CanAbsorbR( SubArray<T>& irAdjArray )
   {
      if( mpData+mSize == orSplitArray.mpData )
         return true;
      else
         return false;
   }
   
   bool IsValid()
   {
      return (mpData!=NULL)&&(mSize!=SRR_ERR_INDEX);
   }

   void Invalidate()
   {
      mSize   = SRR_ERR_INDEX;
      mpData  = NULL;
   }

   T* mpData;
   int mSize;
protected:

};

/*
template <class T> class ScratchArray : protected FixedArray<T>
{
public:
   ScratchArray(){}
   ~ScratchArray(){}

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void Init( int iSize )
   {
      Allocate( iSize );
      CleanFreeSlots();
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void SetAll( T& irDefaultValue )
   {
      for(int i=0; i<mSize; i++ )
      {
         mpData[i] = irDefaultValue;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void Acquire( SubArray<T>& orSubArray, int iSize )
   {
      int FSIndex = IsThereSpace(iSize); //get free frag slot index
      if( FSIndex != SRR_ERR_INDEX )
      {
         Alloc( orSubArray, iSize, FSIndex );
      }
      else
      {
         opSubArray = NULL;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void Release( SubArray<T>& opSubArray )
   {
      all of this work is to set up a set of scratchpad allocators to do bin counts
         to better divide the data based on good spreads
         it will run faster... yay.

   }

protected:
   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void Alloc( SubArray<T>& orSubArray, int iSize, int iHoleIndex )
   {
      mAllocCount++;

      mFreeHoles[iHoleIndex].SplitOffEnd( orSubArray, iSize );
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void Free( SubArray<T>& orSubArray )
   {
      int AdjLIndex;
      FindAdjLHole( AdjLIndex, orSubArray );

      if( AdjLIndex != SRR_ERR_INDEX )
      {
         mFreeHoles[AdjLIndex].AbsorbR(orSubArray);
      }
      else
      {
         AddFreeHole( orSubArray, InvalidFreeHole() );
      }
      
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void FindAdjLHole( int& i, SubArray<T>& orSubArray )
   {
      rAssert((orSubArray.mpData >= mpData) &&  (orSubArray.mpData < (mpData+mSize)));

      for( i=0; i<msMaxFreeHoles; i++ )
      {
         if( (mFreeHoles[i].IsValid()) && (mFreeHoles[i].CanAbsorbR(orSubArray)) )
            return;
      }
      i = SRR_ERR_INDEX;
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void AddFreeHole( SubArray<T>& orSubArray, int& i )
   {
      rAssert( i!=SRR_ERR_INDEX );

   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   void CleanFreeHoles()
   {
      for( int i=0; i<msMaxFreeHoles; i++ )
      {
         mFreeFrags[i].Invalidate();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   int InvalidFreeHole()
   {
      for( int i=0; i<msMaxFreeHoles; i++ )
      {
         if( mFreeHoles[i].IsInvalid() )
            return i;
      }
      return SRR_ERR_INDEX;
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   int IsThereSpace( int iSize )
   {
      int svfi = SRR_ERR_INDEX;//SmallestValidFragIndex;
      int numFreeHoles = 0;
      for( int i=0; i<msMaxFreeHoles; i++ )
      {
         if( mFreeHoles[i].IsValid() )
         {
            numFreeHoles++;
            if ( (mFreeHoles[i].mSize >= iSize) && (mFreeHoles[i].mSize <= mFreeHoles[svfi].mSize) )
               svfi = i;
         }
      }

      if( mAllocCount < msMaxFreeHoles-1 )
      {
         return svfi;
      }
      else
      {
         return SRR_ERR_INDEX;
      }
   }

   static const int msMaxFreeHoles = 10;

   int         mAllocCount;
   SubArray<T> mFreeHoles[msMaxFreeHoles];

};
*/
#endif