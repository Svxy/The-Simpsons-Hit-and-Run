#ifndef __IntersectDSG_H__
#define __IntersectDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IntersectDSG
//
// Description: The IntersectDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/06]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/p3dtypes.hpp>
#include <p3d/geometry.hpp>
#include <p3d/primgroup.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/IEntityDSG.h>
#include <render/Culling/ReserveArray.h>

//========================================================================
//
// Synopsis:   The IntersectDSG; Synopsis by Inspection.
//
//========================================================================
class IntersectDSG : public IEntityDSG 
{
public:
   IntersectDSG();
   IntersectDSG( tGeometry* ipGeometry );
   ~IntersectDSG();
   
   void Display();
//#ifndef RAD_RELEASE
   inline void DrawTri(rmt::Vector* ipTriPts, tColour iColour);
//#endif
   int mFlatTriFast( int& orTriNum, rmt::Vector* iopTriPts, rmt::Vector& orTriNorm );
   int mTri( int inTri, rmt::Vector* opTriPts, rmt::Vector& orTriNorm );
   int nTris();

   virtual void GetBoundingBox(rmt::Box3D* box);
   virtual void GetBoundingSphere(rmt::Sphere* sphere);
   virtual void SetBoundingBox(  float x1, float y1, float z1,
                                 float x2, float y2, float z2);
   virtual void SetBoundingSphere(float x, float y, float z, float radius);
   
   virtual rmt::Vector*       pPosition();
   virtual const rmt::Vector& rPosition();
   virtual void GetPosition( rmt::Vector* ipPosn );   

   virtual int  GetNumPrimGroup();

   //////////////////////////////////////////////////////////////////////////
   // DONT TOUCH, unles you know what you're doing
   //////////////////////////////////////////////////////////////////////////
   ReserveArray<int>             mTriIndices;
   ReserveArray<rmt::Vector>     mTriPts;       //Separated Triangles
   ReserveArray<rmt::Vector>     mTriNorms;     //Triangle Normals
   ReserveArray<unsigned char>   mTerrainType;  // The terrain type for the triangle.

   //////////////////////////////////////////////////////////////////////////
   // Pre and Post calls for ScratchPad
   //////////////////////////////////////////////////////////////////////////
   void IntoTheVoid_WithGoFastaStripes();
   void OutOfTheVoid_WithGoFastaStripes();
   //////////////////////////////////////////////////////////////////////////
   // Used to cache real data ptrs while data is uploaded to ScratchPad
   //////////////////////////////////////////////////////////////////////////
   static int*           mspIndexData;
   static unsigned char* mspTerrainData;
   static rmt::Vector*   mspVertexData;
   static rmt::Vector*   mspNormalData;
   static bool           msbInScratchPad;

//   ReserveArray<rmt::Vector>     mTriCentroids; //Triangle Centroids
//   ReserveArray<float>           mTriRadius;    //Triangle Radius; the approximate 
                                                // length of the furthest pt from the centroid

   int mnPrimGroups;

   rmt::Box3D  mBox3D;
   rmt::Sphere mSphere;
   rmt::Vector mPosn;

   enum
   {
       UNINIT_PT= 0x0,
       X_GT_PT  = 0x0001,
       X_LT_PT  = 0x0002,
       Z_GT_PT  = 0x0004,
       Z_LT_PT  = 0x0008,
       INIT_PT  = 0x0010,
       FOUND_PT = 0X001F
   };

protected:
   void GenIDSG( tGeometry* ipGeometry );

   void PreParseTris( tPrimGroupStreamed* ipPrimGroup );
   void PreParseTriStrips( tPrimGroupStreamed* ipPrimGroup );
   void DoAllAllocs();
   void ParseTris( tPrimGroupStreamed* ipPrimGroup );
   void ParseTriStrips( tPrimGroupStreamed* ipPrimGroup );
   void CalcAllFields();

private:
};

#endif
