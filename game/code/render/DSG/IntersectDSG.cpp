//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        IntersectDSG.cpp
//
// Description: Implementation for IntersectDSG class.
//
// History:     Implemented	                         --Devin [5/6/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/geometry.hpp>
#include <p3d/primgroup.hpp>
#include <p3d/vertexlist.hpp>
#include <raddebug.hpp>
#include <pddi/pddiext.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/IntersectDSG.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************
int*           IntersectDSG::mspIndexData   = NULL;
unsigned char* IntersectDSG::mspTerrainData = NULL;
rmt::Vector*   IntersectDSG::mspVertexData  = NULL;
rmt::Vector*   IntersectDSG::mspNormalData  = NULL;
bool           IntersectDSG::msbInScratchPad= false;

//************************************************************************
//
// Public Member Functions : IntersectDSG Interface
//
//************************************************************************
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
IntersectDSG::IntersectDSG( tGeometry* ipGeometry )
{
   GenIDSG(ipGeometry);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
IntersectDSG::IntersectDSG()
{
}

//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
IntersectDSG::~IntersectDSG()
{
}
//========================================================================
// intersectdsg::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
//#ifndef RAD_RELEASE
inline void IntersectDSG::DrawTri(rmt::Vector* ipTriPts, tColour iColour)
{
   pddiPrimStream* stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 4);
   stream->Colour(iColour);
   stream->Coord(ipTriPts[0].x, ipTriPts[0].y, ipTriPts[0].z);
   stream->Colour(iColour);
   stream->Coord(ipTriPts[1].x, ipTriPts[1].y, ipTriPts[1].z);
   stream->Colour(iColour);
   stream->Coord(ipTriPts[2].x, ipTriPts[2].y, ipTriPts[2].z);    
   stream->Colour(iColour);
   stream->Coord(ipTriPts[0].x, ipTriPts[0].y, ipTriPts[0].z);
   p3d::pddi->EndPrims(stream);
}
//#endif
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::Display()
{
   //Currently unsupported function. Contact Devin.
 // rAssert(false);
//#ifndef RAD_RELEASE
    rmt::Vector triPts[3], triNorm;
    for(int i = nTris()-1; i>-1; i--)
    {
        mTri(i,triPts,triNorm);
        DrawTri(triPts,tColour(255,255,255));
    }
//#endif
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
const rmt::Vector& IntersectDSG::rPosition()
{
   return mPosn;
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
rmt::Vector* IntersectDSG::pPosition()
{
   return &mPosn;
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::GetPosition( rmt::Vector* ipPosn )
{
   *ipPosn = mPosn;
}
//========================================================================
// intersectdsg::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::IntoTheVoid_WithGoFastaStripes()
{
    mspVertexData     = mTriPts.mpData;
    mspNormalData     = mTriNorms.mpData;
    mspIndexData      = mTriIndices.mpData;
    mspTerrainData    = mTerrainType.mpData;
    msbInScratchPad   = false;
#ifdef RAD_PS2
   static pddiExtPS2Control* ps2Control = (pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL);

   //////////////////////////////////////////////////////////////////////////
   // If we can fit on the scratchpad used by the mFIFO
   //////////////////////////////////////////////////////////////////////////
   if( ( sizeof(*mTriPts.mpData)*mTriPts.mUseSize 
       + sizeof(*mTriNorms.mpData)*mTriNorms.mUseSize 
       + sizeof(*mTriIndices.mpData)*mTriIndices.mUseSize 
       + sizeof(*mTerrainType.mpData)*mTerrainType.mUseSize ) < 8192 ) 
   {
      msbInScratchPad = true;
      ps2Control->SyncScratchPad();


      char* pScratchPad = (char*)0x70000000;

      if(mTriPts.mUseSize>0)
      {
         memcpy(pScratchPad, mTriPts.mpData, mTriPts.mUseSize*sizeof(*mTriPts.mpData) );
         mTriPts.mpData = (rmt::Vector*)pScratchPad;
         pScratchPad+=mTriPts.mUseSize*sizeof(*mTriPts.mpData);
         for(int i=mTriPts.mUseSize-1; i>-1; i--)
         {
             ((int&)(mTriPts[i].y)) = IntersectDSG::UNINIT_PT;
         }
      }

      if(mTriNorms.mUseSize>0)
      {
         memcpy(pScratchPad, mTriNorms.mpData, mTriNorms.mUseSize*sizeof(*mTriNorms.mpData) );
         mTriNorms.mpData = (rmt::Vector*)pScratchPad;
         pScratchPad+=mTriNorms.mUseSize*sizeof(*mTriNorms.mpData);
      }

      if(mTriIndices.mUseSize>0)
      {
         memcpy(pScratchPad, mTriIndices.mpData, mTriIndices.mUseSize*sizeof(*mTriIndices.mpData) );
         mTriIndices.mpData = (int*)pScratchPad;
         pScratchPad+=mTriIndices.mUseSize*sizeof(*mTriIndices.mpData);
      }

      if(mTerrainType.mUseSize>0)
      {
         memcpy(pScratchPad, mTerrainType.mpData, mTerrainType.mUseSize*sizeof(*mTerrainType.mpData) );
         mTerrainType.mpData = (unsigned char*)pScratchPad;
         pScratchPad+=mTerrainType.mUseSize*sizeof(*mTerrainType.mpData);
      }

      //if(((int)pScratchPad)-0x70000000 > 8192)
      //    rReleasePrintf("***ack! Overwriting ScratchPad memory for something non mFIFO\n" );
   }
#endif
}
//========================================================================
// intersectdsg::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::OutOfTheVoid_WithGoFastaStripes()
{
    mTriPts.mpData        = mspVertexData;
    mTriNorms.mpData      = mspNormalData;
    mTriIndices.mpData    = mspIndexData;
    mTerrainType.mpData   = mspTerrainData;
    msbInScratchPad = false;
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int IntersectDSG::mFlatTriFast
( 
   int& orTriNum,
   rmt::Vector* iopTriPts, 
   rmt::Vector& orTriNorm
)
{
    const float PT_EPSILON = 0.01f;
    rmt::Vector posnRef = iopTriPts[0];
    int Mask = IntersectDSG::UNINIT_PT;
    rReleaseAssert(mTriIndices.mUseSize > 0);

    rAssert(mTriIndices.IsSetUp());

    int i, j;

    if(msbInScratchPad)
    {
        for(i=orTriNum; Mask!=IntersectDSG::FOUND_PT && i>-1; i--)
        {
            Mask = IntersectDSG::UNINIT_PT;
            //////////////////////////////////////////////////////////////////////////
            // Intentional Copy n Paste is done to ensure inlining
            //////////////////////////////////////////////////////////////////////////
            j = mTriIndices[i*3];
            
            int* pPtFlags = &((int&)(mTriPts[j].y));//rPtFlags = Mask;
            if( *pPtFlags == IntersectDSG::UNINIT_PT )//!(rPtFlags & IntersectDSG::INIT_PT) )
            {
                //Initialise Point; intentional overlap. See Mask.
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_GT_PT;            
                (*pPtFlags) |= IntersectDSG::INIT_PT;
            }
            Mask |= (*pPtFlags);
            
            j = mTriIndices[i*3+1];
            pPtFlags = &((int&)(mTriPts[j].y));//rPtFlags = Mask;
            if( (*pPtFlags) == IntersectDSG::UNINIT_PT )//!(rPtFlags & IntersectDSG::INIT_PT) )
            {
                //Initialise Point; intentional overlap. See Mask.
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_GT_PT;            
                (*pPtFlags) |= IntersectDSG::INIT_PT;
            }
            Mask |= (*pPtFlags);
            
            j = mTriIndices[i*3+2];
            pPtFlags = &((int&)(mTriPts[j].y));//rPtFlags = Mask;
            if( (*pPtFlags) == IntersectDSG::UNINIT_PT )//!(rPtFlags & IntersectDSG::INIT_PT) )
            {
                //Initialise Point; intentional overlap. See Mask.
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     (*pPtFlags) |= IntersectDSG::Z_GT_PT;            
                (*pPtFlags) |= IntersectDSG::INIT_PT;
            }
            Mask |= (*pPtFlags);

            if(Mask==IntersectDSG::FOUND_PT)
            {
                j=mTriIndices[i*3];   *iopTriPts      = mTriPts[j];  iopTriPts->y     = mspVertexData[j].y; 
                j=mTriIndices[i*3+1]; *(iopTriPts+1)  = mTriPts[j];  (iopTriPts+1)->y = mspVertexData[j].y; 
                j=mTriIndices[i*3+2]; *(iopTriPts+2)  = mTriPts[j];  (iopTriPts+2)->y = mspVertexData[j].y;
                orTriNum=i;

                orTriNorm = mTriNorms[orTriNum];

                if( mTerrainType.IsSetUp() )
                {
                    return mTerrainType[orTriNum];
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    else //NotInScratchPad
    {
        for(i=orTriNum; Mask!=IntersectDSG::FOUND_PT && i>-1; i--)
        {
            Mask |= IntersectDSG::INIT_PT;
        //////////////////////////////////////////////////////////////////////////
            // Intentional Copy n Paste is done to ensure inlining
            //////////////////////////////////////////////////////////////////////////
            j = mTriIndices[i*3];
            {
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     Mask |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     Mask |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     Mask |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     Mask |= IntersectDSG::Z_GT_PT;            
            }
            
            j = mTriIndices[i*3+1];
            {
                //Initialise Point; intentional overlap. See Mask.
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     Mask |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     Mask |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     Mask |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     Mask |= IntersectDSG::Z_GT_PT;            
            }
 
            j = mTriIndices[i*3+2];
            {
                //Initialise Point; intentional overlap. See Mask.
                if( mTriPts[j].x <= (posnRef.x + PT_EPSILON) )     Mask |= IntersectDSG::X_LT_PT;
                if( mTriPts[j].x >= (posnRef.x - PT_EPSILON) )     Mask |= IntersectDSG::X_GT_PT;
                if( mTriPts[j].z <= (posnRef.z + PT_EPSILON) )     Mask |= IntersectDSG::Z_LT_PT;
                if( mTriPts[j].z >= (posnRef.z - PT_EPSILON) )     Mask |= IntersectDSG::Z_GT_PT;            
            }

            if(Mask==IntersectDSG::FOUND_PT)
            {
                j=mTriIndices[i*3];   *iopTriPts      = mTriPts[j];  
                j=mTriIndices[i*3+1]; *(iopTriPts+1)  = mTriPts[j];  
                j=mTriIndices[i*3+2]; *(iopTriPts+2)  = mTriPts[j]; 
                orTriNum=i;

                orTriNorm = mTriNorms[orTriNum];

                if( mTerrainType.IsSetUp() )
                {
                    return mTerrainType[orTriNum];
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    return -1;
    //rTuneAssert(orTriNum==i+1);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int IntersectDSG::mTri
( 
   int inTri, 
   rmt::Vector* opTriPts, 
   rmt::Vector& orTriNorm//, 
   //rmt::Vector& orTriCtr,
   //int* opTerrainType
)
{
    //rReleaseAssert(     ((((int)mspVertexData)&0x70000000)==0)
    //                &&  ((((int)mspNormalData)&0x70000000)==0)
    //                &&  ((((int)mspIndexData)&0x70000000)==0)
    //                &&  ((((int)mspTerrainData)&0x70000000)==0)); 

    if( mTriIndices.mUseSize > 0 )
    {
        rAssert(mTriIndices.IsSetUp());

        *opTriPts      = mTriPts[mTriIndices[inTri*3]];
        *(opTriPts+1)  = mTriPts[mTriIndices[inTri*3+1]];
        *(opTriPts+2)  = mTriPts[mTriIndices[inTri*3+2]];

        orTriNorm = mTriNorms[inTri];
        //orTriCtr  = mTriCentroids[inTri];
        //if( opTerrainType )
        {
            if( mTerrainType.IsSetUp() )
            {
                //*opTerrainType = mTerrainType[ inTri ];
                return mTerrainType[ inTri ];
            }
            else
            {
                //*opTerrainType = 0;
                return 0;
            }
        }
        //return 0.0f;// mTriRadius[inTri];
    }
    else
    {
        rAssert(mTriPts.IsSetUp());

        *opTriPts      = mTriPts[inTri*3];
        *(opTriPts+1)  = mTriPts[inTri*3+1];
        *(opTriPts+2)  = mTriPts[inTri*3+2];

        orTriNorm = mTriNorms[inTri];
        //orTriCtr  = mTriCentroids[inTri];
        //if( opTerrainType )
        {
            if( mTerrainType.IsSetUp() )
            {
                //*opTerrainType = mTerrainType[ inTri ];
                return mTerrainType[ inTri ];
            }
            else
            {
                //*opTerrainType = 0;
                return 0;
            }
        }
        //return mTriRadius[inTri];
    }

}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int IntersectDSG::nTris()
{
    if( mTriIndices.mUseSize > 0 )
    {
        rAssert(mTriIndices.IsSetUp());

        return mTriIndices.mUseSize/3;   
    }
    else
    {
        rAssert(mTriPts.IsSetUp());

        return mTriPts.mUseSize / 3;
    }
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::GetBoundingBox(rmt::Box3D* box)
{
   *box = mBox3D;
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
   *sphere = mSphere;
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::SetBoundingBox
(  
   float x1, float y1, float z1,
   float x2, float y2, float z2
)
{
   mBox3D.low.Set(x1,y1,z1);
   mBox3D.high.Set(x2,y2,z2);

   mPosn.Sub(mBox3D.high, mBox3D.low);
   mPosn *= 0.5f;
   mPosn.Add(mBox3D.low);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::SetBoundingSphere(float x, float y, float z, float radius)
{
   mSphere.centre.Set(x,y,z);
   mSphere.radius = radius;      

   mPosn.Set(x,y,z);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int IntersectDSG::GetNumPrimGroup()
{
   return mnPrimGroups;
}

//************************************************************************
//
// Protected Member Functions : IntersectDSG 
//
//************************************************************************
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::GenIDSG( tGeometry* ipGeometry )
{
   int i;

   ipGeometry->GetBoundingBox( &mBox3D );
   ipGeometry->GetBoundingSphere( &mSphere );

   SetUID(ipGeometry->GetUID());
   CopyName(ipGeometry);
   mnPrimGroups = ipGeometry->GetNumPrimGroup();

   for( i=0; i<mnPrimGroups; i++ )
   {
      switch( ipGeometry->GetPrimGroup(i)->GetPrimType() ) 
      {
      case PDDI_PRIM_TRIANGLES:
         PreParseTris((tPrimGroupStreamed*)ipGeometry->GetPrimGroup(i));
      	break;
      case PDDI_PRIM_TRISTRIP:
         PreParseTriStrips((tPrimGroupStreamed*)ipGeometry->GetPrimGroup(i));
      	break;
      default:
         rAssert(false);
         break;
      }
   }

   DoAllAllocs();

//   for( int i=ipGeometry->GetNumPrimGroup()-1; i>-1; i-- )
   for( i=0; i<mnPrimGroups; i++ )
   {
      switch( ipGeometry->GetPrimGroup(i)->GetPrimType() ) 
      {
      case PDDI_PRIM_TRIANGLES:
         ParseTris((tPrimGroupStreamed*)ipGeometry->GetPrimGroup(i));
      	break;
      case PDDI_PRIM_TRISTRIP:
         ParseTriStrips((tPrimGroupStreamed*)ipGeometry->GetPrimGroup(i));
      	break;
      default:
         rAssert(false);
         break;
      }
   }
   CalcAllFields();
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::PreParseTris( tPrimGroupStreamed* ipPrimGroup )
{
#if 0
    int n = ipPrimGroup->GetVertexCount();
    mTriPts.Reserve(n); 
    int m = ipPrimGroup->GetNumIndices();

    // don't reserve indices if this is de-indexed geometry
    if (m > 0)
    {
        mTriIndices.Reserve(m);
        m=m/3;
        mTriNorms.Reserve(m);
        mTriCentroids.Reserve(m);
        mTriRadius.Reserve(m);
    }
    else
    {
        n=n/3;
        mTriNorms.Reserve(n);
        mTriCentroids.Reserve(n);
        mTriRadius.Reserve(n);
    }
#endif
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::PreParseTriStrips( tPrimGroupStreamed* ipPrimGroup )
{
   //Currently unsupported feature; contact Devin
   rAssert(false);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::DoAllAllocs()
{
#if 0
    mTriIndices.Allocate();
   mTriPts.Allocate(); 
   mTriNorms.Allocate();
   mTriCentroids.Allocate();
   mTriRadius.Allocate();
#endif
}

//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::ParseTris( tPrimGroupStreamed* ipPrimGroup )
{
#if 0
    int nVerts = ipPrimGroup->GetVertexCount();
    int nStart = mTriPts.mUseSize;

    //mTriPts.AddUse(nVerts);

    rmt::Vector* pVerts = ipPrimGroup->GetVertexList()->GetPositions();


    //memcpy(mTriPts.mpData+nStart, pVerts, sizeof(rmt::Vector)*nVerts);
    for( int i=0; i<nVerts; i++ )
    {
        mTriPts.Add(*(pVerts+i));
    }
    //////////////////////////////////////////////////////////////////////////
    int nIndices = ipPrimGroup->GetNumIndices();
    if( nIndices > 0 )
    {
        rAssert(nStart+nVerts<=mTriIndices.mSize);

        //      nStart = mTriIndices.mUseSize;
        //      mTriIndices.AddUse(nVerts);

        unsigned short* pIndices = ipPrimGroup->GetIndices();

        rAssert(nStart+nIndices<=mTriIndices.mSize);

        //      memcpy(mTriIndices.mpData+nStart, pIndices, sizeof(unsigned short)*nVerts);
        //      for( int i=nVerts-1; i>-1; i-- )
        for( int i=0; i<nIndices; i++ )
        {
            unsigned long FuckU_MS = (*(pIndices+i))+(nStart);
            mTriIndices.Add( FuckU_MS );
        }

    }
#endif
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void IntersectDSG::ParseTriStrips( tPrimGroupStreamed* ipPrimGroup )
{
   //Currently unsupported feature; contact Devin
   rAssert(false);
}
//========================================================================
// IntersectDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//======================================================================== 
void IntersectDSG::CalcAllFields()
{
#if 0
   rmt::Vector tmpA, tmpB;
   rmt::Vector *pPt0, *pPt1, *pPt2;

   if(mTriIndices.mUseSize > 0) //Indexed Geometry
   {
      mTriNorms.AddUse(    mTriIndices.mUseSize/3);
      mTriCentroids.AddUse(mTriIndices.mUseSize/3);
      mTriRadius.AddUse(   mTriIndices.mUseSize/3);

//      for( int i=mTriIndices.mUseSize-1; i>-1; i-=3 )
      for( int i=0; i<mTriIndices.mUseSize; i+=3 )
      {
         pPt0 = &mTriPts[mTriIndices[i]];
         pPt1 = &mTriPts[mTriIndices[i+1]];
         pPt2 = &mTriPts[mTriIndices[i+2]];

         tmpA.Sub( *pPt1, *pPt0 );
         tmpB.Sub( *pPt2, *pPt0 );

         mTriNorms[i/3].CrossProduct( tmpA, tmpB );
         mTriNorms[i/3].Normalize();
/*
         if( mTriNorms[i/3].y < 0.1f )
         {
            rDebugPrintf("Warning: -y in Terrain Intersect Normal\n");
            mTriNorms[i/3].Scale(-1.0f);
//            mTriNorms[i/3].Set(0.0f,1.0f,0.0f);
         }
  */       
         tmpA.Add( *pPt0, *pPt1);
         tmpA.Add( *pPt2 );
         tmpA.Scale( 0.33333333f );

         mTriCentroids[i/3] = tmpA;

         tmpA.Sub( *pPt2, tmpA ); 
         tmpB.Sub( *pPt1, mTriCentroids[i/3] ); 

         if( tmpB.MagnitudeSqr() > tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         tmpB.Sub( *pPt0, mTriCentroids[i/3] );

         if( tmpB.MagnitudeSqr() > tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         mTriRadius[i/3] = tmpA.Magnitude();
      }
   }
   else //De-indexed Geometry
   {
      mTriNorms.AddUse(    mTriPts.mUseSize/3);
      mTriCentroids.AddUse(mTriPts.mUseSize/3);
      mTriRadius.AddUse(   mTriPts.mUseSize/3);

      for( int i=0; i < mTriPts.mUseSize; i += 3 )
      {
         pPt0 = &mTriPts[i];
         pPt1 = &mTriPts[i+1];
         pPt2 = &mTriPts[i+2];

         tmpA.Sub( *pPt1, *pPt0 );
         tmpB.Sub( *pPt2, *pPt0 );

         mTriNorms[i/3].CrossProduct( tmpA, tmpB );
         mTriNorms[i/3].Normalize();

         tmpA.Add( *pPt0, *pPt1);
         tmpA.Add( *pPt2 );
         tmpA.Scale( 0.33333333f );

         mTriCentroids[i/3] = tmpA;

         tmpA.Sub( *pPt2, tmpA ); 
         tmpB.Sub( *pPt1, mTriCentroids[i/3] ); 

         if( tmpB.MagnitudeSqr() > tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         tmpB.Sub( *pPt0, mTriCentroids[i/3] );

         if( tmpB.MagnitudeSqr() > tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         mTriRadius[i/3] = tmpA.Magnitude();
         
/*         tmpA.Sub( mTriPts[i+1], mTriPts[i] );
         tmpB.Sub( mTriPts[i+2], mTriPts[i] );

         mTriNorms[i/3].CrossProduct( tmpA, tmpB );

         tmpA.Add( mTriPts[i], mTriPts[i+1] );
         tmpA.Add( mTriPts[i+2] );
         tmpA.Scale( 0.33333333f );

         mTriCentroids[i/3] = tmpA;

         tmpA.Sub( mTriPts[i+2], tmpA );
         tmpB.Sub( mTriPts[i+1], mTriCentroids[i/3] );

         if( tmpB.MagnitudeSqr() < tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         tmpB.Sub( mTriPts[i], mTriCentroids[i/3] );

         if( tmpB.MagnitudeSqr() < tmpA.MagnitudeSqr() )
            tmpA = tmpB;

         mTriRadius[i/3] = tmpA.MagnitudeSqr();*/
      }
   }
#endif
}

//************************************************************************
//
// Private Member Functions : IntersectDSG 
//
//************************************************************************
