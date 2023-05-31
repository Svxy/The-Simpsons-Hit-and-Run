//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StatePropDSG
//
// Description: DSG object that contains a state prop
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <memory/classsizetracker.h>
#include <radtime.hpp>
#include <render/DSG/StatePropDSG.h>
#include <console/console.h>
#include <stateprop/statepropdata.hpp>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <poser/pose.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcollision/collisionobject.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <simphysics/physicsobject.hpp>
#include <worldsim/character/character.h>
#include <render/breakables/breakablesmanager.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <ai/actor/actormanager.h>
#include <ai/actor/actoranimation.h>
#include <events/eventenum.h>
#include <data/persistentworldmanager.h>
#include <constants/StatePropEnum.h>
#include <worldsim/hitnrunmanager.h>
#include <sound/soundcollisiondata.h>
#include <worldsim/avatarmanager.h>
#include <events/eventdata.h>
#include <worldsim/avatar.h>
#include <mission/gameplaymanager.h>
#include <worldsim/redbrick/rootmatrixdriver.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

// Amount to move the shadow/light pools off the ground
const float STATEPROP_SHADOW_Z_OFFSET = 1.0f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

StatePropDSG::PoseMap* StatePropDSG::sp_SharedtPoses;

// How large the pool of tPoses is
const int MAX_NUM_SHARED_TPOSES = 20;

// Determine if a given physics object is a heavy duty ArticulatedPhysicsObject
// or not. 
static bool IsFullArticulatedPhysicsObject( sim::PhysicsObject* object )
{
    bool isArtPhys;
    sim::ArticulatedPhysicsObject* artPhys = dynamic_cast< sim::ArticulatedPhysicsObject* >( object );
    if ( artPhys != NULL )
    {    
        isArtPhys = true;
    }
    else
    {
        isArtPhys = false;
    }
    return isArtPhys;
}



//===========================================================================
// Member Functions
//===========================================================================

StatePropDSG::StatePropDSG() :
mpStateProp( NULL ),
mpPhysObj( NULL ),
mpProcAnimator( 0 ),
m_ShadowElement( -1 ),
m_SimAnimJoint( -1 )
{
    CLASSTRACKER_CREATE( StatePropDSG );
    mTransform.Identity();
    mPosn = mTransform.Row(3);
    // Allocate some space for our array of shared tPoses
    if ( sp_SharedtPoses == NULL )
    {
        sp_SharedtPoses = new PoseMap;
        sp_SharedtPoses->reserve( MAX_NUM_SHARED_TPOSES );
    }
}

StatePropDSG::~StatePropDSG()
{
    CLASSTRACKER_DESTROY( StatePropDSG );
    GetAnimEntityDSGManager()->Remove( this );
    delete mpProcAnimator;
    mpProcAnimator = 0;

    // Ok, we are destroying a stateprop
  

    if ( mpStateProp != NULL )
    {
        mpStateProp->RemoveStatePropListener( this );
        mpStateProp->Release();
        mpStateProp = NULL;
    }       


    if ( mpPhysObj != NULL )
    {
        mpPhysObj->Release();
        mpPhysObj = NULL;
    }
}


void 
StatePropDSG::LoadSetup( CStatePropData* statePropData, 
                         int startState, 
                         const rmt::Matrix& transform,
                         CollisionAttributes* ipCollAttr,
                         bool isDynaLoaded,
                         tEntityStore* ipSearchStore,
                         bool useSharedtPose,
                         sim::CollisionObject* collisionObject,
                         sim::PhysicsObject* physicsObject )

{
    int type = ipCollAttr->GetClasstypeid();
    mTransform = transform;
    mPosn = transform.Row(3);
    // Very likely that mTranslucent will always be true considering all the particles
    // animations, fades, etc that are possible.
    mTranslucent = true;
    m_IsDynaLoaded = isDynaLoaded;
    rAssert( mpStateProp == NULL );
    // Memory optimization. If we are told to use a shared tPose, then
    // try and grab an existing one (for this type) off out map
    bool newSharedtPoseCreated;
    tPose* tpose;
    if ( useSharedtPose )
    {
        tpose = GetSharedtPose( statePropData->GetUID() );
        if ( tpose == NULL )
            // we want to share the pose, but not premade pose exists, so
            // grab the pose that will be created in CreateStateProp
            newSharedtPoseCreated = true;
        else
            newSharedtPoseCreated = false;
    }
    else
    {
        newSharedtPoseCreated = false;
        tpose = NULL;          
    }
    mpStateProp = CStateProp::CreateStateProp( statePropData, startState, tpose );
    mpStateProp->SetUID( statePropData->GetUID() );
    mpStateProp->AddStatePropListener( this );

    // A new shared tPose was created, grab it off the composite drawable and insert it
    // into our list
    if ( newSharedtPoseCreated )
    {
        tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable());
        AddNewSharedtPose( statePropData->GetUID(), compDraw->GetPose() );
    }



    // Give the DSG object name the same as the stateprop object by default
    SetName( mpStateProp->GetName() );

	// Setup the physics, retrieve the pose
    rAssert( dynamic_cast< tCompositeDrawable* > ( mpStateProp->GetDrawable()) );
    tCompositeDrawable* pCompDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() );
    tPose* p3dPose = pCompDraw->GetPose();
    p3dPose->Evaluate();


    // The sim library does in fact add something to the store, the above comment notwithstanding.
    // So we wrap this call in a PushSection/PopSection.
    //
    if ( physicsObject == NULL )
        physicsObject = p3d::find< sim::PhysicsObject >( statePropData->GetUID() );

    if ( collisionObject == NULL )
        collisionObject = p3d::find< sim::CollisionObject >( statePropData->GetUID() );
    
    // Abort if there are no physics or collision objects found
    if ( collisionObject == NULL )
        return;

    p3d::inventory->PushSection( );
    p3d::inventory->SelectSection( "Default" );

    sim::SimState* pSimState = NULL;
    if ( type == PROP_MOVEABLE || type == PROP_ONETIME_MOVEABLE || type == PROP_BREAKABLE )
    {
              
        if ( IsFullArticulatedPhysicsObject( physicsObject ) )
        {
            poser::PoseEngine* poseEngine = new poser::PoseEngine(p3dPose , 1, p3dPose->GetNumJoint() );
            poseEngine->AddRef();
            RootMatrixDriver* rmd = new RootMatrixDriver( &mTransform );
            rmd->AddRef();
            poseEngine->AddPoseDriver( 0, rmd );    
            poseEngine->Begin();

            poser::Pose* poserPose = poseEngine->GetPose();

            // This is they expensive route, heavy duty articulated physics.
            sim::SimStateArticulated* pSimStateArt = sim::SimStateArticulated::CreateSimStateArticulated( statePropData->GetUID(), poserPose, sim::SimStateAttribute_Default , ipSearchStore );
            if ( pSimStateArt != NULL )
            {
                // Simplify calculations, we don't need or want expensive articulated computations
                pSimStateArt->ConvertToRigidBody();
                // Kill the poser::Pose object. Its *TONS* of useless matrices and garbage
                pSimStateArt->ReleasePose();
                pSimState = pSimStateArt;
            }
            else
            {
                // Fallback path
                pSimState = sim::SimState::CreateSimState( statePropData->GetUID(), sim::SimStateAttribute_Default , ipSearchStore );
            }

            // Cleanup unused Articulated poseengine stuff
            poseEngine->End();
            // With no pose, the poseengine should be killed
            poseEngine->ReleaseVerified();
            poseEngine = NULL;
            // No use for the root matrix driver either
            rmd->ReleaseVerified();
            rmd = NULL;

        }
        else
        {
            rAssert( physicsObject != NULL );
            // Try creating a simplified sim state (Ideal by far!)
            pSimState = sim::SimState::CreateSimState( statePropData->GetUID(), sim::SimStateAttribute_Default , ipSearchStore );
        }

        // Place the collision object in the correct place
        pSimState->SetTransform( transform ); 
    }
    else
    {
        pSimState = sim::SimStateArticulated::CreateStaticSimState( collisionObject );
        // Tell the simstate where to place it in the world
        pSimState->GetCollisionObject()->SetIsStatic(false);
        pSimState->GetCollisionObject()->SetManualUpdate(false);
        pSimState->SetTransform( transform ); 
        pSimState->GetCollisionObject()->Update();  
        pSimState->GetCollisionObject()->GetCollisionVolume()->UpdateAll();  
        pSimState->GetCollisionObject()->SetIsStatic(true);
        pSimState->GetCollisionObject()->SetManualUpdate(true);
    }
    rAssert( pSimState != NULL );
    pSimState->SetControl( sim::simAICtrl );

    // Lets hold onto the physics object, we'll need it for physics updates 
    // when in sim mode
    mpPhysObj = static_cast< sim::PhysicsObject* >( pSimState->GetSimulatedObject() );
    if ( mpPhysObj )
        mpPhysObj->AddRef();

    p3d::inventory->PopSection();

    // Needed for SetSimState to work because of tRefCounted::Assign
    pSimState->AddRef();
    // The simstate holds the ai ref pointer of its parent object
    // tell it what type of object this is
	pSimState->mAIRefIndex = GetAIRef();
    SetCollisionAttributes(ipCollAttr);
    pSimState->mAIRefPointer = this;
    // This sets the mpSimStateObj pointer and calls OnSetsimstate
    SetSimState( pSimState );
    pCompDraw->ProcessShaders(*this);
    pSimState->Release();

    // Add this object to the animation update list that gets called every frame
    GetAnimEntityDSGManager()->Add( this );

}

StatePropDSG* 
StatePropDSG::Clone(const char* Name, const rmt::Matrix& iMatrix) const
{
    StatePropDSG* pClone = new StatePropDSG();
   
    CStatePropData* spData = mpStateProp->GetStatePropData();
    const int START_STATE = 0;
    CollisionAttributes* pCollAttr = GetCollisionAttributes();
    bool usingSharedPose;

    // Should we share the pose? 
    // That depends on whether the original is sharing a pose
    if ( GetSharedtPose( spData->GetUID() ) != NULL )
        usingSharedPose = true;
    else
        usingSharedPose = false;

    pClone->LoadSetup( spData, START_STATE, iMatrix, pCollAttr, true, NULL, usingSharedPose, NULL, NULL );
    pClone->SetName( Name );

    return pClone;
}

void StatePropDSG::OnSetSimState( sim::SimState* ipSimState )
{
    tRefCounted::Assign( mpSimStateObj, ipSimState );
    mpSimStateObj->mAIRefIndex = this->GetAIRef();
    mpSimStateObj->ResetVelocities();
        
    // set up box and sphere for DSG
    //
    // note, unlike the StaticPhysDSG, this box and sphere are in object space
    rmt::Vector center;
    if ( mpPhysObj != NULL )
    {
        center = mpPhysObj->GetExternalCMOffset();
    }
    else
    {
        center = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mPosition;
        center -= mPosn;
    }
    float radius = mpSimStateObj->GetSphereRadius();

    mBBox.low   = center;
    mBBox.high  = center;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre = center;
    mSphere.radius = radius;

    rAssert(mpCollisionAttributes);
    mpSimStateObj->SetPhysicsProperties(mpCollisionAttributes->GetPhysicsProperties());
}

void
StatePropDSG::Display()
{

#ifdef PROFILER_ENABLED
    char profileName[] = "  StatePropDSG Display";
#endif
    if(IS_DRAW_LONG) return;
    DSG_BEGIN_PROFILE(profileName)
    if(CastsShadow())
    {
        BillboardQuadManager::Enable();
    }
    p3d::stack->PushMultiply( mTransform );
    mpStateProp->Display( mpProcAnimator );
    p3d::stack->Pop();
    if(CastsShadow())
    {
        BillboardQuadManager::Disable();
        DisplaySimpleShadow();
    }
    DSG_END_PROFILE(profileName)
}

void 
StatePropDSG::AdvanceAnimation( float timeInMS )
{
    // Simple animation frame controller advancement
    mpStateProp->Update( timeInMS );
    if( mpProcAnimator )
    {
        mpProcAnimator->Advance( timeInMS );
    }
    if ( m_SimAnimJoint != -1 )
    {
        AnimateCollisionVolume();
    }
}


void
StatePropDSG::Update( float timeInSec )
{
    // Update physics and collision, if necessary
    if ( mpPhysObj != NULL )
    {
        mpPhysObj->Update( timeInSec );
    }

    if ( mpSimStateObj )
    {
        sim::CollisionObject* collObj = mpSimStateObj->GetCollisionObject();
        if ( collObj->HasMoved( ) || collObj->HasRelocated( ) )
        {
            collObj->Update();
          
            // do this _before_ updating matrix!
            rmt::Box3D oldBox;
        
            GetBoundingBox( &oldBox );

            mTransform = mpSimStateObj->GetTransform();
            mPosn = mpSimStateObj->GetPosition();

            WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
            // Sanity check
            rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
            pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   
        }    

    }

    if(mIsHit && (GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE))
    {
        if(!GetGameplayManager()->TestPosInFrustrumOfPlayer( mPosn, 0, mSphere.radius))
        {
            this->AddRef();
            ((WorldRenderLayer*)GetRenderManager()->mpLayer(RenderEnums::LevelSlot))->RemoveGuts(this);
            GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(this);
            GetRenderManager()->mEntityDeletionList.Add(this);
        }
    }
}



void 
StatePropDSG::GetBoundingBox( rmt::Box3D* box )
{
     
    (box->low).Add(mBBox.low, mPosn);
    (box->high).Add(mBBox.high, mPosn);
}
void 
StatePropDSG::GetBoundingSphere( rmt::Sphere* out_sphere )
{
    (out_sphere->centre).Add(mSphere.centre, mPosn);

    out_sphere->radius = mSphere.radius;
}

rmt::Vector*
StatePropDSG::pPosition()
{ 
    rAssert( false ); 
    return &mTransform.Row(3);
}


const rmt::Vector& 
StatePropDSG::rPosition()
{
    return mTransform.Row(3);
}
 
void 
StatePropDSG::GetPosition( rmt::Vector* ipPosn )
{
    rAssert( ipPosn != NULL );
    *ipPosn = mTransform.Row(3);
}
        
void 
StatePropDSG::SetPosition( const rmt::Vector& position )
{
    rmt::Matrix newTransform = mTransform;
    newTransform.FillTranslate( position );
    SetTransform( newTransform );
}

void
StatePropDSG::GetTransform( rmt::Matrix* pTransform )
{
    rAssert( pTransform != NULL );
    *pTransform = mTransform;
}

void 
StatePropDSG::SetTransform( const rmt::Matrix& transform )
{


	rmt::Box3D oldBox;
	GetBoundingBox( &oldBox );

    // Remember old manual update settings
    bool manualUpdate = mpSimStateObj->GetCollisionObject()->IsManualUpdate();
    mpSimStateObj->GetCollisionObject()->SetManualUpdate( false );

    if ( mpSimStateObj->GetControl() == sim::simAICtrl )
    {
        mpSimStateObj->SetTransform( transform );
    }
    else
    {
        // Can't just tell an object to move to a new position
        // if we are in sim control
        // force it to ai, set transform, then set it back
        mpSimStateObj->SetControl( sim::simAICtrl );
        mpSimStateObj->SetTransform( transform );
        mpSimStateObj->SetControl( sim::simSimulationCtrl );
    }
    mpSimStateObj->ResetVelocities();

    // Matrix is reset, recompute the bounding box and move it
	// in the DSG( Warning, this assumes that the user actually has it IN
	// the DSG. This was always the case for DSG objects so lets not break 
	// tradition
	// Bounding box relies on mPosn, lets update it 

    mTransform = mpSimStateObj->GetTransform(-1);
    mPosn = mTransform.Row( 3 );


    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
    // Sanity check

    rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
    pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   

    // Restore manual update settings
    mpSimStateObj->GetCollisionObject()->SetManualUpdate( manualUpdate );
}


sim::Solving_Answer 
StatePropDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{  
    // Make sure the object doesn't collide with other objects that it may be touching that are
    // part of the static world
    if(this->GetSimState()->GetControl() == sim::simAICtrl && pCollidedObj->GetCollisionObject()->IsStatic() == true)
    {
        return sim::Solving_Aborted;
    }        

    switch ( pCollidedObj->mAIRefIndex )
    {
    case PhysicsAIRef::PlayerCharacter:
    case PhysicsAIRef::NPCharacter:
        {
            rAssert( dynamic_cast< Character* >( static_cast< tRefCounted* >( pCollidedObj->mAIRefPointer) ) );
            Character* pCharacter = static_cast< Character* >( pCollidedObj->mAIRefPointer );
		    return sim::Solving_Aborted;
        }
    case PhysicsAIRef::redBrickVehicle:
        {
            // Collision with a vehicle
            // is this prop a breakable? 
            if ( GetType() == PROP_BREAKABLE )
            {
                Vehicle* vehicle = static_cast< Vehicle* >( pCollidedObj->mAIRefPointer );
          
                if ( HandleEvent( VEHICLE_HIT ) == false )
                {
                    // goto the destroyed state
                    HandleEvent( DESTROYED );
                    HandleEvent( STOMP );
                    HandleEvent( HIT );
                }

                //
                // Cue the breaking sounds
                //
                SoundCollisionData soundData( 1.0f, vehicle, this );

                GetEventManager()->TriggerEvent( EVENT_COLLISION, &soundData );

                // return solving aborted, the breakable isn't stopping this vehicle
                return sim::Solving_Aborted;
            }
        }
        break;
    }   

    HandleEvent( StatePropDSG::FEATHER_TOUCH );
    
    // need this here?
    this->mIsHit = true;

    // Breakables always stay put and animate in place
    if ( GetType() != PROP_BREAKABLE )
    	AddToSimulation( );
    return sim::Solving_Continue;
}
        

sim::Solving_Answer 
StatePropDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
    // Calculate impulse
    float impulseMagSqr = impulse.MagnitudeSqr();
  
    if ( impulseMagSqr > 10000 )
    {
        sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
        sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
        
        sim::SimState* simStateA = collObjA->GetSimState();
        sim::SimState* simStateB = collObjB->GetSimState();

        // Has the stateprop been hit by a vehicle?
        // If so, notify the stateprop
        if ( simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle || 
             simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle )
        {
            bool msgHandled = HandleEvent( VEHICLE_HIT );
            if ( msgHandled == false )
                HandleEvent( HIT );
        }
        else
        {
            // Tell the stateprop that a normal event was triggered
            HandleEvent( HIT );
        }
    }


    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);
}

void 
StatePropDSG::GenerateCoins( int numCoins )
{
    HitnRunManager* hrm = GetHitnRunManager();

	// This is just so bad.
	//Since an object has no idea what it really is, we can't tell if the
	//breakable object is a crate, tree, or krusty glass, etc. So to try
	//and pick out the crates we'll look at the sound file played when the
	//object breaks and if it's car_hit_crate, we'll ASSUME that it's a
	//crate. *sigh*
	bool isCrate = false;
	bool coinsDisabled = hrm->IsHitnRunDisabled();
	CollisionAttributes* collAttribs = this->GetCollisionAttributes();
	if(collAttribs)
	{
		if((strcmp(collAttribs->GetSound(), "car_hit_crate")) == 0 && (mPersistentObjectID >= 0))
		{
			isCrate = true;
		}
	}

    if(!coinsDisabled || isCrate)
    {
		GetCoinManager()->SpawnCoins( numCoins + (coinsDisabled && isCrate ? 1 : 0), rPosition() );
    }
}



void 
StatePropDSG::RecieveEvent( int callback , CStateProp* stateProp )
{
    // The artists are trying to tell us something!
    // These are callbacks set from the stateprop builder tool
    // switch on the callback type and perform the appropriate action
    
    switch ( callback )
    {
    case StatePropEnum::eStateChange:
        break;

    case StatePropEnum::eRemoveFromWorld:
        GetBreakablesManager()->RemoveBrokenObjectFromWorld( this, RenderEnums::LevelSlot, m_IsDynaLoaded );
        GetPersistentWorldManager()->OnObjectBreak( mPersistentObjectID );
        break;
    case StatePropEnum::eSpawn1Coin:
        GenerateCoins( 1 );
        break;
    case StatePropEnum::eSpawn5Coins:
        GenerateCoins( 5 );
        break;
    case StatePropEnum::eSpawn10Coins:
        GenerateCoins( 10 );
        break;
    case StatePropEnum::eSpawn15Coins:
        GenerateCoins( 15 );
        break;
    case StatePropEnum::eSpawn20Coins:
        GenerateCoins( 20 );
        break;
    case StatePropEnum::eRemoveCollisionVolume:
        {
            EnableCollisionVolume( false );
            GetPersistentWorldManager()->OnObjectBreak( mPersistentObjectID );
        }
        break;
    case StatePropEnum::eRemoveFirstCollisionVolume:
        {
            RemoveSubCollisionVolume( 0 );
        }
        break;
    case StatePropEnum::eRemoveSecondCollisionVolume:
        {
            RemoveSubCollisionVolume( 1 );
        }
        break;
    case StatePropEnum::eRemoveThirdCollisionVolume:
        {
            RemoveSubCollisionVolume( 2 );
        }
        break;
    case StatePropEnum::eKillSpeed:
        if ( mpSimStateObj != NULL )
        {
             mpSimStateObj->ResetVelocities();                     
             mpSimStateObj->SetControl( sim::simAICtrl );
        } 
        break;
    case StatePropEnum::eRadiateForce:
        {
            const float FORCE_RADIUS = 1.5f;
            ReserveArray< DynaPhysDSG* > dynamicsList( 200 );
            // Alpha hack to make the head (jeb's statute) go into SIM but nothing else. 
            // Benches have bad volumes so they look stupid when they go into sim control
            rmt::Vector forcePosition( 135.16f, 13.1979f, 37.911f );
            GetIntersectManager()->FindDynaPhysElems( forcePosition, FORCE_RADIUS, dynamicsList );
            for ( int i = 0 ; i < dynamicsList.mUseSize ; i++ )
            {
                if ( dynamicsList[i]->GetAIRef() != PhysicsAIRef::PlayerCharacter )
                {
                    dynamicsList[i]->ApplyForce( rmt::Vector(0,1,0), 200.0f );
                }
            }
        }
        break;
    case StatePropEnum::eEmitLeaves:
        {

        }
        break;
    case StatePropEnum::eSpawn5CoinsZ:
        {
            int numCoins = 5;
            rmt::Vector coinDirection = -mTransform.Row(2);
            GetCoinManager()->SpawnCoins( numCoins, rPosition(), rPosition().y, &coinDirection );
        }
        break;
    case StatePropEnum::eColaDestroyed:
        // A cola object (crate/vending machine) was destroyed. For some reason, people actually
        // care about this. (Hit and Run meter I think) fire off an event
        // signalling this
        GetEventManager()->TriggerEvent( EVENT_COLAPROP_DESTROYED, this );
        break;



    default:
        break;
    };
}

void
StatePropDSG::AddToSimulation()
{
    // Only valid if this object is flagged as moveable
    if ( GetType() == PROP_MOVEABLE || GetType() == PROP_ONETIME_MOVEABLE)
    {
        if ( mpSimStateObj->GetControl() == sim::simAICtrl )
        {
    	    mpSimStateObj->SetControl(sim::simSimulationCtrl);
            if ( mGroundPlaneIndex != -1 ) 
            {
                //mGroundPlaneIndex = FetchGroundPlane();
                GetWorldPhysicsManager()->EnableGroundPlaneCollision(mGroundPlaneIndex);
            }
   	        //GetWorldPhysicsManager()->EnableGroundPlaneCollision(mGroundPlaneIndex);
            GetEventManager()->TriggerEvent( EVENT_STATEPROP_ADDED_TO_SIM, this );
        }
    }
}

void
StatePropDSG::ApplyForce( const rmt::Vector& direction, float force )
{
    if ( force >= 800.0f )
    {
        HandleEvent( StatePropDSG::STOMP );
    }

    if ( force > 400.0f )
    {
        HandleEvent( StatePropDSG::DESTROYED );
        HandleEvent( StatePropDSG::HIT );      
    }
    else
    {
        HandleEvent( StatePropDSG::HIT );      
    }
    if ( GetType() != PROP_BREAKABLE )
    {
        DynaPhysDSG::ApplyForce( direction, force );
    }
}

int 
StatePropDSG::CastsShadow()
{
    // StatepropDSGs can use a joint as a shadow
    // if the m_ShadowElement variable is not -1, then 
    // we want to use the joint as a shadow
    int retVal;
    if ( m_ShadowElement == -1 )
        retVal = 0;
    else
        retVal = 1;
    
    return retVal;
}

void 
StatePropDSG::DisplaySimpleShadow()
{
    BEGIN_PROFILE("DisplaySimpleShadow")
    p3d::pddi->SetZWrite(false);
    if ( mpShadowMatrix != NULL )
    {
        rAssert( m_ShadowElement != -1 );    
        // Activate the joint's visibility
        // DONT TOUCH THE LOCK SETTINGS, the stateprop will lock visibility
        // off. Don't override the artists
        tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() );
        tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement( m_ShadowElement );

        element->SetVisibility( true );    

	    // the camera
	    rmt::Vector camPos;
	    p3d::context->GetView()->GetCamera()->GetWorldPosition( &camPos );
	    camPos.Sub( mpShadowMatrix->Row(3) );
	    camPos.Normalize();
	    rmt::Matrix toCamera;
	    toCamera.Identity();
	    toCamera.FillTranslate( camPos * STATEPROP_SHADOW_Z_OFFSET );
    	
	    // Final shadow transform = position/orientation * tocamera translation
	    rmt::Matrix shadowTransform( *mpShadowMatrix );
	    shadowTransform.Mult( toCamera );

	    // Display
	    p3d::stack->PushMultiply( shadowTransform );
        element->Display();
        p3d::stack->Pop();
        element->SetVisibility( false );
    }
    else
    {
        tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() );
        tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement( m_ShadowElement );
        compDraw->GetPose()->SetPoseReady( false );
        compDraw->GetPose()->Evaluate( &mTransform );
        const rmt::Matrix* worldMatrix = element->GetWorldMatrix();
        mpShadowMatrix = CreateShadowMatrix( worldMatrix->Row(3) );
        compDraw->GetPose()->SetPoseReady( false );
        if ( mpShadowMatrix == NULL )
        {

        }

    }
    p3d::pddi->SetZWrite(true);
    END_PROFILE("DisplaySimpleShadow")
}
        
void 
StatePropDSG::SetRank(rmt::Vector& irRefPosn, rmt::Vector& irRefVect)
{
    if ( CastsShadow() )
    {
        mRank = FLT_MAX;
    }
    else
    {
        IEntityDSG::SetRank( irRefPosn, irRefVect );
    }
}

void 
StatePropDSG::SetShadowElement( tUID elementName )
{
    // Lets use the given joint as a shadow / light pool!
    tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() );
   
    m_ShadowElement = compDraw->FindNodeIndex( elementName );
    rAssert( m_ShadowElement != -1 );
    tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement( m_ShadowElement );
    
    element->SetPose( compDraw->GetPose() );
    compDraw->GetPose()->SetPoseReady( false );
    compDraw->GetPose()->Evaluate( &mTransform );
    // Each drawable elements pose is set upon call to tCompositeDrawable::
    const rmt::Matrix* worldMatrix = element->GetWorldMatrix();
    rAssert( mpShadowMatrix == NULL );
    mpShadowMatrix = CreateShadowMatrix( worldMatrix->Row(3) );

    // set visibility to false so it won't be drawn as part of the regular pass
    // Get the drawable element
    element->SetVisibility( false );
    compDraw->GetPose()->SetPoseReady( false );
}


// Turn collisions on and off. Pretty useful if you want to 
// attach stateprops to vehicles as collectibles
// or when they get destroyed
void 
StatePropDSG::EnableCollisionVolume( bool enable )
{
    sim::SimState* pSimState = GetSimState();
    if ( pSimState == NULL )
        return;

    sim::CollisionObject* pCollisionObject = pSimState->GetCollisionObject();
    if ( pCollisionObject == NULL )
        return;

    pCollisionObject->SetCollisionEnabled( enable );
}

void 
StatePropDSG::RemoveSubCollisionVolume( int volumeIndex )
{
    sim::SimState* pSimState = GetSimState();
    rTuneAssert( pSimState != NULL );
    
    sim::CollisionObject* pCollisionObject = pSimState->GetCollisionObject();
    rTuneAssert( pCollisionObject != NULL );

    // Find the collision volume, then grab its subcollisionvolume
    sim::CollisionVolume* primaryVolume = pCollisionObject->GetCollisionVolume();
    
    sim::TList<sim::CollisionVolume*>* pSubVolumeList = primaryVolume->SubVolumeList();
    rTuneAssert( volumeIndex < pSubVolumeList->GetSize() && volumeIndex >= 0 );
    if ( volumeIndex >= 0 && volumeIndex < pSubVolumeList->GetSize() )
    {
        sim::CollisionVolume* subVolume = pSubVolumeList->GetAt( volumeIndex );
        rTuneAssert( subVolume != NULL );
        if ( subVolume != NULL )
        {
            // Kill the subvolume
            primaryVolume->RemoveSubVolume( subVolume );
        }
    }
}

int 
StatePropDSG::GetAIRef()
{
    return PhysicsAIRef::StateProp;
}

   
// Force an animation state change
void 
StatePropDSG::SetState( int state )
{
    mpStateProp->SetState( state );
}

// Lets send this thing an event and hope that the artists
// have everything set up to handle it
bool 
StatePropDSG::HandleEvent( Event eventID )
{
    return mpStateProp->OnEvent( eventID );
}

unsigned int
StatePropDSG::GetState()const
{
    return mpStateProp->GetState();
}

// Used if other people are interested in the messages generated from the
// stateprop subsystem
void 
StatePropDSG::AddStatePropListener( CStatePropListener* statePropListener )
{
    if ( statePropListener != NULL && mpStateProp != NULL )
    {
        mpStateProp->AddStatePropListener( statePropListener );   
    }
}

void 
StatePropDSG::RemoveStatePropListener( CStatePropListener* statePropListener )
{
    if ( statePropListener != NULL && mpStateProp != NULL )
    {
        mpStateProp->RemoveStatePropListener( statePropListener );     
    }
}
       
// Sets which joint the sim state should animate upon
void 
StatePropDSG::SetSimJoint( int jointId )
{
    if ( GetType() != PROP_STATIC ) 
        m_SimAnimJoint = jointId;
   
}

int 
StatePropDSG::GetType()const
{
    return GetCollisionAttributes()->GetClasstypeid();
}

// Turn on/off visibility
void 
StatePropDSG::EnableVisibility( bool enable )
{
    // Iterate through the composite drawable and set visiblity on each one to
    // the input
    tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() );
    rAssert( compDraw != NULL );
    for ( int i = 0 ; i < compDraw->GetNumDrawableElement() ; i++ )
    {
        tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement(i);       
        if ( element )
            element->SetVisibility( enable );
    }
}

        
void 
StatePropDSG::RemoveAllSharedtPoses()
{

    if ( sp_SharedtPoses )
    {
        PoseMapIt it;
        // We addrefed each tPose when we put them onto the array, now release them
        for ( it = sp_SharedtPoses->begin() ; it != sp_SharedtPoses->end() ; it++ )
        {
            it->second->Release();
        }
        delete sp_SharedtPoses;
        sp_SharedtPoses = NULL;
    }
}

// Uses m_SimJoint to animate the collision volume
void 
StatePropDSG::AnimateCollisionVolume()
{
    rAssert( m_SimAnimJoint != -1 );
    // We want to call SimState::SetTransform with the world space transform
    // of the evaluated joint
    // So first evaluate the composite drawables pose
    tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mpStateProp->GetDrawable() ); 
    rAssert( compDraw != NULL );
    tPose* pose = compDraw->GetPose();
    pose->Evaluate();
    tPose::Joint* joint = pose->GetJoint( m_SimAnimJoint );
    // Now grab the world matrix and apply it to the simstate
    rmt::Matrix simTransform = mTransform;
    simTransform.Row(3) += joint->worldMatrix.Row(3);

    sim::SimState* simState = GetSimState();

    // Before setting the transform, make sure to set manual updates to true
    bool origManualUpdate = simState->GetCollisionObject()->IsManualUpdate();
    simState->GetCollisionObject()->SetManualUpdate( false );

    simState->SetTransform( simTransform );  
    simState->GetCollisionObject()->SetManualUpdate( origManualUpdate );
    
    // Call reset velocities.
    // Other the sim state is going to get a velocity based upon the distance I just moved it
    simState->ResetVelocities();
}


tPose* 
StatePropDSG::GetSharedtPose( tUID type )
{
    tPose* pose;
    PoseMapIt it = sp_SharedtPoses->find( type );
    if ( it != sp_SharedtPoses->end() )
    {
        pose = it->second;
    }
    else
    {
        pose = NULL;
    }
    return pose;
}

// Add the given shared pose to the shared pose list
void 
StatePropDSG::AddNewSharedtPose( tUID type, tPose* pose )
{
    if(sp_SharedtPoses->find(type) == sp_SharedtPoses->end())
    {
        pose->AddRef();
        sp_SharedtPoses->insert( type, pose );
    }
    else
    {
        rAssert(0);
    }
}
