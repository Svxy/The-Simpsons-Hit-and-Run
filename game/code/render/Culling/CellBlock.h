#ifndef __CELL_BLOCK_H__
#define __CELL_BLOCK_H__

#include <render/culling/Cell.h>
#include <render/culling/FixedArray.h>
#include <render/culling/BlockCoord.h>
#include <render/culling/Vector3i.h>

class CellBlock : public FixedArray<Cell>
{
public:
   CellBlock();
   ~CellBlock();

   void Add( FixedArray<Vector3f>&  irPoints );
   void Add( Vector3f&              irPoint );
   void Add( FixedArray<Vector3f>&  irPoints,
             FixedArray<int>&       irPointWeights);


 /*
   void Init(  int         iSize, 
               BlockCoord& irBlockCoord, 
               Vector3f&   irGranularities );
   */

   void Init(  FixedArray<Vector3f>&   irPoints, 
               Vector3f&               irGranularities );

   void Init(  FixedArray<Vector3f>&   irPoints,
               FixedArray<int>&        irPointWeights,
               Vector3f&               irGranularities );

   void GenerateCells();
   
   int   CellIndex(  Vector3f& irPoint );
   int   CellIndex(  int iX, int iY, int iZ );
   Cell& GetCell(    int iX, int iY, int iZ );

   void Translate( FixedArray<Cell>& orCellArray, Vector3i& orSpans );
   void TranslateNonEmpty( FixedArray<Cell>& orCellArray, Vector3i& orSpans );

   void ExtractNonEmptyCells( FixedArray<Cell>& orCellArray );
   void ExtractCells(         FixedArray<Cell>& orCellArray );
   void ExtractDims(    Vector3i& orSpans );
   void ExtractBounds(  Bounds3f& orBounds );

   Bounds3f    mBounds;
   BlockCoord  mBlockCoord;
   Vector3f    mGranularities;
protected:
   void SetCellsBlockData();
   int  CountEmptyCells();
   void BoundCellsByGranularities();
};
#endif