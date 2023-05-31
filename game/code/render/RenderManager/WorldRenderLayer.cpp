//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WorldRenderLayer.cpp
//
// Description: Implementation for WorldRenderLayer class.
//
// History:     Implemented	                         --Devin [4/23/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <raddebugwatch.hpp>
#include <radtime.hpp>
#include <p3d/billboardobject.hpp>

//========================================
// Project Includes
//========================================
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/DSG/DynaPhysDSG.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/IntersectDSG.h>
#include <render/DSG/StaticEntityDSG.h>
#include <render/DSG/FenceEntityDSG.h>
#include <render/DSG/AnimCollisionEntityDSG.h>
#include <render/DSG/AnimEntityDSG.h>
#include <render/DSG/WorldSphereDSG.h>
#include <roads/roadsegment.h>
#include <pedpaths/pathsegment.h>
#include <meta/triggervolume.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/character/footprint/footprintmanager.h>

#include <worldsim/WorldPhysicsManager.h>

#include <meta/triggervolumetracker.h> // HACK for drawing the trigger volumes

#include <render/breakables/breakablesmanager.h>
#include <render/particles/particlemanager.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>

#include <p3d/shadow.hpp>
#include <p3d/view.hpp>

#include <events/eventmanager.h>
#include <events/eventenum.h>

#include <data/persistentworldmanager.h>

#include <ai/vehicle/vehicleairender.h>

#include <render/Loaders/BillboardWrappedLoader.h>
#include <worldsim/avatarmanager.h>

#ifdef RAD_PS2
#define DEFAULT_R 67;
#define DEFAULT_G 67;
#define DEFAULT_B 67;
#elif defined(RAD_XBOX)
#define DEFAULT_R 67;
#define DEFAULT_G 67;
#define DEFAULT_B 67;
#elif defined(RAD_WIN32)
#define DEFAULT_R 67;
#define DEFAULT_G 67;
#define DEFAULT_B 67;
#else
#define DEFAULT_R 67;
#define DEFAULT_G 67;
#define DEFAULT_B 67;
#endif

static unsigned char gWashColourR = DEFAULT_R;
static unsigned char gWashColourG = DEFAULT_G;
static unsigned char gWashColourB = DEFAULT_B;

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : WorldRenderLayer Interface
//
//************************************************************************

//========================================================================
// WorldRenderLayer::WorldRenderLayer
//========================================================================
//
// Description: Do some init stuff
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
WorldRenderLayer::WorldRenderLayer()
{
   mQdDump = false;
   OnWorldRenderLayerInit();
   //mpShadowGenerator = NULL;    // VolShadows

#ifdef DEBUGWATCH
   static bool firstTimeAdding = true;

   radDbgWatchAddUnsignedInt( &mDebugInnerRenderTime, "Inner Render Time", "World Render Layer" );
   radDbgWatchAddUnsignedInt( &mDebugRenderTime,      "Render Time",       "World Render Layer" );
   radDbgWatchAddUnsignedInt( &mDebugGutsTime,        "Guts Time",         "World Render Layer" );

   if ( firstTimeAdding )
   {
       radDbgWatchAddUnsignedChar( &gWashColourR, "Colour R", "Shadows" );
       radDbgWatchAddUnsignedChar( &gWashColourG, "Colour G", "Shadows" );
       radDbgWatchAddUnsignedChar( &gWashColourB, "Colour B", "Shadows" );
       firstTimeAdding = false;
   }
#endif

   mMirror = false;
}

//========================================================================
// WorldRenderLayer::~WorldRenderLayer()
//========================================================================
//
// Description: Quick! to the Garbage heap!
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
WorldRenderLayer::~WorldRenderLayer()
{
   NullifyGuts();
   //delete mpShadowGenerator;    // VolShadows
   
#ifdef DEBUGWATCH
   static bool firstTimeRemoving = true;

   radDbgWatchDelete( &mDebugInnerRenderTime );
   radDbgWatchDelete( &mDebugRenderTime );
   radDbgWatchDelete( &mDebugGutsTime );

   if ( firstTimeRemoving )
   {
       radDbgWatchDelete( &gWashColourR );
       radDbgWatchDelete( &gWashColourG );
       radDbgWatchDelete( &gWashColourB );
       firstTimeRemoving = false;
   }
#endif

}

static bool simpleShadows = true;

//////////////////////////////////////////////////////////////////////////
// Render Interface
//////////////////////////////////////////////////////////////////////////
//========================================================================
// WorldRenderLayer::Render
//========================================================================
//
// Description: Whacha got? Render it.
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldRenderLayer::Render()
{
    BEGIN_PROFILE( "WRL Render" );
#ifdef DEBUGWATCH
    mDebugRenderTime = radTimeGetMicroseconds();
#endif
    rAssert(!IsDead());

    BEGIN_PROFILE( "pddi ZBuf" );
    p3d::pddi->EnableZBuffer(true);
    END_PROFILE( "pddi ZBuf" );

    // VolShadows
    /*
    if(mpShadowGenerator == NULL)
    {
         mpShadowGenerator = new tShadowGenerator;
    }

    BEGIN_PROFILE( "PreRender Shadows" );
    mpShadowGenerator->PreRender();
    END_PROFILE( "PreRender Shadows" );
    */

    // Hack for the number of players
    for ( unsigned int view = 0; view < GetNumViews(); view++ )
    {
#ifdef DEBUGWATCH
        mDebugInnerRenderTime = radTimeGetMicroseconds();
#endif

        for(int mirrorPass = mMirror ? 1 : 0; mirrorPass >= 0; mirrorPass--)
        {
            rmt::Matrix originalCamera;
            if(mirrorPass == 1)
            {
                tCamera * camera = mpView[ view ]->GetCamera();
                originalCamera = camera->GetCameraToWorldMatrix();
                rmt::Matrix mirroredCamera;
                mirroredCamera.Mult(originalCamera, mMirrorMatrix);
                camera->SetCameraMatrix(&mirroredCamera);
            }

            if(mMirror && (mirrorPass == 0))
            {
                mpView[ view ]->SetClearMask(PDDI_BUFFER_DEPTH);
            }
            else
            {
                mpView[ view ]->SetClearMask(PDDI_BUFFER_ALL);
            }

 		    BEGIN_PROFILE( "View Begin Render" );
            mpView[ view ]->BeginRender();
		    END_PROFILE( "View Begin Render" );

            int i;
            
            if(!mMirror)
            {
                BEGIN_PROFILE( "Render World Spheres" );
                p3d::pddi->EnableZBuffer(false);
                for(i=0; i<mWorldSpheres.mUseSize; i++)
                {
                    mWorldSpheres[i]->Display();
                }
 		        BEGIN_PROFILE( "pddi ZBuf" );
                p3d::pddi->EnableZBuffer(true);
 		        END_PROFILE( "pddi ZBuf" );
                END_PROFILE( "Render World Spheres" );
            }


            BEGIN_PROFILE( "Render WorldScene" );
            mpWorldScene->Render( view );
    #ifdef DEBUGWATCH
        mDebugInnerRenderTime = radTimeGetMicroseconds()-mDebugInnerRenderTime;
    #endif
            END_PROFILE( "Render WorldScene" );

            //p3d::inventory->PushSection();
            //p3d::inventory->SelectSection("Default");

            mpWorldScene->RenderOpaque();

            BEGIN_PROFILE( "Render coins" );
            GetCoinManager()->Render();
            END_PROFILE( "Render coins" );

            // VolShadows
            /*
            BEGIN_PROFILE( "Render Shadows" );
            // let's draw ourselves some shadows
		    mpShadowGenerator->Begin();
		    mpWorldScene->RenderShadows();
		    tColour washColour(gWashColourR, gWashColourG, gWashColourB);
		    mpShadowGenerator->SetWashColour(washColour);
		    mpShadowGenerator->End();
            END_PROFILE( "Render Shadows" );
            */
            GetFootprintManager()->Render();
            BEGIN_PROFILE( "Render Simple Shadows" );
            mpWorldScene->RenderSimpleShadows();
            END_PROFILE( "Render Simple Shadows" );

        //Temp Disable BBQ optimisation for cars, as it may be outstripped by
        // Kevin's fix to the art
            BEGIN_PROFILE( "Render Shadow Casters" );
    	    //mpWorldScene->RenderShadowCasters();
            END_PROFILE( "Render Shadow Casters" );


            BEGIN_PROFILE( "RenderTranslucent" );
            mpWorldScene->RenderTranslucent();
            END_PROFILE( "RenderTranslucent" );


        BillboardQuadManager::Enable();
        BEGIN_PROFILE( "BBQ Display All" );
        GetBillboardQuadManager()->DisplayAll();
		END_PROFILE( "BBQ Display All" );
        BillboardQuadManager::Disable();

        BEGIN_PROFILE( "RenderSparkles" );
        // Render the procedural particle effects:
        // coin glints, collection/spawn trail sparkles, hit sparks, etc.
        GetSparkleManager()->Render( Sparkle::SRM_ExcludeSorted );

            END_PROFILE( "RenderSparkles" );
            //p3d::inventory->PopSection();
            
            //Drawing the characters and stuff after the shadows to attempt to 
            //eliminate the bleeding shadows.
            BEGIN_PROFILE( "Render Guts" );
            for(i = mpGuts.mUseSize-1; i>-1; i-- )
            {
                mpGuts[i]->Display();
            }
            END_PROFILE( "Render Guts" );
            
            BEGIN_PROFILE( "Render Trigger Volume Tracker" );
            GetTriggerVolumeTracker()->Render();
            END_PROFILE( "Render Trigger Volume Tracker" );
        
    #ifdef DEBUGWATCH
            BEGIN_PROFILE( "Render Vehicle AI Debug" );
            VehicleAIRender::GetVehicleAIRender()->Display();
            END_PROFILE( "Render Vehicle AI Debug" );
    #endif

		    BEGIN_PROFILE( "Lens Flare Render" );
		    LensFlareDSG::DisplayAllFlares();
		    END_PROFILE( "Lens Flare Render" );

		    BEGIN_PROFILE( "View End Render" );
		    mpView[ view ]->EndRender();
		    END_PROFILE( "View End Render" );

            if(mirrorPass == 1)
            {
                tCamera * camera = mpView[ view ]->GetCamera();
                camera->SetCameraMatrix(&originalCamera);
            }
            if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_SHOW_TREE))
            {
                rmt::Vector posn, pTriPts[3], intPosn, intNorm;
                IntersectDSG* pIntersectChunk;
                AvatarManager::GetInstance()->GetAvatarForPlayer(0)->GetPosition(posn);
                pIntersectChunk = GetIntersectManager()->FindIntersectionTri(posn,pTriPts,intPosn,intNorm);
                if(pIntersectChunk!=NULL)
                {
                    pddiCompareMode origZCompare = p3d::pddi->GetZCompare();
                    p3d::pddi->SetZCompare(PDDI_COMPARE_ALWAYS);
                        pIntersectChunk->Display();
                        pIntersectChunk->DrawTri(pTriPts, tColour(255,0,0));
                    p3d::pddi->SetZCompare(origZCompare);
                }
            }
        }
    }
#ifdef DEBUGWATCH
    mDebugRenderTime = radTimeGetMicroseconds()-mDebugRenderTime;
#endif
    END_PROFILE( "WRL Render" );
}


//////////////////////////////////////////////////////////////////////////
// Resource Interface
//////////////////////////////////////////////////////////////////////////
//========================================================================
// WorldRenderLayer::AddGuts
//========================================================================
//
// Description: Add a tDrawable
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
//void WorldRenderLayer::AddGuts( tDrawable* ipDrawable )
//{
//   // This is a currently unsupported function
//   rAssert(false);
//}

//========================================================================
// WorldRenderLayer::AddGuts
//========================================================================
//
// Description: Add a tGeometry
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
//void WorldRenderLayer::AddGuts( tGeometry* ipGeometry )
//{
//   mpWorldScene->Add( ipGeometry );
//
//}

//========================================================================
// WorldRenderLayer::AddGuts
//========================================================================
//
// Description: add an IntersectDSG
//
// Parameters:  IntersectDSG to add
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldRenderLayer::AddGuts( IntersectDSG* ipIntersectDSG )
{
   mpWorldScene->Add( ipIntersectDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mIntersectElems.Add(ipIntersectDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// RenderLayer::
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
void WorldRenderLayer::AddGuts( StaticEntityDSG* ipStaticEntityDSG )
{
   mpWorldScene->Add( ipStaticEntityDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mSEntityElems.Add(ipStaticEntityDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// RenderLayer::
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
void WorldRenderLayer::AddGuts( StaticPhysDSG* ipStaticPhysDSG )
{
   mpWorldScene->Add( ipStaticPhysDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mSPhysElems.Add(ipStaticPhysDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( FenceEntityDSG* ipFenceEntityDSG )
{
   mpWorldScene->Add( ipFenceEntityDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mFenceElems.Add(ipFenceEntityDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( AnimCollisionEntityDSG* ipAnimCollDSG )
{
   mpWorldScene->Add( ipAnimCollDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mAnimCollElems.Add(ipAnimCollDSG); 
	  // Add it to a list of managed animentitydsgs so that Update can be called on it every frame
      GetAnimEntityDSGManager()->Add( ipAnimCollDSG );
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( AnimEntityDSG* ipAnimDSG )
{
   mpWorldScene->Add( ipAnimDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mAnimElems.Add(ipAnimDSG);    
	  // Add it to a list of managed animentitydsgs so that Update can be called on it every frame
	  GetAnimEntityDSGManager()->Add( ipAnimDSG );
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( DynaPhysDSG* ipDynaPhysDSG )
{
   mpWorldScene->Add( ipDynaPhysDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mDPhysElems.Add(ipDynaPhysDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( TriggerVolume* ipTriggerVolume )
{
   mpWorldScene->Add( ipTriggerVolume );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mTrigVolElems.Add(ipTriggerVolume);      
   }
   else
   {

       rAssert(mDynaLoadState==msPreLoads);
   }
}

//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( RoadSegment* ipRoadSegment )
{
   mpWorldScene->Add( ipRoadSegment );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mRoadSegmentElems.Add(ipRoadSegment);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}


//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( PathSegment* ipPathSegment )
{
   mpWorldScene->Add( ipPathSegment );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mPathSegmentElems.Add(ipPathSegment);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}


//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( WorldSphereDSG* ipWorldSphereDSG )
{
   ipWorldSphereDSG->AddRef();
   mWorldSpheres.Add( ipWorldSphereDSG );

   if(mDynaLoadState==msLoad)
   {
      mLoadLists[mCurLoadIndex]->mWorldSphereElems.Add(ipWorldSphereDSG);      
   }
   else
   {
      rAssert(mDynaLoadState==msPreLoads);
   }
}

//=============================================================================
// WorldRenderLayer::GetCurSectionName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tName
//
//=============================================================================
tName& WorldRenderLayer::GetCurSectionName()
{
    return mLoadLists[mCurLoadIndex]->mGiveItAFuckinName;
}

//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::RemoveGuts( IEntityDSG* ipEDSG )
{
   rmt::Box3D BBox;
   BoxPts     BBoxSP;

   int i,j;
   for(i=0;i<mLoadLists.mUseSize;i++ )
   {
     //////////////////////////////////////////////////////////////////////////
     // DPhys
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mDPhysElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mDPhysElems[j])
        {
            mLoadLists[i]->mDPhysElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Intersect
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mIntersectElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mIntersectElems[j])
        {
            mLoadLists[i]->mIntersectElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // SEntity
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mSEntityElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mSEntityElems[j])
        {
            mLoadLists[i]->mSEntityElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // SPhys
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mSPhysElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mSPhysElems[j])
        {
            mLoadLists[i]->mSPhysElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // AnimColl
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mAnimCollElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mAnimCollElems[j])
        {
            mLoadLists[i]->mAnimCollElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Anim
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mAnimElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mAnimElems[j])
        {
            mLoadLists[i]->mAnimElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Fences
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mFenceElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mFenceElems[j])
        {
            mLoadLists[i]->mFenceElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Trigger Volumes
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mTrigVolElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mTrigVolElems[j])
        {
            mLoadLists[i]->mTrigVolElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Road Segments
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mRoadSegmentElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mRoadSegmentElems[j])
        {
            mLoadLists[i]->mRoadSegmentElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
     //////////////////////////////////////////////////////////////////////////
     // Path Segments
     //////////////////////////////////////////////////////////////////////////
     for(j=0;j<mLoadLists[i]->mPathSegmentElems.mUseSize;j++)
     {
        if(ipEDSG == mLoadLists[i]->mPathSegmentElems[j])
        {
            mLoadLists[i]->mPathSegmentElems.Remove(j);
            mpWorldScene->Remove(ipEDSG);
            return;
        }
     }
   }   

   //Item ipEDSG was not found in any of the dynaloadlists
   rTuneAssert(false);
}


//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::AddGuts( SpatialTree* ipSpatialTree )
{
   mpWorldScene->SetTree(ipSpatialTree);
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::ActivateWS(tUID iUID)
{
    for(int i=mWorldSpheres.mUseSize-1; i>-1; i--)
    {
        if(mWorldSpheres[i]->GetUID() == iUID)
        {
            mWorldSpheres[i]->Activate();
            return;
        }
    }
    //rAssert(false);
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::DeactivateWS(tUID iUID)
{
    for(int i=mWorldSpheres.mUseSize-1; i>-1; i--)
    {
        if(mWorldSpheres[i]->GetUID() == iUID)
        {
            mWorldSpheres[i]->Deactivate();
            return;
        }
    }
   // rAssert(false);
}
//========================================================================
// WorldRenderLayer::NullifyGuts()
//========================================================================
//
// Description: Get rid of it all! Burn. it. all. down.
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldRenderLayer::NullifyGuts()
{
   RenderLayer::NullifyGuts();
//   RenderLayer::DumpGuts();

   DumpAllDynaLoads();

   if( mpWorldScene != NULL )
   {
      delete mpWorldScene;
      mpWorldScene = NULL;
   }

   mLoadLists.Clear();
   mStaticLoadLists.Clear();
   mWorldSpheres.Clear();

   mDynaLoadState = msPreLoads;
   mCurLoadIndex  = -1;
}

//========================================================================
// WorldRenderLayer::SetUpGuts()
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
void WorldRenderLayer::SetUpGuts()
{
MEMTRACK_PUSH_GROUP( "WorldRenderLayer" );
   HeapMgr()->PushHeap (GMA_LEVEL_ZONE);

   RenderLayer::SetUpGuts();
   rAssert( mpWorldScene == NULL );
   mpWorldScene = new WorldScene;
   mpWorldScene->Init(msMaxGuts);

   mStaticLoadLists.Allocate(7);
   mLoadLists.Allocate(7);

   mLoadLists.AddUse(7);
   mStaticLoadLists.AddUse(7);

   mWorldSpheres.Allocate(10);

   mDynaLoadState = msPreLoads;
   mnLoadListRefs = 2000;
   mCurLoadIndex  = -1;

   int i;
   for(i=0;i<7;i++)
   {
      mStaticLoadLists[i].AllocateAll(mnLoadListRefs);
      mLoadLists[i] = &(mStaticLoadLists[i]);
   }

   mLoadLists.ClearUse();


   HeapMgr()->PopHeap (GMA_LEVEL_ZONE);
MEMTRACK_POP_GROUP( "WorldRenderLayer" );
}

//************************************************************************
// Load-Related Interface
//************************************************************************
void WorldRenderLayer::DoPreStaticLoad()
{
   if( !IsGutsSetup() )
      SetUpGuts();

   mExportedState = msFrozen;
}

void WorldRenderLayer::DoPostStaticLoad()
{
//   mpWorldScene->GenerateSpatialReps();
}
//========================================================================
// WorldRenderLayer::
//========================================================================
//
// Description: 
//
// Parameters:  unsigned int start.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldRenderLayer::DumpAllDynaLoads( unsigned int start, SwapArray<tRefCounted*>& irEntityDeletionList )
{
    GetEventManager()->TriggerEvent( EVENT_INTERIOR_DUMPED );

    rTuneAssert(mQdDump==false);

    if(mDynaLoadState==msLoad)
    {
        rReleasePrintf("-=-=-=-=-=-=-=-Dump Queued-=-=-=-=-=-=-=-\n");
        mQdDump = true;
        mQdDeletionStart = start;
        mpQdDeletionList = &irEntityDeletionList;
    }
    else
    {
        while( mLoadLists.mUseSize > static_cast<int>( start ) )
        {
            DumpDynaLoad(mLoadLists[start]->mGiveItAFuckinName, irEntityDeletionList );
        }
    }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::DumpDynaLoad(tName& irGiveItAFuckinName, SwapArray<tRefCounted*>& irEntityDeletionList)
{
    //TODO: this is the ugliest, most embarrasing piece of code I've ever written.
    // re-write.

   rmt::Box3D BBox;
   BoxPts     BBoxSP;
 
   int i,j,k;
   for(i=0;i<mLoadLists.mUseSize;i++ )
   {
      if( mLoadLists[i]->mGiveItAFuckinName.GetUID() == irGiveItAFuckinName.GetUID() )
      {
BEGIN_PROFILE( "Remove Searches" );

         GetEventManager()->TriggerEvent( EVENT_DUMP_DYNA_SECTION, (void*)(&(mLoadLists[i]->mGiveItAFuckinName)) );
         //////////////////////////////////////////////////////////////////////////
         // WorldSpheres
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mWorldSphereElems.mUseSize;j++)
         {
             for(k=0;k<mWorldSpheres.mUseSize;k++)
             {
                 if(mLoadLists[i]->mWorldSphereElems[j] == mWorldSpheres[k])
                 {
                     irEntityDeletionList.Add((tRefCounted*&)mWorldSpheres[k]);
                     //mWorldSpheres[k]->Release();
                     mWorldSpheres.Remove(k);
                     k = mWorldSpheres.mUseSize +10;
                 }
             }
             rAssert( k= mWorldSpheres.mUseSize+10 );
         }

         //////////////////////////////////////////////////////////////////////////
         // DPhys
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mDPhysElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mDPhysElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mDPhysElems[j]->mpSpatialNode);


            for(k=0;k<rSNode.mDPhysElems.mUseSize;k++)
            {
               if( rSNode.mDPhysElems[k] == mLoadLists[i]->mDPhysElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mDPhysElems[k]);
                  GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(rSNode.mDPhysElems[k]);
                  rSNode.mDPhysElems.Remove(k);
                  k = -1;
                  break;
               }
            }

            if(k!=-1)
            {
                rAssert(false);
                //it might be in the root node, which now doubles as overflow...
            
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mDPhysElems.mUseSize;k++)
                {
                    if( rRootNode.mDPhysElems[k] == mLoadLists[i]->mDPhysElems[j] )
                    {
                        irEntityDeletionList.Add((tRefCounted*&)rRootNode.mDPhysElems[k]);
                        GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(rRootNode.mDPhysElems[k]);
                        rRootNode.mDPhysElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
           //Cant find the dynaphys where the bbox says it is
            rAssert(k==-1);
       }
         //////////////////////////////////////////////////////////////////////////
         // SEntity
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mSEntityElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mSEntityElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mSEntityElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mSEntityElems.mUseSize;k++)
            {
               if( rSNode.mSEntityElems[k] == mLoadLists[i]->mSEntityElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mSEntityElems[k]);
                  rSNode.mSEntityElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                //it might be in the root node, which now doubles as overflow...
            
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mSEntityElems.mUseSize;k++)
                {
                    if( rRootNode.mSEntityElems[k] == mLoadLists[i]->mSEntityElems[j] )
                    {
                        irEntityDeletionList.Add((tRefCounted*&)rRootNode.mSEntityElems[k]);
                        rRootNode.mSEntityElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // AnimColl
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mAnimCollElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mAnimCollElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mAnimCollElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mAnimCollElems.mUseSize;k++)
            {
               if( rSNode.mAnimCollElems[k] == mLoadLists[i]->mAnimCollElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mAnimCollElems[k]);
                  GetAnimEntityDSGManager()->Remove( rSNode.mAnimCollElems[k] );
                  rSNode.mAnimCollElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mAnimCollElems.mUseSize;k++)
                {
                    if( rRootNode.mAnimCollElems[k] == mLoadLists[i]->mAnimCollElems[j] )
                    {
                        irEntityDeletionList.Add((tRefCounted*&)rRootNode.mAnimCollElems[k]);
                        GetAnimEntityDSGManager()->Remove( rRootNode.mAnimCollElems[k] );
                        rRootNode.mAnimCollElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // Anim
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mAnimElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mAnimElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */

            SpatialNode& rSNode = *(mLoadLists[i]->mAnimElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mAnimElems.mUseSize;k++)
            {
               if( rSNode.mAnimElems[k] == mLoadLists[i]->mAnimElems[j] )
               {
				   // Remove it from the list of managed animentities
 				   GetAnimEntityDSGManager()->Remove( rSNode.mAnimElems[k] );
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mAnimElems[k]);
                  rSNode.mAnimElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mAnimElems.mUseSize;k++)
                {
                    if( rRootNode.mAnimElems[k] == mLoadLists[i]->mAnimElems[j] )
                    {
				        // Remove it from the list of managed animentities
 				        GetAnimEntityDSGManager()->Remove( rRootNode.mAnimElems[k] );
                        irEntityDeletionList.Add((tRefCounted*&)rRootNode.mAnimElems[k]);
                        rRootNode.mAnimElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // Intersect
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mIntersectElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mIntersectElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mIntersectElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mIntersectElems.mUseSize;k++)
            {
               if( rSNode.mIntersectElems[k] == mLoadLists[i]->mIntersectElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mIntersectElems[k]);
                  rSNode.mIntersectElems.Remove(k);
                  k = rSNode.mIntersectElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mIntersectElems.mUseSize+11);
         }

         //////////////////////////////////////////////////////////////////////////
         // SPhys
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mSPhysElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mSPhysElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mSPhysElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mSPhysElems.mUseSize;k++)
            {
               if( rSNode.mSPhysElems[k] == mLoadLists[i]->mSPhysElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mSPhysElems[k]);
                  //rSNode.mSPhysElems[k]->Release();
    //              rSNode.mSPhysElems[k]->ReleaseVerified();
                  rSNode.mSPhysElems.Remove(k);
                  k = rSNode.mSPhysElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mSPhysElems.mUseSize+11);
         }
         //////////////////////////////////////////////////////////////////////////
         // Fences
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mFenceElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mFenceElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mFenceElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mFenceElems.mUseSize;k++)
            {
               if( rSNode.mFenceElems[k] == mLoadLists[i]->mFenceElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mFenceElems[k]);
                  //rSNode.mFenceElems[k]->Release();
    //              rSNode.mSPhysElems[k]->ReleaseVerified();
                  rSNode.mFenceElems.Remove(k);
                  k = rSNode.mFenceElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mFenceElems.mUseSize+11);
         }

         //////////////////////////////////////////////////////////////////////////
         // Trigger Volumes
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mTrigVolElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mTrigVolElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mTrigVolElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mTrigVolElems.mUseSize;k++)
            {
               if( rSNode.mTrigVolElems[k] == mLoadLists[i]->mTrigVolElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mTrigVolElems[k]);
                  //rSNode.mTrigVolElems[k]->Release();
                  rSNode.mTrigVolElems.Remove(k);
                  k = rSNode.mTrigVolElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mTrigVolElems.mUseSize+11);
         }
         //////////////////////////////////////////////////////////////////////////
         // Road Segments
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mRoadSegmentElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mRoadSegmentElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */

            SpatialNode& rSNode = *(mLoadLists[i]->mRoadSegmentElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mRoadSegmentElems.mUseSize;k++)
            {
               if( rSNode.mRoadSegmentElems[k] == mLoadLists[i]->mRoadSegmentElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mRoadSegmentElems[k]);
                  //rSNode.mRoadSegmentElems[k]->Release();
                  rSNode.mRoadSegmentElems.Remove(k);
                  k = rSNode.mRoadSegmentElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mRoadSegmentElems.mUseSize+11);
         }

         //////////////////////////////////////////////////////////////////////////
         // Path Segments
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mPathSegmentElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mPathSegmentElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mPathSegmentElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mPathSegmentElems.mUseSize;k++)
            {
               if( rSNode.mPathSegmentElems[k] == mLoadLists[i]->mPathSegmentElems[j] )
               {
                  irEntityDeletionList.Add((tRefCounted*&)rSNode.mPathSegmentElems[k]);
                  //rSNode.mPathSegmentElems[k]->Release();
                  rSNode.mPathSegmentElems.Remove(k);
                  k = rSNode.mPathSegmentElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mPathSegmentElems.mUseSize+11);
         }
 
END_PROFILE( "Remove Searches" );

         radTime64 start = radTimeGetMicroseconds64();

BEGIN_PROFILE( "Remove Section Elems" );
         p3d::inventory->RemoveSectionElements(irGiveItAFuckinName.GetUID());
END_PROFILE( "Remove Section Elems" );
BEGIN_PROFILE( "Delete Section" );
         p3d::inventory->DeleteSection(irGiveItAFuckinName.GetUID());
END_PROFILE( "Delete Section" );

         radTime64 end = radTimeGetMicroseconds64();
         unsigned deleteTime = (unsigned) (end - start);

         rTunePrintf("WorldRenderLayer::DumpDynaLoad Delete time: %.3fms\n", deleteTime / 1000.0F);

         HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
BEGIN_PROFILE( "Cleanup" );
		rReleasePrintf("LoadList Dump: %s Num: %d\n", mLoadLists[i]->mGiveItAFuckinName.GetText(), i );
         //mLoadLists[i]->ClearAll();
	     //mLoadLists[i]->AllocateAll(mnLoadListRefs);
         mLoadLists[i]->ClearAllUse();
         mLoadLists.Remove(i);
END_PROFILE( "Cleanup" );
         HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
      }
   }
}
//========================================================================
// WorldRenderLayer::
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
bool WorldRenderLayer::DoPreDynaLoad(tName& irGiveItAFuckinName)//tUID iUID)
{
   int i;
   for(i=0;i<mLoadLists.mUseSize;i++)
   {
      if(mLoadLists[i]->mGiveItAFuckinName.GetUID() == irGiveItAFuckinName.GetUID() )
         return false;
   }

   HeapMgr()->PushHeap (GMA_LEVEL_ZONE);

   if( mDynaLoadState == msLoad )
   {
      rReleasePrintf("ARGH! You're driving TOO FAST!*******************\n");
      rAssert(mDynaLoadState != msLoad);
   }
   else
   {
      rReleasePrintf("*******************Loading*****%s****\n", irGiveItAFuckinName.GetText());
      mLoadLists.AddUse(1);
      mCurLoadUID    = irGiveItAFuckinName.GetUID(); 
	  rReleasePrintf("CurLoadIndex was %d\n", mCurLoadIndex );
      mCurLoadIndex  = mLoadLists.mUseSize-1; 
	  rReleasePrintf("CurLoadIndex is %d\n", mCurLoadIndex );
      mDynaLoadState = msLoad;
      HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
      mLoadLists[mCurLoadIndex]->mGiveItAFuckinName= irGiveItAFuckinName;
      //mLoadLists[mCurLoadIndex]->ClearAll();
      //mLoadLists[mCurLoadIndex]->AllocateAll(mnLoadListRefs);
      mLoadLists[mCurLoadIndex]->ClearAllUse();
      HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
      BillboardWrappedLoader::OverrideLoader( false );
   }

   GetPersistentWorldManager()->OnSectorLoad( irGiveItAFuckinName.GetUID() );

   HeapMgr()->PopHeap (GMA_LEVEL_ZONE);

   return true;
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::DoPostDynaLoad()
{
   rAssert(mDynaLoadState != msNoLoad);
   mDynaLoadState = msNoLoad;
   mCurLoadUID = 0;

   if(mQdDump)
   {
      rReleasePrintf("-=-=-=-=-=-=-=-Queued Dump, Dumped-=-=-=-=-=-=-=-\n");
      mQdDump = false;
      DumpAllDynaLoads(mQdDeletionStart,*mpQdDeletionList);
      GetEventManager()->TriggerEvent( EVENT_ALL_DYNAMIC_ZONES_DUMPED, NULL ); 
   }
   BillboardWrappedLoader::OverrideLoader( true );
}

//========================================================================
// WorldRenderLayer::
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
WorldScene* WorldRenderLayer::pWorldScene()
{
   rAssert( mpWorldScene != NULL );

   return mpWorldScene;
}

//************************************************************************
//
// Protected Member Functions : WorldRenderLayer 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : WorldRenderLayer 
//
//************************************************************************
//========================================================================
// WorldRenderLayer::IsGutsSetup()
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
bool WorldRenderLayer::IsGutsSetup()
{
   return( mpGuts.IsSetUp() && (mpWorldScene != NULL) );
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::OnWorldRenderLayerInit()
{
   mpWorldScene = NULL;
}
//========================================================================
// WorldRenderLayer::
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
//========================================================================
// WorldRenderLayer::
//========================================================================
//
// Description: 
//
// Parameters:  unsigned int start.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldRenderLayer::DumpAllDynaLoads()
{
    GetEventManager()->TriggerEvent( EVENT_INTERIOR_DUMPED );

    int start = 0;
    while( mLoadLists.mUseSize > start )
    {
        DumpDynaLoad(mLoadLists[start]->mGiveItAFuckinName);
    }
}
//========================================================================
// WorldRenderLayer::
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
void WorldRenderLayer::DumpDynaLoad(tName& irGiveItAFuckinName)
{
   rmt::Box3D BBox;
   BoxPts     BBoxSP;
  
   int i,j,k;
   for(i=0;i<mLoadLists.mUseSize;i++ )
   {
      if( mLoadLists[i]->mGiveItAFuckinName.GetUID() == irGiveItAFuckinName.GetUID() )
      {
         GetEventManager()->TriggerEvent( EVENT_DUMP_DYNA_SECTION, (void*)(&(mLoadLists[i]->mGiveItAFuckinName)) );

         //////////////////////////////////////////////////////////////////////////
         // WorldSpheres
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mWorldSphereElems.mUseSize;j++)
         {
             for(k=0;k<mWorldSpheres.mUseSize;k++)
             {
                 if(mLoadLists[i]->mWorldSphereElems[j] == mWorldSpheres[k])
                 {
                     //irEntityDeletionList.Add(mWorldSpheres[k]);
                     mWorldSpheres[k]->Release();
                     mWorldSpheres.Remove(k);
                     k = mWorldSpheres.mUseSize +10;
                 }
             }
             rAssert( k= mWorldSpheres.mUseSize+10 );
         }

         //////////////////////////////////////////////////////////////////////////
         // DPhys
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mDPhysElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mDPhysElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mDPhysElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mDPhysElems.mUseSize;k++)
            {
               if( rSNode.mDPhysElems[k] == mLoadLists[i]->mDPhysElems[j] )
               {
                  GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(rSNode.mDPhysElems[k]);
                  rSNode.mDPhysElems[k]->Release();
                  rSNode.mDPhysElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mDPhysElems.mUseSize;k++)
                {
                    if( rRootNode.mDPhysElems[k] == mLoadLists[i]->mDPhysElems[j] )
                    {
                        GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(rRootNode.mDPhysElems[k]);
                        rRootNode.mDPhysElems[k]->Release();
                        rRootNode.mDPhysElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // SEntity
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mSEntityElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mSEntityElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mSEntityElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mSEntityElems.mUseSize;k++)
            {
               if( rSNode.mSEntityElems[k] == mLoadLists[i]->mSEntityElems[j] )
               {
                  rSNode.mSEntityElems[k]->Release();
                  rSNode.mSEntityElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mSEntityElems.mUseSize;k++)
                {
                    if( rRootNode.mSEntityElems[k] == mLoadLists[i]->mSEntityElems[j] )
                    {
                        rRootNode.mSEntityElems[k]->Release();
                        rRootNode.mSEntityElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // AnimColl
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mAnimCollElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mAnimCollElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
                */
            SpatialNode& rSNode = *(mLoadLists[i]->mAnimCollElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mAnimCollElems.mUseSize;k++)
            {
               if( rSNode.mAnimCollElems[k] == mLoadLists[i]->mAnimCollElems[j] )
               {
                  rSNode.mAnimCollElems[k]->Release();
                  GetAnimEntityDSGManager()->Remove( rSNode.mAnimCollElems[k] );
                  rSNode.mAnimCollElems.Remove(k);
                  k = -1;
                  break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mAnimCollElems.mUseSize;k++)
                {
                    if( rRootNode.mAnimCollElems[k] == mLoadLists[i]->mAnimCollElems[j] )
                    {
                        rRootNode.mAnimCollElems[k]->Release();
                        GetAnimEntityDSGManager()->Remove( rRootNode.mAnimCollElems[k] );
                        rRootNode.mAnimCollElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // Anim
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mAnimElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mAnimElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mAnimElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mAnimElems.mUseSize;k++)
            {
               if( rSNode.mAnimElems[k] == mLoadLists[i]->mAnimElems[j] )
               {
				   // Remove it from the list of managed animentities
 				   GetAnimEntityDSGManager()->Remove( rSNode.mAnimElems[k] );
				   rSNode.mAnimElems[k]->Release();
                   rSNode.mAnimElems.Remove(k);
                   k = -1;
                   break;
               }
            }
            if(k!=-1)
            {
                rAssert(false);
                SpatialNode& rRootNode = mpWorldScene->mStaticTreeWalker.rIthNode(0);

                for(k=0;k<rRootNode.mAnimElems.mUseSize;k++)
                {
                    if( rRootNode.mAnimElems[k] == mLoadLists[i]->mAnimElems[j] )
                    {
				        // Remove it from the list of managed animentities
 				        GetAnimEntityDSGManager()->Remove( rRootNode.mAnimElems[k] );
				        rRootNode.mAnimElems[k]->Release();
                        rRootNode.mAnimElems.Remove(k);
                        k = -1;
                        break;
                    }
                }
            }
            rAssert(k==-1);
         }
         //////////////////////////////////////////////////////////////////////////
         // Intersect
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mIntersectElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mIntersectElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mIntersectElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mIntersectElems.mUseSize;k++)
            {
               if( rSNode.mIntersectElems[k] == mLoadLists[i]->mIntersectElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mIntersectElems[k]);
                  rSNode.mIntersectElems[k]->Release();
   //               rSNode.mIntersectElems[k]->ReleaseVerified();
                  rSNode.mIntersectElems.Remove(k);
                  k = rSNode.mIntersectElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mIntersectElems.mUseSize+11);
         }
         //////////////////////////////////////////////////////////////////////////
         // SPhys
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mSPhysElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mSPhysElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mSPhysElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mSPhysElems.mUseSize;k++)
            {
               if( rSNode.mSPhysElems[k] == mLoadLists[i]->mSPhysElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mSPhysElems[k]);
                  rSNode.mSPhysElems[k]->Release();
    //              rSNode.mSPhysElems[k]->ReleaseVerified();
                  rSNode.mSPhysElems.Remove(k);
                  k = rSNode.mSPhysElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mSPhysElems.mUseSize+11);
         }
         //////////////////////////////////////////////////////////////////////////
         // Fences
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mFenceElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mFenceElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mFenceElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mFenceElems.mUseSize;k++)
            {
               if( rSNode.mFenceElems[k] == mLoadLists[i]->mFenceElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mFenceElems[k]);
                  rSNode.mFenceElems[k]->Release();
    //              rSNode.mSPhysElems[k]->ReleaseVerified();
                  rSNode.mFenceElems.Remove(k);
                  k = rSNode.mFenceElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mFenceElems.mUseSize+11);
         }

         //////////////////////////////////////////////////////////////////////////
         // Trigger Volumes
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mTrigVolElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mTrigVolElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mTrigVolElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mTrigVolElems.mUseSize;k++)
            {
               if( rSNode.mTrigVolElems[k] == mLoadLists[i]->mTrigVolElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mTrigVolElems[k]);
                  rSNode.mTrigVolElems[k]->Release();
                  rSNode.mTrigVolElems.Remove(k);
                  k = rSNode.mTrigVolElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mTrigVolElems.mUseSize+11);
         }
         //////////////////////////////////////////////////////////////////////////
         // Road Segments
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mRoadSegmentElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mRoadSegmentElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mRoadSegmentElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mRoadSegmentElems.mUseSize;k++)
            {
               if( rSNode.mRoadSegmentElems[k] == mLoadLists[i]->mRoadSegmentElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mRoadSegmentElems[k]);
                  rSNode.mRoadSegmentElems[k]->Release();
                  rSNode.mRoadSegmentElems.Remove(k);
                  k = rSNode.mRoadSegmentElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mRoadSegmentElems.mUseSize+11);
         }

         //////////////////////////////////////////////////////////////////////////
         // Path Segments
         //////////////////////////////////////////////////////////////////////////
         for(j=0;j<mLoadLists[i]->mPathSegmentElems.mUseSize;j++)
         {
             /*
            mLoadLists[i]->mPathSegmentElems[j]->GetBoundingBox(&BBox);
            BBoxSP.mBounds.mMin.Add(BBox.low, mpWorldScene->mEpsilonOffset);
            BBoxSP.mBounds.mMax.Sub(BBox.high,mpWorldScene->mEpsilonOffset);
            
            SpatialNode& rSNode = mpWorldScene->mStaticTreeWalker.rSeekNode(BBoxSP);
            */
            SpatialNode& rSNode = *(mLoadLists[i]->mPathSegmentElems[j]->mpSpatialNode);

            for(k=0;k<rSNode.mPathSegmentElems.mUseSize;k++)
            {
               if( rSNode.mPathSegmentElems[k] == mLoadLists[i]->mPathSegmentElems[j] )
               {
                  //irEntityDeletionList.Add(rSNode.mPathSegmentElems[k]);
                  rSNode.mPathSegmentElems[k]->Release();
                  rSNode.mPathSegmentElems.Remove(k);
                  k = rSNode.mPathSegmentElems.mUseSize+10;
               }
            }
            rAssert(k==rSNode.mPathSegmentElems.mUseSize+11);
         }

         radTime64 start = radTimeGetMicroseconds64();

         p3d::inventory->RemoveSectionElements(irGiveItAFuckinName.GetUID());
         p3d::inventory->DeleteSection(irGiveItAFuckinName.GetUID());

         radTime64 end = radTimeGetMicroseconds64();
         unsigned deleteTime = (unsigned) (end - start);

//         printf("WorldRenderLayer::DumpDynaLoad Delete time: %.3fms\n", deleteTime / 1000.0F);

         HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
         //mLoadLists[i]->ClearAll();
	     //mLoadLists[i]->AllocateAll(mnLoadListRefs);
         mLoadLists[i]->ClearAllUse();
         mLoadLists.Remove(i);
         HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
      }
   }
}
