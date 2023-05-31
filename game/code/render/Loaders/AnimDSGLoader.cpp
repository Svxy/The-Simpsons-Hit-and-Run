//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains the implementation of...
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/Loaders/AnimDSGLoader.h>
#include <render/DSG/animentitydsg.h>
#include <p3d/chunkfile.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/inventory.hpp>
#include <simcollision/collisionobject.hpp>
#include <render/Loaders/BillboardWrappedLoader.h>

#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <constants/chunks.h>
#include <memory/srrmemory.h>

#include <render/Loaders/AllWrappers.h>

#include <roads/roadmanager.h>
#include <roads/intersection.h>



//========================================================================
// AnimDSGLoader::AnimDSGLoader
//========================================================================
//
// Description: AnimDSGLoader Constructor. 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
AnimDSGLoader::AnimDSGLoader() :
tSimpleChunkHandler(SRR2::ChunkID::ANIM_DSG)
{
    mpCompDLoader = new(GMA_PERSISTENT) tCompositeDrawableLoader;
    mpCompDLoader->AddRef();

    mpMCLoader    = new(GMA_PERSISTENT) tMultiControllerLoader;
    mpMCLoader->AddRef();
    
    mpFCLoader    = new(GMA_PERSISTENT) tFrameControllerLoader;
    mpFCLoader->AddRef();

    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// AnimDSGLoader::~AnimDSGLoader
//========================================================================
//
// Description: AnimDSGLoader dtor
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
AnimDSGLoader::~AnimDSGLoader()
{
    mpCompDLoader->Release();
    mpMCLoader->Release();
	mpFCLoader->Release();

}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// AnimCollLoader::
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
tEntity* AnimDSGLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);

    int version = f->GetLong();
    int HasAlpha = f->GetLong();

    tCompositeDrawable* pCompD = NULL;
    tMultiController* pAnimMC = NULL;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case P3D_COMPOSITE_DRAWABLE:
            {
                pCompD = (tCompositeDrawable*)mpCompDLoader->LoadObject(f,store);
				if( store->TestCollision( pCompD->GetUID(), pCompD ) )
				{
					HandleCollision( pCompD );
					pCompD = NULL;
				}
				else
				{
					store->Store(pCompD);
				}
				rAssert( pCompD );
                break;
            }
			case P3D_MULTI_CONTROLLER:
				pAnimMC = static_cast<tMultiController*>(mpMCLoader->LoadObject( f, store ));
				break;

            case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
            {
                tFrameController* pFC = static_cast<tFrameController*>(mpFCLoader->LoadObject(f,store));
                pFC->AddRef();
                if(!store->TestCollision(pFC->GetUID(), pFC))
                {
				    store->Store( pFC );
                }
                pFC->Release();
                break;
            }
			case Pure3D::BillboardObject::QUAD_GROUP:
			{

				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));
                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject(f, store) );
				rAssert( pGroup != NULL );
				store->Store( pGroup );	
				BillboardWrappedLoader::OverrideLoader( false );
				break;
			}

            default:
				rAssertMsg( 0, "Unknown chunk found in AnimDSG chunk" );
                break;
        } // switch
        f->EndChunk();
    } // while

    //
    // This is a Hack. I don't like it, but it is the simplest, and least likely 
    // to break (ie, "most robust") implementation I can come up with, since it 
    // needs to be in by tomorrow morning.
    //
    AnimEntityDSG *pAnimDSG; 

    tUID darrowUID  = tName::MakeUID("darrow");
    tUID warrowUID  = tName::MakeUID("warrow");
    tUID nameUID    = tName::MakeUID(name); 
    int linkEnum = 0;
    if( nameUID==darrowUID || nameUID==warrowUID )
    {
        if( nameUID==warrowUID )
            linkEnum = 1;

        bool bHasAlpha = (HasAlpha != 0);
        pAnimDSG = LoadAnimAtIntersections(pCompD, pAnimMC, store, bHasAlpha, name, linkEnum);
    }
    else
    {
        pAnimDSG = new AnimEntityDSG;
        pAnimDSG->SetName(name);

        if(HasAlpha)
        {
            pAnimDSG->mTranslucent = true;
        }

        rmt::Vector tempPosn(0.0f,0.0f,0.0f); 
        pAnimDSG->LoadSetUp(pCompD, pAnimMC, store, tempPosn);

        mpListenerCB->OnChunkLoaded( pAnimDSG, mUserData, _id );
    }

    IEntityDSG::msDeletionsSafe=false;
    return pAnimDSG;
}

//========================================================================
// animdsgloader::
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
AnimEntityDSG* AnimDSGLoader::LoadAnimAtIntersections
( 
    tCompositeDrawable* ipCompD, 
    tMultiController* ipAnimMC, 
    tEntityStore* ipStore,
    bool iHasAlpha,
    char* ipName,
    int iLinkEnum
)
{
    rmt::Vector IntersectionPosn;
    AnimEntityDSG *pAnimDSG = NULL;
    RoadManager* pRoadManager = RoadManager::GetInstance();
    for( int i = pRoadManager->GetNumIntersectionsUsed()-1; i>-1; i--)
    {
        pAnimDSG = new AnimEntityDSG;

        pAnimDSG->SetName(ipName);

        // Only add one instance to update list, because
        // otherwise the animation get updated too many times
        pAnimDSG->SetAddToUpdateList( i == 0 );

        if(iHasAlpha)
        {
            pAnimDSG->mTranslucent = true;
        }

        pRoadManager->FindIntersection(i)->GetLocation(IntersectionPosn);

        pAnimDSG->LoadSetUp(ipCompD, ipAnimMC, ipStore, IntersectionPosn );

        pAnimDSG->SetVisibility(false);

        pRoadManager->FindIntersection(i)->LinkAnimEntity(pAnimDSG, iLinkEnum);

        mpListenerCB->OnChunkLoaded( pAnimDSG, mUserData, _id );
    }
 
    ///////////////////////////////////////////////////////////////////////////
    // Special Case for the one arrow that is used to float arround, not bound 
    // to an intersection
    //
    // Now includes 5 floaters the longer road segments
    //
    ///////////////////////////////////////////////////////////////////////////
    for(int i=11+iLinkEnum; i>0; i-=2)
    {
        pAnimDSG = new AnimEntityDSG;

        pAnimDSG->SetName(ipName);

        // Only add one instance to update list, because
        // otherwise the animation get updated too many times
        pAnimDSG->SetAddToUpdateList( false );

        if(iHasAlpha)
        {
            pAnimDSG->mTranslucent = true;
        }

        pAnimDSG->LoadSetUp(ipCompD, ipAnimMC, ipStore, IntersectionPosn );
        pAnimDSG->SetVisibility(false);
        pAnimDSG->SetTrackSeparately(i); //index into the stored array as well as whether to track separately bool

        mpListenerCB->OnChunkLoaded( pAnimDSG, mUserData, _id );
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    return pAnimDSG;
}

///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// AnimDSGLoader::SetRegdListener
//========================================================================
//
// Description: Register a new listener/caretaker, notify old listener of 
//              severed connection.
//
// Parameters:  pListenerCB: Callback to call OnChunkLoaded
//              pUserData:   Data to pass along for filtering, etc
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void AnimDSGLoader::SetRegdListener
(
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
   //
   // Follow protocol; notify old Listener, that it has been 
   // "disconnected".
   //
   if( mpListenerCB != NULL )
   {
      mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
   }
   mpListenerCB  = pListenerCB;
   mUserData     = iUserData;
}

//========================================================================
// AnimDSGLoader::ModRegdListener
//========================================================================
//
// Description: Just fuck with the current pUserData
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void AnimDSGLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "AnimDSGLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
