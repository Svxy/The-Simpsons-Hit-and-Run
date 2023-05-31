#include <render/culling/CellBlock.h>
#include <render/culling/VectorLib.h>
#include <render/culling/FloatFuncs.h>


CellBlock::CellBlock(){}
CellBlock::~CellBlock(){}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
/*
void CellBlock::Init
(
   int         iSize, 
   BlockCoord& irBlockCoord, 
   Vector3f&   irGranularities 
) 
{
   mBlockCoord    = irBlockCoord;
   mGranularities = irGranularities;

   Allocate( iSize );
}
*/

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::Init
( 
   FixedArray<Vector3f>&   irPoints, 
   Vector3f&               irGranularities 
)
{
   ///////////////////////////////////////////////////////////// 
   // TODO - 
   //
   // 1) Resolve: Find out whether any platforms round negative 
   // numbers toward Zero (ie, the sensible, magnitude driven way)
   // as ARM says it's platform dependant
   /////////////////////////////////////////////////////////////

   theVectorLib().FindBounds( mBounds, irPoints );

   mGranularities = irGranularities;
   mBlockCoord.Init( mBounds, mGranularities );

   Allocate( mBlockCoord.mExtents[0] * mBlockCoord.mExtents[1] * mBlockCoord.mExtents[2] );

   //BoundCellsByGranularities();
   
   Add( irPoints );   
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::Init
(  
   FixedArray<Vector3f>&   irPoints,
   FixedArray<int>&        irPointWeights,
   Vector3f&               irGranularities 
)
{
   theVectorLib().FindBounds( mBounds, irPoints );

   mGranularities = irGranularities;
   mBlockCoord.Init( mBounds, mGranularities );

   Allocate( mBlockCoord.mExtents[0] * mBlockCoord.mExtents[1] * mBlockCoord.mExtents[2] );

   //BoundCellsByGranularities();

   Add( irPoints, irPointWeights );   
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
/*void CellBlock::BoundCellsByGranularities()
{
   Bounds3f CurBounds;
   for( int i=mBlockCoord.mExtents[0]; i>=0; i-- )
   {
      for( int j=mBlockCoord.mExtents[1]; j>=0; j-- )
      {
         for( int k=mBlockCoord.mExtents[2]; k>=0; k-- )
         {
            CurBounds.mMin.Set( mBlockCoord.mOffset.x + i*mGranularities.x,
                                mBlockCoord.mOffset.y + j*mGranularities.y,
                                mBlockCoord.mOffset.z + k*mGranularities.z );

            CurBounds.mMax.Set( mBlockCoord.mOffset.x + (i+1)*mGranularities.x,
                                mBlockCoord.mOffset.y + (j+1)*mGranularities.y,
                                mBlockCoord.mOffset.z + (k+1)*mGranularities.z );

            
            GetCell(i,j,k).SetBounds( CurBounds );
         }
      }
   }
}
*/
///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::Add( FixedArray<Vector3f>&  irPoints )
{
   rAssert( IsSetUp() );

   int i;
   for( i=(irPoints.mSize-1); i>-1; i-- )
   {
      mpData[ CellIndex(irPoints[i]) ].AccVertex(irPoints[i]) ;
   }
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::Add
( 
   FixedArray<Vector3f>&  irPoints,
   FixedArray<int>&       irPointWeights
)
{
   rAssert( IsSetUp() );

   int i;
   for( i=(irPoints.mSize-1); i>-1; i-- )
   {
      operator[](CellIndex(irPoints[i])).AccVertex( irPoints[i], irPointWeights[i] );
   }
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::Add( Vector3f& irPoint )
{
   rAssert( IsSetUp() );

   mpData[ CellIndex(irPoint) ].AccVertex(irPoint);
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::GenerateCells()
{
   for( int i=mSize-1; i>-1; i-- )
   {
      mpData[i].GenerateCell();
   }

   SetCellsBlockData();
}

///////////////////////////////////////////////////////////// 
// Translate Extracts the Data from this Object destructively
//
// The Object is emptied of its translation-salient data, as 
// said data takes on a new representation.
///////////////////////////////////////////////////////////// 
void CellBlock::Translate( FixedArray<Cell>& orCellArray, Vector3i& orSpans )
{
   //
   // Extract the array without allocations
   //
   orCellArray.Clear();
   orCellArray.mSize = mSize;
   orCellArray.mpData = mpData;

   mpData = NULL;

   orSpans.Set( mBlockCoord.mExtents );
}
///////////////////////////////////////////////////////////// 
// Only extracts the non-empty cells from the cell matrix
// More expensive than the simple Translate, as it requires
// two iterative passes and an extra alloc and free.
///////////////////////////////////////////////////////////// 
void CellBlock::TranslateNonEmpty( FixedArray<Cell>& orCellArray, Vector3i& orSpans )
{
   int emptyCells = CountEmptyCells();

   orCellArray.Clear();
   orCellArray.Allocate( mSize - emptyCells );

   for( int j = 0, i=mSize-1; i>-1; i-- )
   {
      if( mpData[i].RenderWeight() > 0 )
      {
         orCellArray.mpData[j] = mpData[i];
         j++;
      }
   }

   Clear();

   orSpans.Set( mBlockCoord.mExtents );
}
///////////////////////////////////////////////////////////// 
// Only extracts the non-empty cells from the cell matrix
// More expensive than the simple Translate, as it requires
// two iterative passes and an extra alloc and free.
///////////////////////////////////////////////////////////// 
void CellBlock::ExtractNonEmptyCells( FixedArray<Cell>& orCellArray )
{
   int emptyCells = CountEmptyCells();

   orCellArray.Allocate( mSize - emptyCells );

   for( int j = 0, i=mSize-1; i>-1; i-- )
   {
      if( mpData[i].RenderWeight() > 0 )
      {
         orCellArray.mpData[j] = mpData[i];
         j++;
      }
   }

   Clear();
}
///////////////////////////////////////////////////////////// 
// Counts all the Cells which have not been seeded with 
// non-zero render weights
///////////////////////////////////////////////////////////// 
int CellBlock::CountEmptyCells()
{
   int Count=0;

   for( int i=mSize-1; i>-1; i-- )
   {
      if( mpData[i].RenderWeight() < 1 )
      {
         Count++;
      }
   }
   return Count;
}
///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::ExtractCells( FixedArray<Cell>& orCellArray )
{
   //
   // Extract the array without allocations
   //
   orCellArray.Clear();
   orCellArray.mSize = mSize;
   orCellArray.mpData = mpData;

   mpData = NULL;
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::ExtractDims( Vector3i& orSpans )
{
   orSpans.Set( mBlockCoord.mExtents );
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::ExtractBounds( Bounds3f& orBounds )
{
   orBounds = mBounds;
}

///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
int CellBlock::CellIndex( Vector3f& irPoint )
{
   Vector3f index3f;
   Vector3f AdjustedGranularities;

   AdjustedGranularities.Mult( mGranularities, 1.0f+FloatFuncs::MinRemainder );

   index3f.Sub( irPoint,   mBlockCoord.mOffset);
   index3f.Div( index3f,   AdjustedGranularities );//mGranularities );

   return CellIndex( (int)index3f[0], (int)index3f[1], (int)index3f[2] );
//   return( ((int)(index3f[0]) * mBlockCoord.mExtents[1] * mBlockCoord.mExtents[2]) +  
//           ((int)(index3f[1]) * mBlockCoord.mExtents[2]) +
//            (int)(index3f[2]) );
}

Cell& CellBlock::GetCell( int iX, int iY, int iZ )
{
   return mpData[CellIndex(iX, iY, iZ)];
}

int CellBlock::CellIndex( int iX, int iY, int iZ )
{
   return( ((iX) * mBlockCoord.mExtents[1] * mBlockCoord.mExtents[2]) +  
           ((iY) * mBlockCoord.mExtents[2]) +
            (iZ) );
}
///////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////// 
void CellBlock::SetCellsBlockData()
{
   Bounds3f Bounds;
   Vector3i Index;

   for( Index.mX=0; Index.mX<mBlockCoord.mExtents[0]; Index.mX++ )
   {
      for( Index.mY=0; Index.mY<mBlockCoord.mExtents[1]; Index.mY++ )
      {
         for( Index.mZ=0; Index.mZ<mBlockCoord.mExtents[2]; Index.mZ++ )
         {
            Bounds.mMin.Set(  mBlockCoord.mOffset.x + ((float)Index.mX)*mGranularities.x, 
                              mBlockCoord.mOffset.y + ((float)Index.mY)*mGranularities.y, 
                              mBlockCoord.mOffset.z + ((float)Index.mZ)*mGranularities.z );         
            Bounds.mMax.Set(  Bounds.mMin.x+mGranularities.x, 
                              Bounds.mMin.y+mGranularities.y, 
                              Bounds.mMin.z+mGranularities.z );         
            
            GetCell(Index.mX,Index.mY,Index.mZ).SetBounds(     Bounds );
            GetCell(Index.mX,Index.mY,Index.mZ).SetBlockIndex( Index );
         }
      }
   }
}