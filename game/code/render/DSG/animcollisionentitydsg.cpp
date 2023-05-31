//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animcollisionentitydsg.cpp
//
// Description: Implementation of class AnimCollisionEntityDSG
//
// History:     05/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/multicontroller.hpp>

#include <simcommon/simstatearticulated.hpp>
#include <simcollision/collisionobject.hpp>

#include <poser/poseengine.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/animcollisionentitydsg.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/Culling/WorldScene.h>

#include <worldsim/redbrick/rootmatrixdriver.h>
#include <worldsim/physicsairef.h>
#include <worldsim/worldobject.h>

#include <ai/actionbuttonhandler.h>

#include <simcollision/collisiondisplay.hpp>
#include <simcommon/simutility.hpp>
#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisionobject.hpp>

#include <events/eventmanager.h>
#include <events/eventdata.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Table of objects with sounds (but no sound in CollisionAttributes)
//

struct AnimEntitySoundData
{
    radInt64 animEntityUID;
    const char* soundName;
    unsigned int jointNumber;
    const char* settingName;
};


static const AnimEntitySoundData s_animSoundTable[] =
{
    { tEntity::MakeUID( "Splatform2Trans" ), "platform_02", 1, "platform_settings" },
    { tEntity::MakeUID( "Aplatform1Trans" ), "platform_01", 1, "platform_settings" },
    { tEntity::MakeUID( "Aplatform2Trans" ), "platform_01", 1, "platform_settings" },
    { tEntity::MakeUID( "trapdoor1Trans" ), "platform_01", 1, "platform_settings" },
    { tEntity::MakeUID( "crane" ), "platform_02", 3, "crane_settings" },
    { tEntity::MakeUID( "roboarm" ), "robo_arm", 1, "platform_settings" }
};

static unsigned int s_animSoundTableSize = sizeof( s_animSoundTable ) / sizeof( AnimEntitySoundData );

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// AnimCollisionEntityDSG::AnimCollisionEntityDSG
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AnimCollisionEntityDSG::AnimCollisionEntityDSG()
:
mfDirection( 1.0f ),
mpActionButton( 0 )
{
}

//==============================================================================
// AnimCollisionEntityDSG::~AnimCollisionEntityDSG
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AnimCollisionEntityDSG::~AnimCollisionEntityDSG()
{
BEGIN_PROFILE( "AnimColl Destroy" );
    
    tRefCounted::Release( mpDrawable );
    tRefCounted::Release( mpPoseEngine );
    tRefCounted::Release( mpRootMatrixDriver );
    tRefCounted::Release( mpSimStateArticulated );
    tRefCounted::Release( mpAnimController );
    if ( mpActionButton )
    {
        mpActionButton->Destroy( );
        tRefCounted::Release( mpActionButton );
    }

    GetEventManager()->TriggerEvent( EVENT_STOP_ANIM_ENTITY_DSG_SOUND, this );

END_PROFILE( "AnimColl Destroy" );
}
/*
==============================================================================
AnimCollisionEntityDSG::Create
==============================================================================
Description:    Comment

Parameters:     ( const char* objectName, const char* animName )

Return:         bool 

=============================================================================
*/
bool AnimCollisionEntityDSG::Create( const char* objectName, const char* animName )
{
    rAssert( false );
    return false;
}
//========================================================================
// animcollisionentitydsg::
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
void AnimCollisionEntityDSG::LoadSetUp
(
    tCompositeDrawable*         ipCompD, 
    tMultiController*  ipAnimFC,
    sim::CollisionObject*       ipCollObject,
    tEntityStore*               ipStore
)
{
    mpDrawable = ipCompD;
    mpAnimController = ipAnimFC;

    rAssert( mpDrawable );
    rAssert(mpAnimController);
    rAssert( ipCollObject );
    if(mpDrawable && mpAnimController && ipCollObject )
    {
        mpDrawable->AddRef();
        mpAnimController->AddRef();
        mTransform.Identity( );
        tPose* p3dPose = mpDrawable->GetPose();
        p3dPose->Evaluate();
        mTransform = p3dPose->GetJoint( 0 )->worldMatrix;
        
        mpPoseEngine = new poser::PoseEngine( p3dPose, 1, p3dPose->GetNumJoint() );
        mpPoseEngine->AddRef();

        mpRootMatrixDriver = new RootMatrixDriver(&(mTransform));
        mpRootMatrixDriver->AddRef();   // TODO - not really doing proper cleanup on this shit
        mpPoseEngine->AddPoseDriver(0, mpRootMatrixDriver );


        // The section stuff here is a hack.
        // Tracked to ATG as bug 1259.
        //
        p3d::inventory->PushSection ();
        p3d::inventory->AddSection (SKELCACHE);
        p3d::inventory->SelectSection (SKELCACHE);
        mpSimStateArticulated = sim::SimStateArticulated::CreateSimStateArticulated( mpPoseEngine->GetPose(), ipCollObject, (sim::SimulatedObject*)0 );
        p3d::inventory->PopSection ();

        rAssert( mpSimStateArticulated );

        mpSimStateArticulated->AddRef();
        mpSimStateArticulated->mAIRefIndex = PhysicsAIRef::redBrickPhizMoveableAnim;
        mpSimStateArticulated->mAIRefPointer = (void*)this;
        // Manually set this because physics seems to be pretty confused.
        //
        // TODO: fix this, because now physics system will generate collision pairs
        // for this object with other animcollentity.
        //
        // TBJ [8/28/2002] 
        //
        mpSimStateArticulated->GetCollisionObject()->SetIsStatic( false );
        
        // This will init the pose, and set the bounding boxes.
        //
        UpdatePose( 0.0f );
    }  
    
    mpDrawable->ProcessShaders(*this);

    //
    // Now that we're set up, see if we're supposed to play a sound -- Esan
    //
    findSoundName();
}
/*
==============================================================================
AnimCollisionEntityDSG::SetAction
==============================================================================
Description:    Comment

Parameters:     ( ActionButton::AnimSwitch* pActionButton )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::SetAction( ActionButton::AnimSwitch* pActionButton )
{
    tRefCounted::Assign( mpActionButton, pActionButton );
}
//========================================================================
// animcollisionentitydsg::
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
void AnimCollisionEntityDSG::GetBoundingBox(rmt::Box3D* box)
{
    *box = mBoundingBox;
}
//========================================================================
// animcollisionentitydsg::
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
void AnimCollisionEntityDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
    *sphere = mBoundingSphere;
}

/*
==============================================================================
AnimCollisionEntityDSG::Display
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  AnimCollisionEntityDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    mpDrawable->Display();
    DSG_END_PROFILE(profileName)
}
/*
==============================================================================
AnimCollisionEntityDSG::pPosition
==============================================================================
Description:    Comment

Parameters:     ()

Return:         rmt

=============================================================================
*/
rmt::Vector* AnimCollisionEntityDSG::pPosition()
{
    return (rmt::Vector*)(mTransform.m[3]);    
}

/*
==============================================================================
AnimCollisionEntityDSG::rPosition
==============================================================================
Description:    Comment

Parameters:     ()

Return:         const 

=============================================================================
*/
const rmt::Vector& AnimCollisionEntityDSG::rPosition()
{
    return mTransform.Row( 3 ); 
}

/*
==============================================================================
AnimCollisionEntityDSG::GetPosition
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* ipPosn )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = mTransform.Row(3);
}

void AnimCollisionEntityDSG::AdvanceAnimation( float timeins )
{
    mpAnimController->Advance( timeins * 1000.0f * mfDirection );
}


/*
==============================================================================
AnimCollisionEntityDSG::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::Update( float timeins )
{
    if ( mpActionButton )
    {
        mpActionButton->Update( timeins );
    }
    // Store this before we update the transform so the scenegraph can
    // find 'this'.
    //
    rmt::Box3D oldBox;
    GetBoundingBox( &oldBox );
    
    // We only need to update when the animation is actually playing.
    //
    if ( mfDirection != 0 )
    {
        UpdatePose( timeins );
    }
    // Move the object in the scenegraph.
    //
    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
    // Sanity check
    rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
    pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   

}
/*
==============================================================================
AnimCollisionEntityDSG::UpdatePose
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::UpdatePose( float timeins )
{
    // advance the animation and update transform!
    //
    //mpAnimController->Advance( timeins * 1000.0f * mfDirection );
    this->mTransform = mpDrawable->GetPose()->GetJoint( 0 )->worldMatrix;//objectMatrix;

    mpSimStateArticulated->StoreJointState( timeins );
    mpPoseEngine->Begin(false);
    int i;

    for (i = 0; i < mpPoseEngine->GetPassCount(); i++)
    {
        mpPoseEngine->Advance(i, timeins );
        mpPoseEngine->Update(i);
    }
    mpPoseEngine->End();     // copy over what we're gonna render

    mpSimStateArticulated->UpdateJointState( timeins );

    sim::CollisionObject* pObject = mpSimStateArticulated->GetCollisionObject();
    pObject->Update();

    UpdateBBox( pObject->GetCollisionVolume( ) );
}
/*
==============================================================================
AnimCollisionEntityDSG::UpdateBBox
==============================================================================
Description:    Comment

Parameters:     ( sim::CollisionVolume* pVolume )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::UpdateBBox( sim::CollisionVolume* pVolume )
{
    // Use the BBox and BSphere from the CollisionVolume to 
    // update the drawable BBOX and BSphere.
    //
    mBoundingSphere.centre = pVolume->mPosition;
    mBoundingSphere.radius = pVolume->mSphereRadius;
    
    rmt::Vector position = pVolume->mPosition;
    rmt::Vector size = pVolume->mBoxSize;
    mBoundingBox.high.Add( position, size );
    size.Scale( -1.0f );
    mBoundingBox.low.Add( position, size );
}
/*
==============================================================================
AnimCollisionEntityDSG::UpdateVisibility
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSG::UpdateVisibility( void )
{
    mpSimStateArticulated->GetCollisionObject( )->SetVisibility( mpDrawable );
}
/*
==============================================================================
AnimCollisionEntityDSG::PreReactToCollision
==============================================================================
Description:    Comment

Parameters:     ( sim::SimState* pCollidedObj, sim::Collision& inCollision )

Return:         bool 

=============================================================================
*/
sim::Solving_Answer AnimCollisionEntityDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
    return sim::Solving_Continue;
}



//=============================================================================
// AnimCollisionEntityDSG::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer AnimCollisionEntityDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{

    // subclass-specific shit here

    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);
}




/*
==============================================================================
AnimCollisionEntityDSG::GetPoseJoint
==============================================================================
Description:    Comment

Parameters:     ( int jointIndex, bool bAttachToJoint )

Return:         tPose

=============================================================================
*/
tPose::Joint* AnimCollisionEntityDSG::GetPoseJoint( int jointIndex, bool bAttachToJoint ) 
{
    tPose::Joint* pJoint = 0;
    if ( jointIndex >= 0 )
    {
        pJoint = mpPoseEngine->GetP3DPose( )->GetJoint( jointIndex );//   GetPose( )->GetJoint( jointIndex );
    }
    // If not found, try the root.
    //
    if ( pJoint == (tPose::Joint*)0 && !bAttachToJoint )
    {
        pJoint = mpPoseEngine->GetP3DPose( )->GetJoint( 0 );// mpPoseEngine->GetPose( )->GetJoint( 0 );
    }
    rAssert( pJoint );

    return pJoint;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
void AnimCollisionEntityDSG::OnSetSimState( sim::SimState* ipCollObj )
{
}

//=============================================================================
// AnimCollisionEntityDSG::findSoundName
//=============================================================================
// Description: Platforms messed up the usual sound drill.  Because they don't
//              have entries in the art DB, we need to search a table to find
//              out whether this anim entity has a sound that the sound system
//              needs to know about.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AnimCollisionEntityDSG::findSoundName()
{
    unsigned int i;
    const char* soundName;
    tUID myName = GetUID();

    soundName = NULL;

    for( i = 0; i < s_animSoundTableSize; i++ )
    {
        if( myName == static_cast< tUID >( s_animSoundTable[i].animEntityUID ) )
        {
            soundName = s_animSoundTable[i].soundName;
            break;
        }
    }

    if( soundName != NULL )
    {
        //
        // We've got sound, tell the sound system
        //
        AnimSoundDSGData data( soundName, 
                               this, 
                               GetPoseJoint( s_animSoundTable[i].jointNumber, false ),
                               s_animSoundTable[i].settingName );
        GetEventManager()->TriggerEvent( EVENT_START_ANIM_ENTITY_DSG_SOUND, &data );
    }
}