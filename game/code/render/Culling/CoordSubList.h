#ifndef __COORD_SUB_LIST_H__
#define __COORD_SUB_LIST_H__

#include <render/culling/OctTreeConst.h>
#include <render/culling/Bounds.h>
#include <render/culling/Cell.h>
#include <render/culling/Vector3i.h>

//
// CoordSubList
//
// Contains:
//    -An mSize list of points in mpVectorList 
//       which reaches extents of mBounds
//
// Responsible for:
//    -Subdividing a list of points
//    -NOT responsisble for the data mpVectorList as it is expected that
//       other SubLists will reference the same data
//
class CoordSubList
{
public:
   ~CoordSubList();
   CoordSubList( Cell* ipCellList, int iSize, int iWeightedSize, Bounds3f& irBounds );
   CoordSubList( Cell* ipCellList, int iSize, Vector3i iSpans );
   CoordSubList( Cell* ipCellList, int iSize );

   //
   // SubDivide the List in-place
   //
   void SubDivideHeuristic( Vector3f&      irGranularity, 
                            CoordSubList** opList1, 
                            CoordSubList** opList2, 
                            int&           iorAxis,
                            float&         orPlanePosn );
   bool CanBeSplit();

   //
   // SubDivide the List in-place
   //
   void SubDivideUniform( int iAxis, CoordSubList** opList1, CoordSubList** opList2 );

   int WeightedSize();
   int Size();

   Cell& Get( int i );
protected:
   //////////////////////////////////////////////////////////////////
   // Class Data
   //////////////////////////////////////////////////////////////////
   static const char smAxis0 = 1;
   static const char smAxis1 = 1 << 1;
   static const char smAxis2 = 1 << 2;

   //////////////////////////////////////////////////////////////////
   // Data
   //////////////////////////////////////////////////////////////////
   int         mSize;
   int         mWeightedSize; 
   Cell*       mpCellList;
   Vector3i    mSpans;


   //////////////////////////////////////////////////////////////////
   // Methods
   //////////////////////////////////////////////////////////////////
//   CoordSubList( Cell* ipCellList, int iSize );
   CoordSubList();

   int   ChooseAxis();
   char  BestSpreadAxis();


   int   GetWeightedMedian( int iAxis );
   int   GetWeightedMedian( float& orPlanePosn, int iAxis );
   int   GetPlanarDivision( float& orPlanePosn, int iAxis, int iIndex );

   float GetCellUpperBound( float iCoord, float iGranularity );
   //   int   GetWeightedMedian( int iAxis, int* opLowerWeightCount, tBox3D* opBounds );

   int   FindSplitIndex( int& ioAxis, int iDeviationThreshold );
   void  BinCount( int& oMedianDeviation, int& oMedian, int iAxis );
   int   BinSort( int oMedian, int iAxis );
   void  SeekMinPlane( float& oPlanePosn, int i, int iAxis );
   void  SeekMaxPlane( float& oPlanePosn, int iCur, int iEnd, int iAxis );

   //
   // Generate deterministic members
   //
   void GenerateMembers();
   void GenWeightedSize();
   void GenSpans();
};

#endif