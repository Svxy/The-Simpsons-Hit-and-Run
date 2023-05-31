#include <render/culling/CoordSubList.h>
#include <render/culling/CullData.h>
#include <raddebug.hpp>
#include <render/culling/Vector3i.h>

#include <memory/srrmemory.h>

//
// TODO - Replace this with a real variable setting
//
#define DEV_NUM_BINS 1024

///////////////////////////////////////////////////////////////////////////////////////////
// External Functions
//
// Used in sorting process for centroids
// Subtracting 1 is compensation for integral truncation in conversion
///////////////////////////////////////////////////////////////////////////////////////////

//
// TODO - inline it!
// 
inline int CompareCellN( Cell* ipCell1, Cell* ipCell2, int iIndex )
{
   //
   // Is there anyway to preserve non-zero sign faster for wholly fractional differences?
   // If not, you can always accept truncation loss
   //
   float Diff = ((Cell*)ipCell1)->Centroid(iIndex) - ((Cell*)ipCell2)->Centroid(iIndex);

   if( Diff > 0.0f )
      return 1;

   if( Diff < 0.0f )
      return -1;
   
   return 0;

//Lossy version
//   return (int)( ((Cell*)ipCell1)->Centroid(iIndex) - ((Cell*)ipCell2)->Centroid(iIndex) -1);
//
}

int CompareCellXs( const void* ipCell1, const void* ipCell2)
{
   return CompareCellN( ((Cell*)ipCell1), ((Cell*)ipCell2), 0);
}

int CompareCellYs( const void* ipCell1, const void* ipCell2)
{
   return CompareCellN( ((Cell*)ipCell1), ((Cell*)ipCell2), 1);
}

int CompareCellZs( const void* ipCell1, const void* ipCell2)
{
   return CompareCellN( ((Cell*)ipCell1), ((Cell*)ipCell2), 2);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Data List
//
//      int     mSize;
//      int     mWeightedSize;
//      Cell*   mpCellList;
//      Bounds3f  mBounds;
///////////////////////////////////////////////////////////////////////////////////////////

CoordSubList::CoordSubList( Cell* ipCellList, int iSize, int iWeightedSize, Bounds3f& irBounds )
{
   mpCellList     = ipCellList;
   mSize          = iSize;
   mWeightedSize  = iWeightedSize;

   mSpans.SetToSpan( irBounds.mMin, irBounds.mMax );
}

CoordSubList::CoordSubList( Cell* ipCellList, int iSize, Vector3i iSpans )
{
   mpCellList     =  ipCellList;
   mSize          =  iSize;
   mSpans         =  iSpans;

   GenWeightedSize();

}

CoordSubList::CoordSubList( Cell* ipCellList, int iSize )
{
   mpCellList     = ipCellList;
   mSize          = iSize;

   GenerateMembers();
}


CoordSubList::CoordSubList()
{
   mpCellList   = NULL;
   mSize        = 0;
}

CoordSubList::~CoordSubList()
{
}



//
// Generate deterministic members
//
// WeightedSize
// Spans
//
void CoordSubList::GenerateMembers()
{
   GenWeightedSize();
   GenSpans();
}

void CoordSubList::GenWeightedSize()
{
   int i;
   mWeightedSize = 0;

   for( i=0; i<mSize; i++ )
   {
      mWeightedSize += mpCellList[i].RenderWeight();
   }
}

void CoordSubList::GenSpans()
{
   int i;
/*
   Bounds3f tempBounds;
   
   tempBounds.mMin.SetFP( mpCellList[0].Centroid() );
   tempBounds.mMax.SetFP( mpCellList[0].Centroid() );

   for( i=1; i<mSize; i++ )
   {
      tempBounds.Accumulate( mpCellList[i].Centroid() );
   }
   */

   Bounds3i tempBounds;

   tempBounds.mMin.Set( mpCellList[0].BlockIndex() );
   tempBounds.mMax.Set( mpCellList[0].BlockIndex() );

   for( i=1; i<mSize; i++ )
   {
      tempBounds.Accumulate( mpCellList[i].BlockIndex() );
   }

   mSpans.SetToSpan( tempBounds.mMin, tempBounds.mMax );
}

int CoordSubList::WeightedSize()
{
   return mWeightedSize;
}

int CoordSubList::Size()
{
   return mSize;
}

Cell& CoordSubList::Get( int i )
{
   rAssert( i>-1 && i<mSize );
   return mpCellList[i];
}


///////////////////////////////////////////////////////////////////////////////////////////////
// This data structure should be able to split itself if any of the three axes contain
// elements occupying separate bins (marked by their block index)
///////////////////////////////////////////////////////////////////////////////////////////////
bool CoordSubList::CanBeSplit()
{
   if( mSize > 1 )
   {
      for( int iAxis=0; iAxis<3; iAxis++ )
      {
         for( int i=1; i<mSize; i++ )
         {
            if( mpCellList[i].BlockIndex(iAxis) != mpCellList[0].BlockIndex(iAxis) )
               return true;
         }
      }
   }
   return false;
}
//
// SubDivide the List 
//
//    Divide List by a weighting value
//
void CoordSubList::SubDivideHeuristic
( 
   Vector3f&      irGranularity, 
   CoordSubList** opList1, 
   CoordSubList** opList2, 
   int&           iorAxis,
   float&         orPlanePosn
)
{
MEMTRACK_PUSH_GROUP( "CoortSubList" );

   rAssert( mpCellList );
   rAssert( *opList1 == NULL );
   rAssert( *opList1 == NULL );

 /*  if( iorAxis == -1 )
   {
      iorAxis = ChooseAxis();
   }
*/
   //
   // Accumulate the WeightCount as the list is sorted by median, 
   // where each point of Cell weight accounts for an item
   //
   //
   // Currently this algorithm is inefficient, as it re-sorts the data
   // Each iteration. But this is just a test for an offline tool.
   //
  /* switch( iorAxis )
   {
   case 0:
      qsort( mpCellList, mSize, sizeof(Cell), CompareCellXs );
      break;
   case 1:
      qsort( mpCellList, mSize, sizeof(Cell), CompareCellYs );
      break;
   case 2:
      qsort( mpCellList, mSize, sizeof(Cell), CompareCellZs );
      break;
   default:
      rAssert(false);
      break;
   }
*/
   //
   //  Set Plane Posn
   //
//   int List1Size   = GetWeightedMedian( orPlanePosn, iorAxis );
   int List1Size = FindSplitIndex( iorAxis, 0 );

   //Debug
   if( List1Size >= mSize || List1Size <= 0 )
   {
      bool fuckedUp = true;
      iorAxis = -1;
      List1Size = FindSplitIndex( iorAxis, 0 );
   }


   SeekMinPlane( orPlanePosn, List1Size, iorAxis );

   *opList1 = new(GMA_TEMP) CoordSubList( mpCellList,                List1Size);
   *opList2 = new(GMA_TEMP) CoordSubList( &(mpCellList[List1Size]),  mSize - List1Size);
MEMTRACK_POP_GROUP("CoortSubList");

}

//
// SubDivide the List 
//
void CoordSubList::SubDivideUniform( int iAxis, CoordSubList** opList1, CoordSubList** opList2 )
{
   rAssert( mpCellList );
   rAssert( false ); // not implemented
   if( iAxis == -1 )
   {
      iAxis = ChooseAxis();
   }
}


//
// TODO -rename these GetWeightedMedian's to reflect what the second one does; distinguish
//
int CoordSubList::GetWeightedMedian( int iAxis )
{
   int MedianWeightCount = mWeightedSize / 2;
   int WeightCount = 0;
   int i=-1;

//   for( i = 0; (i < mSize) && (WeightCount < MedianWeightCount); i++ )
//   {
//      WeightCount += mpCellList[i].RenderWeight();
//   }
   do
   {
      i++;
      WeightCount += mpCellList[i].RenderWeight();
   } while( (i<mSize-1) && (WeightCount<MedianWeightCount) );

   //
   // If we've surpassed the median, step back, 
   // undercut the median to handle the 2 element cases.
   //
   if( WeightCount > MedianWeightCount )
   {
      if(i-1>=0)
      {
         i--;
      }
   }

   return i;
}

int CoordSubList::GetWeightedMedian( float& orPlanePosn, int iAxis )
{
   int i = GetWeightedMedian( iAxis );
   return GetPlanarDivision( orPlanePosn, iAxis, i );
}

int CoordSubList::GetPlanarDivision( float& orPlanePosn, int iAxis, int iIndex )
{
   int DivisionIndex = mpCellList[iIndex].BlockIndex(iAxis); 
   orPlanePosn = mpCellList[iIndex].MaxPlane(iAxis);

   for( iIndex++; (iIndex<mSize) && (mpCellList[iIndex].BlockIndex(iAxis) <= DivisionIndex); iIndex++ ){}
   return iIndex;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Returns: The index in the SubList which marks the beginning of the second list
//
// Method:
//    Find the best axis/index to split the data along.
//    Best is defn'd as the one which can be split most closely to half (dichotomy heuristic)
//    Then do an in-place GT/LT median sort, returning the index of the start of the GT list
///////////////////////////////////////////////////////////////////////////////////////////////
int CoordSubList::FindSplitIndex( int& ioAxis, int iDeviationThreshold )
{
   Vector3i MedianDeviation, MedianBin;

   if( ioAxis == SRR_ERR_INDEX )
   {
      for( ioAxis=0; ioAxis<3; ioAxis++ )
      {
         if( mSpans[ioAxis] > 0 )
         {
            BinCount( MedianDeviation[ioAxis], MedianBin[ioAxis], ioAxis );

            if( MedianDeviation[ioAxis] <= iDeviationThreshold )
            {
               return BinSort( MedianBin[ioAxis], ioAxis );
            }
         }
         else
         {
            //Don't consider this axis
            MedianDeviation[ioAxis] = mWeightedSize;
            MedianBin[ioAxis]       = mSize+1;
         }
      }
   /*
      BinCount( MedianDeviation.mY, MedianBin.mY, 1 );
      if( MedianDeviation.mY < iDeviationThreshold )
      {
         return BinSort( MedianBin.mY, 1 );
      }

      BinCount( MedianDeviation.mZ, MedianBin.mZ, 2 );
      if( MedianDeviation.mZ < iDeviationThreshold )
      {
         return BinSort( MedianBin.mZ, 2 );
      }
   */
      ioAxis = MedianDeviation.MinIndex();
      return BinSort( MedianBin[ioAxis], ioAxis );
   }
   else
   {
      BinCount( MedianDeviation[ioAxis], MedianBin[ioAxis], ioAxis );
      return BinSort( MedianBin[ioAxis], ioAxis );
   }
}
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void CoordSubList::BinCount( int& oMedianDeviation, int& oMedianBin, int iAxis )
{
   int i, Count=0, nNonEmptyBins=0;
   SubArray<int> Bins;
   theCullCache().Acquire( Bins, DEV_NUM_BINS );

   Bins.SetAll(Count);

   //Get Total, Bin Count List
   for(  i=0; i<mSize; i++ )
   {
      Bins[ mpCellList[i].BlockIndex(iAxis) ]   += mpCellList[i].RenderWeight();
      Count                                     += mpCellList[i].RenderWeight();
   }

   //Find Median & Deviation
   Count = Count/2;

   for( i=0; (i<Bins.mSize)&&(Count>0); i++ )
   {
      Count -= Bins[i];
   }

   i--;

   //Find the best approximate median
   if( Count < Bins[i]/-2 )
   {
      Count += Bins[i]; 
      oMedianBin = i; 
   }
   else
   {
       Count = rmt::Abs( Count );
      oMedianBin = i+1; 
   }

   oMedianDeviation = Count;

   theCullCache().Release( Bins );
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
int CoordSubList::BinSort( int iMedianBin, int iAxis )
{
   int RightIndex = mSize-1;
   Cell temp;
   Cell* left  = &(mpCellList[0]);
   Cell* right = &(mpCellList[RightIndex]);

   while( left!=right )
   {
      if( left->BlockIndex(iAxis) < iMedianBin )
      {
         left++;
      }
      else
      {
         //left > MedianBin; 
         //left needs to be swapped with a right < Median
         if( right->BlockIndex(iAxis) < iMedianBin )
         {
            temp     = *right;
            *right   = *left;
            *left    = temp;

            left++;
            if( left!=right )
            {
               right--;
               RightIndex--;
            }
         }
         else
         {
            right--;
            RightIndex--;
/*            //In the case where they overlap, ensure right ends on GT
            if( left == right )
            {
               if( right->BlockIndex(iAxis) < iMedianBin )
               {
                  RightIndex++;
               }
            }
  */
         }
      }
   }
   //Debug
   if( right->BlockIndex(iAxis) < iMedianBin )
   {
      rAssert( (right+1)->BlockIndex(iAxis) >= iMedianBin );
      RightIndex++;
   }

   float minPlane;
   SeekMinPlane( minPlane, RightIndex, iAxis );

   return RightIndex;
}
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void CoordSubList::SeekMinPlane( float& oPlanePosn, int i, int iAxis )
{
   //Debug
   float LTMaxPlane;
   SeekMaxPlane( LTMaxPlane, 0, i, iAxis);
   //Debug
   
   oPlanePosn = mpCellList[i].MinPlane(iAxis);
   for( i++; i<mSize; i++ )
   {
      rAssert( mpCellList[i].MinPlane(iAxis) < mpCellList[i].MaxPlane(iAxis) );
      if( mpCellList[i].MinPlane(iAxis) < oPlanePosn )
      {
         oPlanePosn = mpCellList[i].MinPlane(iAxis);
      }
   }

   //Debug
   rAssert( LTMaxPlane <= oPlanePosn );
}

void CoordSubList::SeekMaxPlane( float& oPlanePosn, int iCur, int iEnd, int iAxis )
{
   oPlanePosn = mpCellList[iCur].MaxPlane(iAxis);
   for( iCur++; iCur<iEnd; iCur++ )
   {
      rAssert( mpCellList[iCur].MinPlane(iAxis) < mpCellList[iCur].MaxPlane(iAxis) );

      if( mpCellList[iCur].MaxPlane(iAxis) > oPlanePosn )
      {
         oPlanePosn = mpCellList[iCur].MaxPlane(iAxis);
      }
   }
}
/*
int CoordSubList::GetPlanarDivision( float& orPlanePosn, int iAxis, int iIndex )
{
   orPlanePosn = mpCellList[iIndex].MaxPlane(iAxis);

   for( iIndex++; (iIndex<mSize) && (mpCellList[iIndex].MaxPlane(iAxis) <= orPlanePosn); iIndex++ ){}
   return iIndex;
}
*/
/*
int CoordSubList::GetPlanarDivision( int iIndex, int iAxis, Vector3f irGranularity )
{
   float UpperBound = GetCellUpperBound( mpCellList[iIndex].Centroid(iAxis), irGranularity[iAxis] );

   for( ; (iIndex < mSize) && (mpCellList[iIndex].Centroid(iAxis) < UpperBound); iIndex++ ){}

   return iIndex;
}

float CoordSubList::GetCellUpperBound( float iCoord, float iGranularity )
{
   return (sTruncate(iCoord / iGranularity) + 1.0f) * iGranularity;
}
*/

/*int CoordSubList::GetWeightedMedian( int iAxis, int* opLowerWeightCount, tBox3D* opBounds )
{
   int MedianWeightCount = mWeightedSize / 2;
   int WeightCount = 0;
   int i;

   for( i = 0; (i < mSize) && (WeightCount < MedianWeightCount); i++ )
   {
      WeightCount += mpCellList[i].RenderWeight();
   }
   return i;
}*/




int CoordSubList::ChooseAxis()
{
 //  int ChosenAxis;

   //
   // The idea is to choose the axis that needs 
   // subdivision the most.
   //
   // Key Properties:
   //    Count:   same in each dimension
   //    Span:    calc'd on mBounds
   //    Spread:  may want to ensure that the evenly spread data gets divided
   //
   /*float Spans[3];
   Spans[0] = mBounds.mMax[0] - mBounds.mMin[0]; 
   Spans[1] = mBounds.mMax[1] - mBounds.mMin[1]; 
   Spans[2] = mBounds.mMax[2] - mBounds.mMin[2]; 
*/
   return mSpans.MaxIndex();
/*
   if( mSpans[0] >= mSpans[1] )
   {
      if( mSpans[0] >= mSpans[2] )
      {
         return 0;
      }
      else
      {
         return 2;
      }
   }
   else
   {
      if( mSpans[1] >= mSpans[2] )
      {
         return 1;
      }
      else
      {
         return 2;
      }
   }
   */
}

/*
char CoordSubList::BestSpreadAxis()
{
   int i, i_plane, weightCount, MedianWeight = mWeightedSize/2;
   Vector3i LTCounts, GTCounts;

   qsort( mpCellList, mSize, sizeof(Cell), CompareCellXs );
   for( i=0, weightCount=0; (i<mSize)&&(weightCount<MedianWeight); i++ )
   {
      weightCount += mpCellList.mpData[i].mRenderWeight();
   }
   //
   // If we've surpassed the median, step back, 
   // undercut the median to handle the 2 element cases.
   //
   if( weightCount > MedianWeight )
   {
      if(i-1>=0)
      {
         i--;
      }
   }
   i_plane = GetPlanarDivision( junkedPlane, 0, i );

   qsort( mpCellList, mSize, sizeof(Cell), CompareCellYs );
   qsort( mpCellList, mSize, sizeof(Cell), CompareCellZs );
   mpCellList.mpData[i].
}
*/