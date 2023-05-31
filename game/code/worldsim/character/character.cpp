#include <worldsim/character/character.h>

#include <choreo/animation.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/matrixstack.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcollision/collisionvolume.hpp>

#include <worldsim/character/charactercontroller.h>
#include <worldsim/character/charactermappable.h>
#include <worldsim/character/aicharactercontroller.h>
#include <worldsim/character/characterrenderable.h>
#include <worldsim/character/charactertarget.h>
#include <worldsim/character/charactermanager.h>

#include <worldsim/coins/sparkle.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <roads/geometry.h>
#include <memory/srrmemory.h>
#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/action.h>
#include <ai/statemanager.h>
#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/RenderManager/RenderManager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <worldsim/character/footprint/footprintmanager.h>
#include <worldsim/harass/chasemanager.h>
#include <ai/actor/intersectionlist.h>

#include <render/DSG/StatePropDSG.h>
#include <render/DSG/CollisionEntityDSG.h>

#include <events/eventenum.h>
#include <events/eventmanager.h>

#include <render/IntersectManager/IntersectManager.h>
 
#include <camera/supercammanager.h>

#include <mission/gameplaymanager.h>

#include <meta/locator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>
#include <meta/eventlocator.h>

#include <main/game.h>

#include <sound/soundmanager.h>

#include <interiors/interiormanager.h>

// Sim includes.
//
#include <simcommon/simstate.hpp>
#include <simcommon/physicsproperties.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisionmanager.hpp>
#include <simcollision/proximitydetection.hpp>
#include <simcommon/simmath.hpp>

#include <render/particles/particlemanager.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/Culling/WorldScene.h>
#include <render/DSG/StaticPhysDSG.h>

//#ifdef RAD_DEBUG
    #define DRAW_CHARACTER_COLLISION
//#endif // RAD_DEBUG

#ifdef DRAW_CHARACTER_COLLISION
    #include <main/commandlineoptions.h>
    #include <simcollision/collisiondisplay.hpp>
    #include <simcommon/simutility.hpp>
    #include <simcollision/collisionvolume.hpp>
    #include <simcollision/collisionobject.hpp>
#endif //DRAW_CHARACTER_COLLISION

// NPC includes
//
#include <input/controller.h>
#include <input/inputmanager.h>
#include <worldsim/vehiclecentral.h>

#include <debug/profiler.h>

#include <presentation/presentation.h>
#include <presentation/presentationanimator.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#ifdef RAD_WIN32
#include <camera/pccam.h>
#include <input/usercontrollerwin32.h>
#include <input/mouse.h>
#include <input/realcontroller.h>
#include <camera/supercam.h>
#include <camera/supercamcontroller.h>
#endif

const static rmt::Vector vUp( 0.0f, 1.0f, 0.0f );

const float KICK_ANGLE = 45;

static int s_IntersectFrame;

class AmbientDialogueButton : public ActionButton::ButtonHandler
{
public:
    AmbientDialogueButton(Character* c) : mpCharacter(c)
    {
    }

    bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
    {
        //
        // Send a couple of sound events for dialog.
        //
        if( SoundManager::IsFoodCharacter( mpCharacter ) )
        {
            GetEventManager()->TriggerEvent( EVENT_AMBIENT_ASKFOOD );
            GetEventManager()->TriggerEvent( EVENT_AMBIENT_FOODREPLY, mpCharacter );
        }
        else
        {
            GetEventManager()->TriggerEvent( EVENT_AMBIENT_GREETING );
            GetEventManager()->TriggerEvent( EVENT_AMBIENT_RESPONSE, mpCharacter );
        }
        return true;
    }

protected:
    Character* mpCharacter;
};

class AmbientDialogueTrigger : public SphereTriggerVolume
{
public:

    AmbientDialogueTrigger(Character* c, float radius) : SphereTriggerVolume(rmt::Vector(0,0,0), radius), mpCharacter(c)
    {
        SetLocator(new TriggerLocator);
        GetLocator()->SetNumTriggers(1);
        GetLocator()->AddTriggerVolume(this);
        GetLocator()->AddRef();
        mButton = new AmbientDialogueButton(c);
        mButton->AddRef();
    }

    ~AmbientDialogueTrigger()
    {
        tRefCounted::Release(mButton);
    }

    void ClearLocator(void)
    {
        GetLocator()->Release();
    }
    void Trigger( unsigned int playerID, bool bActive )
    {
        if(bActive)
        {
            Character* character = GetAvatarManager()->GetAvatarForPlayer(playerID)->GetCharacter();
            if ( character )
            {
                character->AddActionButtonHandler(mButton);
            }
        }
        else
        {
            Character* character = GetAvatarManager()->GetAvatarForPlayer(playerID)->GetCharacter();
            if ( character )
            {
                character->RemoveActionButtonHandler(mButton);
            }
        }
    }

protected:
    Character* mpCharacter;
    AmbientDialogueButton* mButton;
};
    
/*
==============================================================================
NPCharacter::NPCharacter
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         NPCharacter

=============================================================================
*/
NPCharacter::NPCharacter( void )
:
Character( ),
mMappableHandle( Input::INVALID_CONTROLLERID )
{
MEMTRACK_PUSH_GROUP( "NPCCharacter" );

    mIsNPC = true;
    SetInSubstep( false ); // initially false

    /***
    CharacterMappable* pCharacterMappable = new (GMA_PERSISTENT) BipedCharacterMappable;

    PhysicalController* pController = new (GMA_PERSISTENT) PhysicalController;
    this->SetController( pController );

    pController->SetCharacterMappable( pCharacterMappable );
    pCharacterMappable->SetCharacterController( pController );
    
    mMappableHandle = InputManager::GetInstance()->RegisterMappable( 1, pCharacterMappable );
    ***/
    this->SetController( new(GMA_LEVEL_OTHER) NPCController );
    this->GetController()->SetCharacter( this );
MEMTRACK_POP_GROUP( "NPCCharacter" );
}
/*
==============================================================================
NPCharacter::~NPCharacter
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         NPCharacter

=============================================================================
*/
NPCharacter::~NPCharacter( void )
{
    // if we don't clear this here, there may be actions cued that 
    // will screw up once we've been destructed
    if(GetActionController())
    {
        GetActionController()->Clear();
    }

    /***
    InputManager::GetInstance()->UnregisterMappable( 1, mMappableHandle );
    ***/
    mMappableHandle = Input::INVALID_CONTROLLERID;
}

#ifdef RAD_DEBUG
    static float timeScale = 1.00f;
#endif

/*
==============================================================================
NPCharacter::OnUpdateRoot
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void NPCharacter::OnUpdateRoot( float timeins )
{
    // Intentionall left blank.
    //
}

/*
==============================================================================
NPCharacter::OnPostSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void NPCharacter::OnPostSimUpdate( float timeins )
{
    UpdatePuppet( timeins );
}
/*
==============================================================================
Character::Character
==============================================================================
Description:    Constructor

Parameters:     ( )

Return:         na

=============================================================================
*/
sim::TArray< sim::RayIntersectionInfo > Character::msIntersectInfo( 64 ); //.ResizeArray( 2 );

Character::Character( )
:
mbCollidedWithVehicle( false ),
mbInAnyonesFrustrum( false ),
mbSurfing(false), 
mbAllowUnload(true),
mbIsPlayingIdleAnim(false),

#ifdef RAD_WIN32
mPCCamFacing( 0 ),
#endif

mIsNPC( false ),

mGroundPlaneSimState( 0 ),
mGroundPlaneWallVolume( 0 ),
mCollisionAreaIndex( WorldPhysicsManager::INVALID_COLLISION_AREA ),

mpController( 0 ),
mpCharacterRenderable( 0 ),
mpPuppet( 0 ),
mfFacingDir( 0.0f ),
mfDesiredDir( 0.0f ),
mfSpeed( 0.0f ),
mVelocity (0.0f, 0.0f, 0.0f),
mfDesiredSpeed( 0.0f ),
mbInCar( false ),
mpCharacterTarget( 0 ),
mpActionController( 0 ),
mpCurrentActionButtonHandler( 0 ),
mpTargetVehicle( 0 ),
mTerrainType( TT_Road ),
mInteriorTerrain( false ),
mpStateManager( 0 ),
mfRadius( 0.35f ),
mbCollided( false ),
mCurrentCollision( 0 ),
mbIsStanding( true ),
mpWalkerLocomotion( 0 ),
mpJumpLocomotion( 0 ),
mpStandingCollisionVolume( 0 ),
mpStandingJoint( 0 ),
mfGroundVerticalVelocity( 0.0f ),
mfGroundVerticalPosition( 0.0f ),
mbTurbo( false ),
mbIsJump( false ),
mbSolveCollisions( true ),
mpPropHandler( 0 ),
mPropJoint( -1 ),
mVisible( false ),
mpWorldScene( 0 ),
m_IsSimpleShadow( true ),
mYAdjust( 0.0f ),
mbBusy(false),
mbSimpleLoco( false ),
m_TimeLeftToShock( 0 ),
m_IsBeingShocked( false ),
mDoKickwave(false),
mKickwave(NULL),
mKickwaveController(NULL),
mAmbient(false),
mAmbientLocator(0),
mAmbientTrigger(NULL),
mLastFramePos(0.0f,0.0f,0.0f),
mbDoGroundIntersect(true),
mIntersectFrame(s_IntersectFrame++),
mAllowRockin(false),
mHasBeenHit(false),
mbSnapToGround(false),
mSecondsSinceActionControllerUpdate( 0.0f ),
mTooFarToUpdate(false),
mSecondsSinceOnPostSimUpdate( 0.0f ),
mRole(ROLE_UNKNOWN),
mScale(1.0f),
mIsInSubstep(true),
mLean(0.0f, 1.0f, 0.0f),
mIsLisa(false),
mIsMarge(false),
mManaged(false)
{

    mLastInteriorLoadCheck = radTimeGetMicroseconds64();

    mPrevSimTransform.Identity();

    mTranslucent = true;

    mShadowColour.Set( 0, 0, 0 );

    mGroundNormal.Set( 0.0f, 0.0f, 0.0f );
    mRealGroundPos.Set( 0.0f, 0.0f, 0.0f );
    mRealGroundNormal.Set( 0.0f, 0.0f, 0.0f );

    mLastGoodPosOverStatic.Set( 0.0f, 0.0f, 0.0f );

    unsigned int i;
    for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
    {
        mpActionButtonHandlers[ i ] = NULL;
    }

}
/*
==============================================================================
Character::Init
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::Init( void )
{

//#ifdef RAD_GAMECUBE
//    HeapMgr()->PushHeap(GMA_GC_VMM);
//#else
    HeapMgr()->PushHeap(GMA_LEVEL_OTHER);
//#endif

    mpCharacterTarget = new CharacterTarget( this );

    mbWasFootPlanted.resize(2, false);

    mpActionController = new ActionController;
    mpActionController->Clear( );

    mpStateManager = new CharacterAi::StateManager( this );

    {
        // Manually create the physics objects for now.
        // Set inventory section to "Default" because the sim library will store stuff
        //
        p3d::inventory->PushSection( );
        p3d::inventory->SelectSection( "Default" );

        // Manually create the physics objects for now.
        //
        sim::SymMatrix identity;
        identity.Identity();
        rmt::Vector offset( 0.0f, 0.9f, -0.05f );
        
        sim::CylinderVolume* pCollisionVolume = new sim::CylinderVolume( offset, vUp, 0.55f, mfRadius );
        sim::CollisionObject* pCollisionObject = new sim::CollisionObject( pCollisionVolume );
        sim::PhysicsProperties* pPhysicsProperties = sim::PhysicsProperties::HardWoodProperties(p3d::inventory);
        sim::PhysicsObject* pSimulatedObject = new sim::PhysicsObject( pPhysicsProperties, offset, identity, 2.0f );
        pSimulatedObject->SetSimEnvironment( GetWorldPhysicsManager()->mSimEnvironment );
        sim::SimState* pSimState = sim::SimState::CreateSimState( pCollisionObject, pSimulatedObject );
        rAssert( pSimState );
        SetSimState( pSimState );

        pCollisionObject->SetCollisionEnabled(false);
        SetSolveCollisions(false);
        
        p3d::inventory->PopSection( );
    }

    int i;
    for ( i = 0; i < CollisionData::MAX_COLLISIONS; i++ )
    {
        mCollisionData[ i ].mCollisionNormal.Set( 0.0f, 0.0f, 0.0f );
        mCollisionData[ i ].mCollisionDistance = 0.0f;
        mCollisionData[ i ].mpCollisionVolume = (sim::CollisionVolume*)0;
    }
    mbCollidedWithVehicle = false;
    
    mpWalkerLocomotion = new WalkerLocomotionAction( this );
    mpWalkerLocomotion->AddRef();

    mpJumpLocomotion = new JumpAction( this, "jump_idle", Character::GetJumpHeight() );
    mpJumpLocomotion->AddRef();

    rmt::Matrix mat;
    mat.Identity( );
    SetParentTransform( mat );

/*    mpPropHandler = new ActionButton::AttachProp;
    mpPropHandler->AddRef( );
  */  
    mPropJoint = 0;

    if( !IsNPC() )
    {
        InitGroundPlane();
    }
    //AssignCollisionAreaIndex( );

    //////////////////////////////
    // Update Simstate transform
    rmt::Vector position;
    GetPosition( position );
    rmt::Vector facing;
    GetFacing( facing );

    mat.Identity( );
    mat.Row( 2 ) = facing;
    mat.FillTranslate( position );

    mpSimStateObj->SetTransform( mat );

    if( !mIsNPC )
    {
        // If you're a player character
        // Add ourself, our groundplane, and our self-groundplane pair 
        // to collision manager
        AddToPhysics();
    }

    // Initialize position at origin
    rmt::Vector zero( 0.0f, 0.0f, 0.0f );
    SetPosition( zero );

    UpdateTransformToLoco( );

    // Dusit [Nov 13,2002]:
    // Need to initialize out the garbage values. The problem is that
    // the garbage values happen to be quite large... In a later call to
    // UpdateSimState, we overwrite the currently quite nice simstate &
    // collisionvolume transforms with the puppet's garbage value (still
    // garbage because cloned NPCs such as pedestrians are not given a valid
    // position by locator, but by arbitrary spawning). So now simstate &
    // collisionvolume have large garbage transforms. Later on when 
    // this cloned NPC gets spawned (e.g. in Pedestrian::Activate()) with
    // a valid position, the collisionvolume tries to update itself
    // by calculating the difference between LARGE garbage value and a
    // comparatively smaller position value. The floating point accuracy
    // favors the large value and instead of moving to the new smaller 
    // position value, we move to (0,0,0). At this point, simstate's 
    // transform and simcollisionvolume's transform are out of synch.
    // 
    SetPosition( position );

//#ifdef RAD_GAMECUBE
//    HeapMgr()->PopHeap( GMA_GC_VMM );
//#else
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
//#endif
}


void Character::InitGroundPlane()
{
    MEMTRACK_PUSH_GROUP( "Character" );

    HeapMgr()->PushHeap (GMA_LEVEL_OTHER);

    rmt::Vector p(0.0f, 0.0f, 0.0f);
    rmt::Vector n(0.0f, 1.0f, 0.0f);

    mGroundPlaneWallVolume = new sim::WallVolume(p, n);
    mGroundPlaneWallVolume->AddRef();

    mGroundPlaneSimState = (sim::ManualSimState*)(
        sim::SimState::CreateManualSimState(mGroundPlaneWallVolume));
    mGroundPlaneSimState->AddRef(); 

    mGroundPlaneSimState->GetCollisionObject()->SetManualUpdate(true);
    mGroundPlaneSimState->GetCollisionObject()->SetAutoPair(false);
    mGroundPlaneSimState->GetCollisionObject()->SetIsStatic(true);
    mGroundPlaneSimState->GetCollisionObject()->SetCollisionEnabled(false);
    
    char buffy[128];
    sprintf( buffy, "player_character_groundplane" );
    mGroundPlaneSimState->GetCollisionObject()->SetName( buffy );

    mGroundPlaneSimState->mAIRefIndex = PhysicsAIRef::redBrickPhizMoveableGroundPlane;
    mGroundPlaneSimState->mAIRefPointer = (void*)this;

    /*
    mGroundPlanePhysicsProperties = new PhysicsProperties;
    mGroundPlanePhysicsProperties->AddRef();

    mGroundPlanePhysicsProperties->SetFrictCoeffCGS(0.8f);
    mGroundPlanePhysicsProperties->SetRestCoeffCGS(1.15f);
    mGroundPlanePhysicsProperties->SetTangRestCoeffCGS(0.0f);
   
    mGroundPlaneSimState->SetPhysicsProperties(mGroundPlanePhysicsProperties);
    */

    HeapMgr()->PopHeap (GMA_LEVEL_OTHER);
    MEMTRACK_POP_GROUP( "Character" );
}

bool Character::IsInCarOrGettingInOut( void )
{
    if(mbInCar ||
        GetStateManager()->GetState() == CharacterAi::GET_IN ||
        GetStateManager()->GetState() == CharacterAi::GET_OUT)
    {
        return true;
    }
    return false;
}

/*
==============================================================================
Character::SetPuppet
==============================================================================
Description:    Comment

Parameters:     ( choreo::Puppet* pPuppet )

Return:         void

=============================================================================
*/
void Character::SetPuppet( choreo::Puppet* pPuppet )
{
    rAssert( pPuppet );

    rmt::Vector position;
    float facing, desiredFacing;
    GetPosition( position );
    facing = GetFacingDir();
    desiredFacing = GetDesiredDir();
    tRefCounted::Assign( mpPuppet, pPuppet );
    SetPosition( position );
    SetFacingDir(facing);
    SetDesiredDir(desiredFacing);

    if(mpWalkerLocomotion)
    {
        mpWalkerLocomotion->SwitchLocomotion( );
    }

    mbWasFootPlanted.resize( pPuppet->GetLegCount(), false );
/*
    if ( pPuppet )
    {
        mPropJoint = pPuppet->GetP3DPose( )->FindJointIndex( "Wrist_R" );
    }
    if ( mPropJoint == -1 )
    {
        // Safe value.
        //
        mPropJoint = 0;
    }
*/
    // The feet collider doesn't work as well as the ray intersection.
    // It isn't any cheaper in the grand scheme of things either.
    //
    //mpFeetCollider = new CharacterFeetCollider( this );
    int legs = mpPuppet->GetLegCount( );
    for ( int i = 0; i < legs; i++ )
    {    
        GetPuppet( )->SetIsLegIKEnabled( i, false );
    }

    if(GetActionController())
    {
        GetActionController()->Clear();
    }

    if(mpStateManager)
    {
        SetInCar(false);

        if(mpStateManager->GetState() != CharacterAi::NOSTATE)
        {
            mpStateManager->ResetState();
        }
    }

    pPuppet->GetEngine()->GetPoseEngine()->Begin(true);
}


void Character::SetYAdjust( float yOffset )
{
    mYAdjust = yOffset;
}

float Character::GetYAdjust()
{
    return mYAdjust;
}

/*
==============================================================================
Character::ResetSpeed
==============================================================================
*/
void Character::ResetSpeed( void )
{   
    mpSimStateObj->ResetVelocities( );
    mpWalkerLocomotion->SetDesiredSpeed( 0.0f );
}
static rmt::Vector dstart,dend;
void Character::Kick()
{
    if(GetInteriorManager()->IsInside())
    {
        return;
    }

    // Fetch the current character position
    rmt::Vector position;
    GetPosition( &position );

    // Fetch character facing vector
    rmt::Vector facing;
    GetFacing( facing );
   

    // We want to rotate the facing vector upwards by N degrees
    // Find the vector thats orthogonal to the facing vector and the world up axis
    // Lets make sure that the facing vector isn't parallel with the up vector
    // first
    rmt::Vector kickdir;
    const rmt::Vector WORLD_UP( 0, 1.0f, 0 );
    const float DOT_PRODUCT_PARALLEL_EPSILON = 0.99f;
    if ( facing.Dot( WORLD_UP ) < DOT_PRODUCT_PARALLEL_EPSILON )
    {
        // Not parallel, take the crossproduct between world and up
        rmt::Vector right;
        right.CrossProduct( WORLD_UP, facing );
        rmt::Matrix rotation;
        rotation.Identity();
        rotation.FillRotation( right, rmt::DegToRadian( KICK_ANGLE ));
        kickdir.Rotate( facing, rotation );
        rAssert( kickdir.y > 0 );
    }
    else
    {
        // They are parallel, just use the facing vector as the kicking direction
        kickdir = facing;
    }




    const float KICK_EFFECT_RADIUS = 5.0f;
    WorldPhysicsManager::NumObjectsHit numObjectsHit;

    // Use the intersection list for querying and performing sim collision testing
    IntersectionList intersectList;
    intersectList.FillIntersectionListDynamics( position, KICK_EFFECT_RADIUS, true, this );
    DynaPhysDSG* objectHit = NULL;
    const float KICK_RAY_LEN = 2.0f;


    rmt::Vector kickDest = position + kickdir;
    rmt::Vector kickStart = position - kickdir;


    rmt::Vector kickIntersect;
    if ( intersectList.TestIntersectionDynamics( kickStart, kickDest, &kickIntersect, &objectHit ) )
    {
        switch ( objectHit->GetAIRef() )
        {
        case PhysicsAIRef::NPCharacter:
            {
                // We kicked an NPC. Send some events
                NPCharacter* ch = (NPCharacter*)objectHit;
                GetEventManager()->TriggerEvent( EVENT_KICK_NPC, ch );
                GetEventManager()->TriggerEvent( EVENT_KICK_NPC_SOUND, ch );
                GetEventManager()->TriggerEvent( EVENT_PEDESTRIAN_SMACKDOWN );
                ch->SetHasBeenHit(true);

                // Make the object that got hit fly
                CharacterAi::CharacterState state = ch->GetStateManager()->GetState();
                if( state == CharacterAi::LOCO )
                {
                    // call a special kick that will reset the pastangular and pastlinear
                    // histories... 
                    ch->ApplyKickForce( kickdir, CharacterTune::sfKickingForce );
                }
                else
                {
                    ch->ApplyForce( kickdir, CharacterTune::sfKickingForce );
                }
                GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, objectHit );
            }
            break;

        case PhysicsAIRef::redBrickVehicle:
            GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, objectHit );
            break;

        case PhysicsAIRef::StateProp:
            {
                rAssert( dynamic_cast< StatePropDSG* >( objectHit ) != NULL );
                StatePropDSG* stateprop = static_cast< StatePropDSG* >( objectHit );
                // Lets not send EVENT_OBJECT_KICKED when the object has no collision volume
                if ( stateprop->IsCollisionEnabled() )
                {
                    objectHit->ApplyForce( kickdir, CharacterTune::sfKickingForce );
                    GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, objectHit );
                }
            }
            break;

        default:
            // Make the object that got hit fly
            objectHit->ApplyForce( kickdir, CharacterTune::sfKickingForce );
            InstDynaPhysDSG* toBreak = dynamic_cast<InstDynaPhysDSG*>(objectHit);

            if(toBreak)
            {
                toBreak->Break();
            }

            GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, objectHit );
            break;
        }
    }
    GetEventManager()->TriggerEvent( EVENT_KICK, this );

}   

void Character::Slam()
{
    int i;
    WorldPhysicsManager::CollisionEntityDSGList dsgList;

    rmt::Vector position;
    GetPosition( &position );

    int numObjectsKicked = GetWorldPhysicsManager()->ApplyForceToDynamicsSpherical( mCollisionAreaIndex, 
                                                                                    position, 
                                                                                    2, 
                                                                                    CharacterTune::sfSlamForce,
                                                                                    &dsgList );

    if ( numObjectsKicked > 0 )
    {
        for( i = 0; i < WorldPhysicsManager::CollisionEntityDSGList::NUM_COLLISION_LIST_ENTITIES; i++ )
        {
            CollisionEntityDSG* collObject = dsgList.collisionEntity[i];

            if( collObject != NULL )
            {
                switch ( collObject->GetAIRef() )
                {
                case PhysicsAIRef::NPCharacter:
                    {
                        Character* ch = static_cast<Character*>(dsgList.collisionEntity[i]);
                        GetEventManager()->TriggerEvent( EVENT_KICK_NPC, ch );
                        GetEventManager()->TriggerEvent( EVENT_KICK_NPC_SOUND, ch );
                        GetEventManager()->TriggerEvent( EVENT_PEDESTRIAN_SMACKDOWN );
                        ch->SetHasBeenHit(true);
                        GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, collObject );
                    }
                    break;
                case PhysicsAIRef::StateProp:
                    {
                        rAssert( dynamic_cast< StatePropDSG* >( collObject ) != NULL );
                        StatePropDSG* stateprop = static_cast< StatePropDSG* >( collObject );
                        // Lets not send EVENT_OBJECT_KICKED when the object has no collision volume
                        if ( stateprop->IsCollisionEnabled() )
                        {
                            GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, collObject );
                        }                    
                        break;
                    }
                default:
                    GetEventManager()->TriggerEvent( EVENT_OBJECT_KICKED, collObject );
                    InstDynaPhysDSG* toBreak = dynamic_cast<InstDynaPhysDSG*>(collObject);

                    if(toBreak)
                    {
                        toBreak->Break();
                    }
                    break;
                }

            }
        }
    }
    GetEventManager()->TriggerEvent( EVENT_STOMP, this );
}   

void Character::RelocateAndReset( const rmt::Vector& position, float facing, bool resetMe, bool snapToGround )
{
    // TODO: Not done yet... 
    // What else need we do to relocate character & reset its states???
    mLastFramePos = position;
    mVelocity.Set(0.0f, 0.0f, 0.0f);

    // Switch to AI control if we need to...
    sim::SimState* simState = mpSimStateObj;
    if( simState != NULL )
    {
        simState->SetControl( sim::simAICtrl );
    }

    // Update transforms for PoseJointZero, Puppet, and SimStateObj
    SetPosition( position );

    // update Puppet's root transform with facing 
    SetFacingDir( facing );
    SetDesiredDir( facing );
	ResetSpeed();

    if( mpWalkerLocomotion != NULL )
    {
        // Gotta do this so facing is not overwritten by blend priorities
        // when the player's character is involved...
        // Without this call here, the player's character will gets its 
        // new facing value clobbered by old values stored in blend priorities
        // when we go into UpdateTransformToLoco()
        choreo::LocomotionDriver* locomod = mpWalkerLocomotion->GetDriver();
        if( locomod != NULL )
        {
            locomod->SetActualFacingAngle( facing );
        }
    }
    
    SetStandingJoint(NULL);

    UpdateTransformToLoco();

    // By this point, our position should be finalized and no UpdateBBox 
    // has been called (for moveinworldscene to do)

    // Don't call UpdatePuppet() because it resets our puppet->engine->rootblender's 
    // and joints' transforms back to previous one due to blend priorities in UpdateRoot()
    //UpdatePuppet( 0.0f );
    choreo::Puppet* pPuppet = GetPuppet( );
    rAssert( pPuppet );
    pPuppet->UpdatePose();
//    pPuppet->UpdateEnd();

    // update jump's root transform after Puppet's transform is set
    if( mpJumpLocomotion != NULL )
    {
        // Copies transform from Puppet->Engine->RootBlender.
        // If that is correct, so will this be.
        mpJumpLocomotion->SetRootTransform();
    }
    if( mpWalkerLocomotion != NULL )
    {
        // Characters obtained a new locomotion driver in previous call to
        // UpdateTransformToLoco, so we have to reset that driver's 
        // actual facing angle again...
        // Without this call, NPCs won't face the correct way...
        // Player character can get away with it cuz the driver gets updated
        // again later.
        choreo::LocomotionDriver* locomod = mpWalkerLocomotion->GetDriver();
        if( locomod != NULL )
        {
            locomod->SetActualFacingAngle( facing );
        }
    }


    ////////////////////////////////////////////////////////////////////
    // 
    // Force NPC to submit statics and whatever's below them &
    // update their terrain intersect info. We should actually do this
    // for EVERY character, but dammit we can't afford to.
    //
    if( mIsNPC && mRole != ROLE_PEDESTRIAN ) 
    {
        // temporarily transit the controller to STOPPED state so we force submit statics
        // to actually submit statics...
        NPCController* npcController = (NPCController*) mpController;
        NPCController::State oldState = npcController->GetState();
        npcController->TransitToState( NPCController::STOPPED );
        
        // temporarily obtain collision area index if we don't got one...
        int oldArea = mCollisionAreaIndex;
        if( oldArea == WorldPhysicsManager::INVALID_COLLISION_AREA )
        {
            AddToPhysics();
        }

        SubmitStatics();

        rmt::Vector prevPosition = position;
        rmt::Vector groundPosition = position;
        rmt::Vector outnorm( 0.0f, 1.0f, 0.0f );
        bool bFoundPlane = false;

        mTerrainType = static_cast<eTerrainType>( GetIntersectManager()->FindIntersection(
            groundPosition,   // IN
            bFoundPlane,      // OUT
            outnorm,          // OUT
            groundPosition ) );   // OUT
        mInteriorTerrain = ( (int)mTerrainType & 0x80 ) == 0x80;
        mTerrainType = static_cast<eTerrainType>( ( (int)mTerrainType & ~0x80 ) );

        if( bFoundPlane )
        {
            mRealGroundPos = groundPosition;
            mRealGroundNormal = outnorm;
            float tooHigh = 100000.0f;
            rAssert( -tooHigh <= mRealGroundNormal.x && mRealGroundNormal.x <= tooHigh );
            rAssert( -tooHigh <= mRealGroundNormal.y && mRealGroundNormal.y <= tooHigh );
            rAssert( -tooHigh <= mRealGroundNormal.z && mRealGroundNormal.z <= tooHigh );
        }
        else
        {
            // 
            // If this assert goes off for the player charater when he's hit by traffic
            // it means that Physics has placed him at some location other than his
            // present location. This is a bad thing... possibly related to 
            // collisions with traffic cars.
            //rAssertMsg( false, "We're SOOO far from the ground, we're not intersecting" );
        }
        rmt::Vector collisionPosition;
        rmt::Vector collisionNormal;
        
        collisionNormal.Clear( );
        collisionPosition.Clear( );

        bool bOverStatic = GetCollisionHeight( prevPosition, position, collisionPosition, collisionNormal );
        if ( bOverStatic )
        {
            if ( !bFoundPlane || collisionPosition.y > groundPosition.y )
            {
                mGroundY = collisionPosition.y;
                mGroundNormal = collisionNormal;

                mLastGoodPosOverStatic = position;
                mLastGoodPosOverStatic.y = mGroundY;
            }
            else 
            {
                rAssert( bFoundPlane );
                mGroundY = groundPosition.y;
                mGroundNormal = outnorm;
            }
        }
        else if ( bFoundPlane )
        {
            mGroundY = groundPosition.y;
            mGroundNormal = outnorm;
        }
        else
        {
            mGroundY = position.y;
            mGroundNormal.Set( 0.0f, 1.0f, 0.0f );
        }

        if( snapToGround )
        {
            rmt::Vector groundPos, groundNormal;
            GetTerrainIntersect( groundPos, groundNormal );
            SetPosition( groundPos );
            SetGroundPoint( groundPos );
            mpJumpLocomotion->SetRootPosition( WorldToLocal(groundPos) );
            snapToGround = false;
        }

        if( oldArea == WorldPhysicsManager::INVALID_COLLISION_AREA )
        {
            RemoveFromPhysics();
        }
        npcController->TransitToState( oldState );
    }
    //////////////////////////////////////////////////////////////////////////



    MoveInWorldScene();
    if( GetCharacterManager()->GetCharacter(0) == this )
        GetTriggerVolumeTracker()->ResetDynaloadZones();

    mbSnapToGround = snapToGround;
    mTooFarToUpdate = false;
    mSecondsSinceActionControllerUpdate = 0.0f;
    mSecondsSinceOnPostSimUpdate = 0.0f;


    if( resetMe )
    {
        // Reset collisions (otherwise the collisions carry on from last placement)
        ResetCollisions();

        // do SimState reset
        if( mpSimStateObj != NULL )
        {
            mpSimStateObj->ResetVelocities();
        }

        // Reset action button so we don't get blinking "y" after reset
        ClearAllActionButtonHandlers();

        mbTurbo = false;

        // Clear props?? 
//        mpPropHandler->Reset();

        GetStateManager()->SetState<CharacterAi::Loco>();

        mbIsJump = false;

        mpActionController->Clear();

        /*
        // TODO: 
        // Do more resetting here... If it gets to be a lot, gotta put it in a 
        // separate ResetStates() function

        // Clear actions & priorities??

        // TODO: 
        // How do we halt the jump sequence & reset back to standing?
        // Actually. We need a general "HaltAnimations" function that stops
        // whatever you're doing and reset to standing & idle animation...
        // This includes stopping: jumping, opening doors, turboing, dashing, etc...

        if( mbIsJump )
        {
            mpJumpLocomotion->End();
            mpJumpLocomotion->Done();
            mbIsJump = false;
        }

        // TODO: 
        // DO we need to do these things?
        if( mpLocomotion != NULL )
        {
            mpLocomotion->Clear();
        }

        if( mpActionController != NULL )
        {
            mpActionController->Clear();
        }

        if( mpController != NULL )
        {
            mpController->ClearIntention();
        }

        if( mpStateManager != NULL )
        {
            mpStateManager->ResetState( CharacterAi::LOCO, this );
        }


        //mbWasFootPlanted.clear();
        mbIsStanding = true;
        mbSolveCollisions = false;

        */
    }
}



void Character::AddToPhysics( void )
{   
    if( !mManaged )
    {
        return;
    }

    if( mCollisionAreaIndex != WorldPhysicsManager::INVALID_COLLISION_AREA )
    {
        return;
    }

    sim::CollisionObject* myColObj = mpSimStateObj->GetCollisionObject();
    sim::CollisionObject* groundPlaneColObj = mGroundPlaneSimState->GetCollisionObject();

    AssignCollisionAreaIndex();

    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(
        myColObj, mCollisionAreaIndex );
    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(
        groundPlaneColObj, mCollisionAreaIndex);
    GetWorldPhysicsManager()->mCollisionManager->AddPair(
        groundPlaneColObj, myColObj, mCollisionAreaIndex);

    myColObj->SetCollisionEnabled( true );

    // disable groundplane collision till we need it
    groundPlaneColObj->SetCollisionEnabled( false ); 

    SetSolveCollisions( true );
}

void Character::RemoveFromPhysics( void )
{
    if( mCollisionAreaIndex == WorldPhysicsManager::INVALID_COLLISION_AREA )
    {
        return;
    }

    sim::CollisionObject* myColObj = mpSimStateObj->GetCollisionObject();
    sim::CollisionObject* groundPlaneColObj = mGroundPlaneSimState->GetCollisionObject();

    myColObj->SetCollisionEnabled( false );
    groundPlaneColObj->SetCollisionEnabled( false );

    // Freeing a collision area will also empty it out, so no remove calls 
    // necessary here.
    GetWorldPhysicsManager()->FreeCollisionAreaIndex( mCollisionAreaIndex );

    mCollisionAreaIndex = WorldPhysicsManager::INVALID_COLLISION_AREA;

    SetSolveCollisions( false );
}

void NPCharacter::AddToPhysics( void )
{   
    // NPCs don't keep track of their own groundplane... When they go into sim
    // they are submitted by some other entity and get temp groundplanes 
    // assigned to them. 
    // So we only add ourselves to collisions

    /*
#ifdef RAD_DEBUG
    rDebugPrintf( "+++++++++ Adding to Physics: %s, index %d\n", GetName(), mCollisionAreaIndex );
#endif
    */

    if( mCollisionAreaIndex != WorldPhysicsManager::INVALID_COLLISION_AREA )
    {
        return;
    }

    sim::CollisionObject* myColObj = mpSimStateObj->GetCollisionObject();

    AssignCollisionAreaIndex();

    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(
        myColObj, mCollisionAreaIndex );

    myColObj->SetCollisionEnabled( true );

    SetSolveCollisions( true );
}

void NPCharacter::RemoveFromPhysics( void )
{
    /*
#ifdef RAD_DEBUG
    rDebugPrintf( "-------- Removing from Physics: %s, index %d\n", GetName(), mCollisionAreaIndex );
#endif
    */

    if( mCollisionAreaIndex == WorldPhysicsManager::INVALID_COLLISION_AREA )
    {
        return;
    }

    sim::CollisionObject* myColObj = mpSimStateObj->GetCollisionObject();

    myColObj->SetCollisionEnabled( false );

    // Freeing a collision area will also empty it out, so no remove calls 
    // necessary here.

    GetWorldPhysicsManager()->FreeCollisionAreaIndex( mCollisionAreaIndex );

    mCollisionAreaIndex = WorldPhysicsManager::INVALID_COLLISION_AREA;

    SetSolveCollisions( false );

}




/*
==============================================================================
Character::~Character
==============================================================================
Description:    Destructor

Parameters:     ( void )

Return:         na

=============================================================================
*/
Character::~Character( void )
{
    tRefCounted::Release(mpStandingCollisionVolume);

    if(GetActionController())
    {
        GetActionController()->Clear();
    }

    tRefCounted::Release(mKickwave);
    tRefCounted::Release(mKickwaveController);

    if( mGroundPlaneSimState )
    {
        if(mCollisionAreaIndex != WorldPhysicsManager::INVALID_COLLISION_AREA)
        {
            GetWorldPhysicsManager()->mCollisionManager->RemoveCollisionObject(
                mGroundPlaneSimState->GetCollisionObject(), 
                mCollisionAreaIndex);
        }
        mGroundPlaneSimState->Release();
        mGroundPlaneSimState = NULL;
    }
    if( mGroundPlaneWallVolume )
    {
        mGroundPlaneWallVolume->Release();
        mGroundPlaneWallVolume = NULL;
    }

    // NOTE:
    // Do the same RemoveCollisionObject call for our simstate?
    // No need. CharacterManager's destroy will call 
    // RemoveFromAllDynamicBlahblahblah for us. 

    if( WorldPhysicsManager::INVALID_COLLISION_AREA != mCollisionAreaIndex )
    {
        GetWorldPhysicsManager()->FreeCollisionAreaIndex( mCollisionAreaIndex );
    }

    if( mpController )
    {
        mpController->Release( );
        mpController = 0;
    }
    if( mpCharacterRenderable )
    {
        delete mpCharacterRenderable;
    }
    if ( mpCharacterTarget )
    {
        delete mpCharacterTarget;
        mpCharacterTarget = 0;
    }
    if ( mpActionController )
    {
        delete mpActionController;
        mpActionController = 0;
    }
    if ( mpStateManager )
    {
        delete mpStateManager;
        mpStateManager = 0;
    }
    if ( mpWalkerLocomotion )
    {
        mpWalkerLocomotion->Release( );
        mpWalkerLocomotion = 0;
    }
    if ( mpJumpLocomotion )
    {
        mpJumpLocomotion->Release( );
        mpJumpLocomotion = 0;
    }
    if ( mbWasFootPlanted.empty() == false )
    {
        mbWasFootPlanted.clear();
    }

    SetTargetVehicle(NULL);
    ClearAllActionButtonHandlers();
//    tRefCounted::Release( mpPropHandler );
    if(mAmbientTrigger)
    {
        mAmbientTrigger->ClearLocator();
    }
    tRefCounted::Release( mAmbientTrigger );

    //
    // Delete the puppet last.  I moved this down to the bottom because of an occasional
    // non-reproducible crash on gameplay exit where the ActionController destruction above
    // ended up referencing the puppet which had already been cleaned up.  Other desctructors
    // above (e.g. locomotion objects) may also end up referencing the puppet.  Putting it
    // at the bottom should avoid that.  -- jdy
    //
    if ( mpPuppet )
    {
        mpPuppet->ReleaseVerified();
        mpPuppet = 0;
    }
}
/*
==============================================================================
Character::UpdateParentTransform
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdateParentTransform( float timeins )
{
    // Do this before UpdateRoot()
    //
    
    rmt::Matrix transform;
    transform.Identity( );
    if ( IsInCar() && GetTargetVehicle() )
    {
        transform = GetTargetVehicle()->GetTransform();
    }
    else if ( mpStandingJoint )
    {
        transform = mpStandingJoint->GetWorldMatrix( );
    } 
    SetParentTransform( transform, timeins );

}


/*
==============================================================================
Character::SetParentTransform
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Matrix& mat, float timeins )

Return:         void 

=============================================================================
*/
void Character::SetParentTransform( const rmt::Matrix& newTransform, float timeins )
{
    if ( !rmt::Epsilon( timeins, 0.0f ) )
    {
        float invDeltaTime = 1.0f / timeins;   
        mfGroundVerticalVelocity = newTransform.Row( 3 ).y - mfGroundVerticalPosition;
        mfGroundVerticalVelocity *= invDeltaTime;  
    }
    else
    {
        mfGroundVerticalVelocity = 0.0f;
    }
    mParentTransform = newTransform;
    mfGroundVerticalPosition = mParentTransform.Row( 3 ).y;

    //rAssertMsg( mParentTransform.IsOrthoNormal( ), "Your parent transform node is screwed!\n" );
    
    if ( !IsInCar( ) )
    {
        // If we are not in the car, we always want to keep the character up
        // at 0,1,0.
        //
        rmt::Vector facing = mParentTransform.Row( 2 );
        mParentTransform.FillHeadingXZ( facing );
        
        // We don't detect vertical motion with the transform.
        // so we will zero it out.
        //
        mParentTransform.Row( 3 ).y = 0.0f;
    }
    mInvParentTransform.InvertOrtho( mParentTransform );
    if ( mpPuppet )
    {
        poser::Transform transform;
        transform.Identity( );
        transform.SetMatrix( mParentTransform );
        mpPuppet->SetParentTransform( transform );
    }
}


void Character::UpdateGroundPlane( float timeins )
{
    // new approach with manual sim state
    rAssert( mGroundPlaneWallVolume );

    float tooHigh = 100000.0f;
    rAssert( -tooHigh <= mRealGroundNormal.x && mRealGroundNormal.x <= tooHigh );
    rAssert( -tooHigh <= mRealGroundNormal.y && mRealGroundNormal.y <= tooHigh );
    rAssert( -tooHigh <= mRealGroundNormal.z && mRealGroundNormal.z <= tooHigh );

    mGroundPlaneWallVolume->mPosition = mRealGroundPos; //p;
    mGroundPlaneWallVolume->mNormal = mRealGroundNormal; //n;
    
    rAssert( mGroundPlaneSimState );   
    sim::CollisionObject* co = mGroundPlaneSimState->GetCollisionObject();
    co->PostManualUpdate();
}



/*
==============================================================================
Character::PreSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::PreSimUpdate( float timeins )
{
#ifdef RAD_DEBUG
    timeins *= timeScale;
#endif

    if( !IsNPC() ) // ok, we're player character
    {
        UpdateGroundPlane( timeins );
    }

    /////////////////////////////////////////////////
    // Test if we're too far from the camera for some updates
    mTooFarToUpdate = false;

    // if we're in First Person Cam, this is bad news... They can ZOOM!
    // So treat it as though we're ALWAYS not too far to update.
    if( GetInputManager()->GetGameState() != Input::ACTIVE_FIRST_PERSON )
    {
        rmt::Vector myPos;
        GetPosition( myPos );

        rmt::Vector testPos;
        GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( testPos );
        /*
        // NOTE:
        // Can't use the camera pos because we could get quite far from the 
        // cam.. use the avatar's pos
        GetSuperCamManager()->GetSCC(0)->GetCamera()->GetPosition( &testPos );
        */
        float fov, aspect;
        GetSuperCamManager()->GetSCC(0)->GetCamera()->GetFOV(&fov, &aspect);
        static float fovCutoff = 1.0f;

        const float DIST_TOO_FAR_TO_UPDATE_SQR = 625.0f;

        float distSqr = (myPos - testPos).MagnitudeSqr();
        if( distSqr > DIST_TOO_FAR_TO_UPDATE_SQR && (fov > fovCutoff))
        {
            mTooFarToUpdate = true;
        }
    }

    //////////////////////////////////////////////////////
    // Sense and think.
    //
    rmt::Vector direction;
    UpdateController( direction, timeins );
    if ( IsMovementLocked() == false )
    {
        UpdateDesiredDirAndSpeed( direction );
    }
    else
    {
        UpdateDesiredDirAndSpeed( rmt::Vector(0,0,0) );
    }



    if( mpSimStateObj->GetControl() == sim::simSimulationCtrl && 
        GetStateManager()->GetState() != CharacterAi::INSIM )
    {
        // this will cause flail & get-up anims to be sequenced
        GetStateManager()->SetState<CharacterAi::InSim>(); 
    }


    ////////////////////////////////////////////////////////
    // Selectively update action controller every n frames if:
    //  - not in view, or
    //  - too far away

    unsigned int modulo = 0x7;
    unsigned int frameCount = GetGame()->GetFrameCount();

    mSecondsSinceActionControllerUpdate += timeins;
    
    //chuck: changed the herusitic to also include drivers in cars
    //since we dont want driver pop a mission restarts
    if(        
                !mIsNPC 
            || 
                (mbInAnyonesFrustrum) 
            || 
                ((frameCount & modulo) == (mIntersectFrame & modulo)
            || 
                (mpTargetVehicle != NULL)
            
       ) 
        
        
        
        )
    {
    BEGIN_PROFILE("ActionController()->Update")
        GetActionController()->Update( mSecondsSinceActionControllerUpdate );
    END_PROFILE("ActionController()->Update")

        // Execute intentions based on current state
        mpStateManager->Update( mSecondsSinceActionControllerUpdate );

        // Do simulation
        GetActionController()->WakeUp( mSecondsSinceActionControllerUpdate );
        GetActionController()->DoSimulation( mSecondsSinceActionControllerUpdate );

        mSecondsSinceActionControllerUpdate = 0.0f;
    }
    ////////////////////////////////////////////////////////

    

    // Reset last frame's collisions in preparation for new collision data 
    // (given unto me by WorldPhysMan's substep)
    ResetCollisions( );

    mpPuppet->UpdateBegin();

    // Zero out the prophandler.  If it is still valid it will get set 
    // during CollisioDetect.
    //
//    mpPropHandler->SetProp( 0 );
}

/*
==============================================================================
Character::ResetCollisions
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::ResetCollisions( void )
{
    // If the thing has moved, then there will be new collision state info.
    // If NOT then we don't reset our collision state info, we will reuse it.
    //
 //   if ( mpSimStateObj->GetCollisionObject( )->HasMoved() )
    {
        // Reset everything after this code, and before PostSimUpdate
        // is when we do the collision detection.
        //
        mbCollided = false;
        int i;
        for ( i = 0; i < CollisionData::MAX_COLLISIONS; i++ )
        {
            mCollisionData[ i ].mCollisionNormal.Set( 0.0f, 0.0f, 0.0f );
            mCollisionData[ i ].mCollisionDistance = 0.0f;
            mCollisionData[ i ].mpCollisionVolume = (sim::CollisionVolume*)0;
        }
        mCurrentCollision = 0;
    }

    mbCollidedWithVehicle = false;

}
/*
==============================================================================
Character::UpdateController
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& direction, float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdateController( rmt::Vector& direction, float timeins )
{

    if ( GetController( ) )
    {
        GetController( )->Update( timeins );
        GetController( )->GetDirection( direction );
    }
    else
    {
        direction.Set( 0.0f, 0.0f, 0.0f );
    }
}
/*
==============================================================================
Character::UpdateDesiredDirAndSpeed
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& dir )

Return:         void 

=============================================================================
*/
void Character::UpdateDesiredDirAndSpeed( const rmt::Vector& dir )
{
    rmt::Vector direction = dir;

#ifdef RAD_WIN32
    SuperCam* cam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    PCCam* pPCCam = NULL;
    if( !mIsNPC && cam->GetType() == SuperCam::PC_CAM )  
    {
        pPCCam = static_cast<PCCam*>(cam);
    }
#endif

#ifndef RAD_WIN32
    if ( direction.DotProduct( direction ) > 0.001f )
    {
        SetDesiredDir( choreo::GetWorldAngle( direction.x, direction.z ) );
    }
#else
    if( !mIsNPC && pPCCam )  
    {
        pPCCam->SetPitchVelocity( direction.x );

        // if input is given in either z-axis (forward and back) or x-axis (left and right)
        //
        rAssert( dynamic_cast<CameraRelativeCharacterController*>( GetController() ) );
        CharacterMappable* map = ((CameraRelativeCharacterController*)this->GetController())->GetCharacterMappable();

        float dirPadZ = map->GetValue( CharacterController::DPadUp ) - map->GetValue( CharacterController::DPadDown );
        float dirAnalogZ = map->GetValue( CharacterController::LeftStickY );
        float dirZ = rmt::Fabs( dirPadZ ) > rmt::Fabs( dirAnalogZ ) ? dirPadZ : dirAnalogZ;

        float dirPadX = map->GetValue( CharacterController::DPadRight ) - map->GetValue( CharacterController::DPadLeft );
        float dirAnalogX = map->GetValue( CharacterController::LeftStickX );
        float dirX = rmt::Fabs( dirPadX ) > rmt::Fabs( dirAnalogX ) ? dirPadX : dirAnalogX;

        /*
        float currAngle = GetFacingDir();

        if ( !rmt::Epsilon( dirX, 0, 0.01f ) || !rmt::Epsilon( dirZ, 0, 0.01f ) )
        {
            rmt::Vector camHeading;
            cam->GetHeading( &camHeading );
            camHeading.y = 0.0f;

            rmt::Vector tmpHeading;

            rmt::Matrix mat;
            mat.Identity();
            mat.FillHeading( camHeading, rmt::Vector( 0.0f, 1.0f, 0.0f ) );
            tmpHeading.Set( dirX, 0.0f, dirZ );
            tmpHeading.Transform( mat );

            if( rmt::Epsilon( dirX, 0.0f ) )
            {
                if ( dirZ > 0.0f )
                {
                    mPCCamFacing = 0;
                }
                else
                {
                    mPCCamFacing = 1;
                }
            }
            else
            {
                mPCCamFacing = 2;
            }

            mInvParentTransform.RotateVector( tmpHeading, &tmpHeading );
            currAngle = choreo::GetWorldAngle( tmpHeading.x, tmpHeading.z );
            SetFacingDir( currAngle );

            direction.x = dirX;
        }
        else
        {
            // Upon no input, face the character in the direction of cam
            //rmt::Vector camHeading;
            //cam->GetHeading( &camHeading );
            //camHeading.y = 0.0f;

            // reset currAngle and modify the character's facing directly.
            //currAngle = choreo::GetWorldAngle( camHeading.x, camHeading.z );
            //SetFacingDir( currAngle );

            //direction.Set( 0.0f, 0.0f, 0.0f );

            //mPCCamFacing = 0;
        }

        static float X_SENSE_MOD = 0.01f;
        float mouseSense = static_cast<Mouse*>(GetInputManager()->GetController( 0 )->GetRealController( MOUSE ))->getSensitivityX();

        static float ROT_DIR = 4.0f;
        
        float fPitchVelocity = pPCCam->GetPitchVelocity();
        float fScaleFactor = 1.0f;
        if( mbTurbo ) fScaleFactor *= mVelocity.Magnitude();
        
        currAngle += ROT_DIR * mouseSense * X_SENSE_MOD * pPCCam->GetAngularSpeed() * pPCCam->GetPitchVelocity();
        
        SetDesiredDir( currAngle );
        */

        if ( !rmt::Epsilon( dirX, 0, 0.01f ) || !rmt::Epsilon( dirZ, 0, 0.01f ) )
        {
            rmt::Vector camHeading;
            cam->GetHeading( &camHeading );
            camHeading.y = 0.0f;

            rmt::Vector tmpHeading;

            rmt::Matrix mat;
            mat.Identity();
            mat.FillHeading( camHeading, rmt::Vector( 0.0f, 1.0f, 0.0f ) );
            tmpHeading.Set( dirX, 0.0f, dirZ );
            tmpHeading.Transform( mat );

            if( rmt::Epsilon( dirX, 0.0f ) )
            {
                if ( dirZ > 0.0f )
                {
                    mPCCamFacing = 0;
                }
                else
                {
                    mPCCamFacing = 1;
                }
            }
            else
            {
                mPCCamFacing = 2;
            }

            mInvParentTransform.RotateVector( tmpHeading, &tmpHeading );
            float currAngle = choreo::GetWorldAngle( tmpHeading.x, tmpHeading.z );
            SetDesiredDir( currAngle );

            direction.x = dirX;
        }
    }
    else // We reached here cuz we're either an NPC or we're not in PC_CAM
    {
        if ( direction.DotProduct( direction ) > 0.001f )
        {
            SetDesiredDir( choreo::GetWorldAngle( direction.x, direction.z ) );
        }
    }
#endif

    float fDesiredSpeed = direction.Magnitude();

    // apply non-linear response to input to Player Character only..
    // the NPCs rely on desired speed to remain unaltered 
    // for the intended speed to be reached.
    if( !mIsNPC )
    {
        fDesiredSpeed *= fDesiredSpeed;
    }

    // Now normalize the speed to a smaller range than 1.0, probably something like 0.84f
    //
    float fMaxScale = GetInputScale( );
    fDesiredSpeed /= fMaxScale;
    if ( fDesiredSpeed > 1.0f )
    {
        fDesiredSpeed = 1.0f;
    }
    SetDesiredSpeed( fDesiredSpeed * GetMaxSpeed( ) );
}
/*
==============================================================================
Character::UpdateRoot
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdateRoot( float timeins )
{
    OnUpdateRoot( timeins );
}

/*
==============================================================================
Character::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::OnUpdateRoot( float timeins )
{
#ifdef RAD_DEBUG
    timeins *= timeScale;
#endif

    UpdatePuppet( timeins );
}
/*
==============================================================================
Character::UpdatePuppet
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdatePuppet( float timeins )
{
    choreo::Puppet* pPuppet = GetPuppet( );
    rAssert( pPuppet );
    // post sim inserted here
    UpdateParentTransform( timeins );

    mbNeedChoreoUpdate = !(
        (pPuppet->GetEngine()->GetRootBlender()->GetRootDriverCount() <= 1) 
        && (!mVisible || !mbInAnyonesFrustrum) 
        && mbSimpleLoco);

    if(pPuppet->GetDriverCount() == 0)
    {
//        rDebugPrintf("WARNING : Character '%s' has no pose drivers, holding last pose\n", GetName());
        mbNeedChoreoUpdate = false;
    }

    if(!mbNeedChoreoUpdate && mbSimpleLoco)
    {
        pPuppet->GetEngine()->GetRootBlender()->ClearRootDrivers();

        rmt::Vector position = pPuppet->GetPosition();
        rAssert( mpWalkerLocomotion != NULL );
        choreo::LocomotionDriver* locoDriver = mpWalkerLocomotion->GetDriver();
        rAssert( locoDriver != NULL );
        float velocity = locoDriver->GetDesiredVelocity();
      
        rmt::Vector facing;
        this->GetFacing(facing);

        facing *= velocity * timeins;
        pPuppet->SetPosition(position + facing);

        pPuppet->UpdateRoot();
    }
    else
    {
//        if(pPuppet->GetDriverCount() != 0)
        {
            // Update choreo.
            //
            // Push() all drivers before call to choreo::Puppet::Begin()
            //
            pPuppet->Advance( timeins );

            // choreo::Puppet::UpdateRoot() will change choreo::Puppet Position and Orientation
            //
            pPuppet->UpdateRoot();
        }
    }
}

bool Character::TestInAnyonesFrustrum()
{
    ////////////////////////////////////////////////
    // Test if we're in anyone's frustrum
    //
    mbInAnyonesFrustrum = false;
    int playerCount = 0;
    int numPlayers = ::GetGameplayManager()->GetNumPlayers();
    while( !mbInAnyonesFrustrum && playerCount < numPlayers )
    {
        TestInFrustrumOfPlayer(playerCount);
        playerCount++;
    }
    if( mpCharacterRenderable != NULL )
    {
        // characterrenderable doesn't have pointer to character, so 
        // store result there too...
        mpCharacterRenderable->SetInAnyonesFrustrum( mbInAnyonesFrustrum );
    }
    return mbInAnyonesFrustrum;
}


/*
==============================================================================
Character::PostSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::PostSimUpdate( float timeins )
{
    TestInAnyonesFrustrum();

#ifdef RAD_DEBUG
    timeins *= timeScale;
#endif

    // determine how frequently we need to make some of the expensive calls
    unsigned int modulo = 0x7;
    unsigned int frameCount = GetGame()->GetFrameCount();
    bool shouldUpdate = !mIsNPC || // update the PC at full rate
                        IsInCar() || // or parent transform will get screwed
                        (mbInAnyonesFrustrum) || // update if we are visible
                        ((frameCount & modulo) == (mIntersectFrame & modulo));

BEGIN_PROFILE("OnPostSimUpdate")
    mSecondsSinceOnPostSimUpdate += timeins;

    if( shouldUpdate )
    {
        OnPostSimUpdate( mSecondsSinceOnPostSimUpdate );
        mSecondsSinceOnPostSimUpdate = 0.0f;
    }

    mbNeedChoreoUpdate = mbNeedChoreoUpdate && shouldUpdate && (!mTooFarToUpdate || ((frameCount & modulo) == (mIntersectFrame & modulo)));

END_PROFILE("OnPostSimUpdate")

    // Call physics update.
    // Needs to be called before UpdatePose in case we're in SimulationCtrl 
BEGIN_PROFILE("UpdateSimState")
    UpdateSimState( timeins );
END_PROFILE("UpdateSimState")

    choreo::Puppet* pPuppet = GetPuppet( );
    rAssert( pPuppet );

    ResolveCollisions();

    BEGIN_PROFILE("UpdateGroundHeight")
    if ( !IsInCar( ) )
    {
        UpdateGroundHeight();
    }
END_PROFILE("UpdateGroundHeight")
 
    // Update the puppet with the physics transform?
    //
/*
BEGIN_PROFILE("UpdateProps")
    UpdateProps( timeins );
END_PROFILE("UpdateProps")
*/

    if ( GetController() )
    {
        GetController()->ClearIntention();
    }

    if(IsNPC())
    {
        if(mAmbientTrigger)
        {
            rmt::Vector pos;
            GetPosition(pos);
            mAmbientTrigger->SetPosition(pos);
        }
    }
    else
    {
        /*
        // Clear the action handler when we are set to handle a prop
        // that has been previously cleared.
        //
BEGIN_PROFILE("GetActionButtonHandler")
        if( mpPropHandler->GetProp() == 0 )
        {
            mpPropHandler->Exit( this );
            RemoveActionButtonHandler( mpPropHandler );
        }
END_PROFILE("GetActionButtonHandler")
        */

BEGIN_PROFILE("UpdateFootPlant")
        if( !IsInCar() )
        {
            if(  radTimeGetMicroseconds64()-mLastInteriorLoadCheck > 3000000 ) //(amortise/3s) if it's been more than 3 sec's since we last checked for volumes
            {
                rmt::Vector posn;
                GetPosition(&posn);
                GetTriggerVolumeTracker()->ActivateNearestInteriorLoadZone(0, posn, 40.0f);
                mLastInteriorLoadCheck = radTimeGetMicroseconds64();
            }

            UpdateFootPlant( );
        }
END_PROFILE("UpdateFootPlant")

        UpdateShock( timeins );

        if(mDoKickwave && mKickwaveController)
        {
            mKickwaveController->Advance(timeins * 1000.0f);
            if(mKickwaveController->LastFrameReached())
            {
                mDoKickwave = false;
            }
        }
    }

    rmt::Vector tmp, tmpDir;
    GetPosition(tmp);
    tmpDir.Sub(tmp, mLastFramePos);
    mVelocity = tmpDir / timeins;

    // if movement is too great while not in car, reset velocity & reset position to last frame's pos
    // this is to stop the player character from going out of world or warping too high in the sky, 
    // or too low below ground, etc...

    const float DIFF_SQR_IN_POS_TOO_HIGH_RESET = 2500.0f;

    CharacterAi::CharacterState state = GetStateManager()->GetState();

    if( !mIsNPC &&
        ( state == CharacterAi::LOCO || state == CharacterAi::INSIM )&&
        ( tmpDir.MagnitudeSqr() > DIFF_SQR_IN_POS_TOO_HIGH_RESET ) )
    {   
        //rAssertMsg( false, "Player character got moved REALLY far in one frame... Using last frame pos!\n" );
        mVelocity.Set( 0.0f, 0.0f, 0.0f );
        SetPosition( mLastFramePos );
        mpJumpLocomotion->SetRootPosition( WorldToLocal( mLastFramePos ) );
        this->SetGroundPoint( mLastFramePos );
    }
    else
    {
        if(mCollisionAreaIndex != -1)
        {
            if(GetWorldPhysicsManager()->FenceSanityCheck(mCollisionAreaIndex, mLastFramePos, tmp, &tmp))
            {
                SetPosition( tmp );
                mpJumpLocomotion->SetRootPosition( WorldToLocal( tmp ) );
                this->SetGroundPoint( tmp );
            }
        }

        mLastFramePos = tmp;
    }

BEGIN_PROFILE("MoveInWorldScene")
    MoveInWorldScene();  
END_PROFILE("MoveInWorldScene")
}

void Character::ResolveCollisions(void)
{
    mCollidedThisFrame = false;
    sim::SimState* simState = mpSimStateObj; //GetSimState();
    if( simState != NULL )
    {
        if( simState->GetControl() == sim::simAICtrl )
        {
            bool inSR1Or2 = false;
            Mission* m = GetGameplayManager()->GetCurrentMission();
            if( m )
            {
                inSR1Or2 = m->mIsStreetRace1Or2;
            }
            bool shouldSolveCollisions = !IsInCar() && 
                (!mIsNPC || (mIsNPC && !inSR1Or2));


            if( shouldSolveCollisions )
            {
                rmt::Vector desiredPos = LocalToWorld( mpPuppet->GetPosition( ) );
                rmt::Vector outPos;
BEGIN_PROFILE("SolveCollisionWithStatic")
                Character::eCollisionType collisionType = SolveCollisionWithStatic( desiredPos, outPos );
END_PROFILE("SolveCollisionWithStatic")
                
                if( collisionType & Character::HitHead )
                {
                    if(mpJumpLocomotion->IsJumpState(JumpAction::Jump))
                    {
                        GetEventManager()->TriggerEvent(EVENT_HIT_HEAD, this);

                        if(!mbIsStanding && (mVelocity.y > 0.0f))
                        {
                            mpJumpLocomotion->Reset(0.0f, true);

                            if(!IsNPC())
                            {
                                outPos.y -= 0.01f;
                            }
                        }
                    }
                }

                if ( collisionType & ( Character::HitWall | Character::HitHead) )
                {
                    // Fix the character position.
                    //
                    SetPosition( outPos );
                    mpJumpLocomotion->SetRootPosition( WorldToLocal( outPos ) );
                    this->SetGroundPoint( outPos);
                    mCollidedThisFrame = true;
                }

            }
        }
    }
}

/*
==============================================================================
Character::UpdateFootPlant
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::UpdateFootPlant( void )
{
    int legs = mpPuppet->GetLegCount( );
    for ( int i = 0; i < legs; i++ )
    {
        bool bFootPlanted = mpPuppet->IsFootPlanted( i );
        if ( bFootPlanted )
        {
            if ( mbWasFootPlanted[ i ] == false )
            {
                // foot planted.
                //

                // If were are dashing around (turbo) and the foot just made
                // contact with the ground, kick up a dust cloud
                // also make sure the user isnt just holding in the turbo button while
                // the character is just standing still
                //


                if ( GetDesiredSpeed() > 1.0f )
                {
                    /* footprints
                    rmt::Matrix transform;
                    transform.Identity();
                    transform.FillTranslate( mpPuppet->GetFootPosition( i ) );
                    
                    GetFootprintManager()->CreateFootprint( transform, FootprintManager::eSquishies );
                    */

                    // no puffs in interior 
                    if(!GetInteriorManager()->IsInside())
                    {
                        rmt::Vector facing;
                        this->GetFacing( facing );
                        GetSparkleManager()->AddDash( mpPuppet->GetFootPosition( i ), facing, GetDesiredSpeed() * 0.125f );
                    }

                    if(this == GetCharacterManager()->GetCharacter(0))
                    {
                        GetEventManager()->TriggerEvent( EVENT_FOOTSTEP, this );
                    }
                }
           }
        }
        mbWasFootPlanted[ i ] = bFootPlanted;
    }
}
/*
==============================================================================
Character::OnPostSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::OnPostSimUpdate( float timeins )
{
}

/*
==============================================================================
Character::UpdateShock
==============================================================================
Description:    Comment

Parameters:     ( float delta time in seconds )

Return:         void 

=============================================================================
*/
void Character::UpdateShock( float timeins )
{
    if ( m_IsBeingShocked )
    {
      //  UpdateDesiredDirAndSpeed( rmt::Vector( 0,0,0 ) );
        m_TimeLeftToShock -= timeins;

        if ( IsNPC() == false )
        {
            float blur = m_TimeLeftToShock * 2.0f;
            if ( blur > 1.0f ) blur = 1.0f;
            GetRenderManager()->SetBlurAlpha( blur );
        }
        if ( m_TimeLeftToShock <= 0 )
        {
            mpCharacterRenderable->SetShocked( false );
            m_IsBeingShocked = false;
        }
    }
    else
    {
        if ( IsNPC() == false )
            GetRenderManager()->SetBlurAlpha( 0 );
    }
}


/*
==============================================================================
Character::AddToWorldScene
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::AddToWorldScene( void )
{
    if( !mVisible && mManaged )
    {
        UpdatePuppet( 0.0f );

        choreo::Puppet* pPuppet = GetPuppet( );
        rAssert( pPuppet );

        pPuppet->UpdatePose();

        if(mpCharacterRenderable->GetDrawable())
        {
            mpPuppet->GetP3DPose()->SetSkeleton(mpCharacterRenderable->GetDrawable()->GetSkeleton());
            pPuppet->UpdateEnd();
        }

        UpdateSimState( 0.0f );

        // Call updateBBox so the DSG knows where to put this guy at the start.
        rmt::Box3D dummyBox;
        UpdateBBox( dummyBox );

        rAssert( mpWorldScene == 0 );
        mpWorldScene = ((WorldRenderLayer*)GetRenderManager()->mpLayer(RenderEnums::LevelSlot))->pWorldScene();
        mpWorldScene->Add( this );

        //UpdateProps( 0.0f );

        mVisible = true;
    }
}
/*
==============================================================================
Character::RemoveFromWorldScene
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         

=============================================================================
*/
void Character::RemoveFromWorldScene( void )
{
    if( mVisible )
    {
        mpWorldScene->Remove( this );
        mpWorldScene = 0;
        mVisible = false;
    }
}



void Character::MoveInWorldScene( void )
{
    if( mVisible )
    {
        rmt::Box3D oldBox;
        UpdateBBox( oldBox );
        // now move!
        //
        mpWorldScene->Move(oldBox, (IEntityDSG*)this);
    }
}

/*
==============================================================================
Character::UpdateSimState
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdateSimState( float timeins )
{
    sim::SimState* simState = mpSimStateObj; //GetSimState();
    if( simState != 0 )
    {
        if( simState->GetControl() == sim::simAICtrl )
        {
            /*
            poser::Transform poserTrans = mpPuppet->GetRootTransform();
            rmt::Matrix matrix = poserTrans.GetMatrix();
            simState->SetTransform( matrix, timeins );
            */

            rmt::Matrix matrix;
            rmt::Vector facing;
            rmt::Vector side;
            rmt::Vector position;

            GetFacing(facing);
            GetPosition( position );

            side.CrossProduct(facing, rmt::Vector(0.0f, 1.0f, 0.0f));

            // leaning while in jump or when doing idle is *very* likely to push us through stuff
            bool dontLean = GetJumpLocomotionAction()->IsInJump() || mbIsPlayingIdleAnim;

            if(!dontLean)
            {
                dontLean = mLean.Dot(rmt::Vector(0.0f, 1.0f, 0.0f)) > 0.925;
            }

            matrix.Identity();
            matrix.Row(0) = side;
            matrix.Row(1) = dontLean ? rmt::Vector(0.0f, 1.0f, 0.0f) : mLean;
            matrix.Row(2).CrossProduct(side, mLean);
            matrix.Row(3) = position;

            rmt::Matrix oldMat = simState->GetTransform( );
            simState->SetTransform( matrix, timeins );
            simState->GetCollisionObject()->Update();

            // TODO: 
            // Do we really need to update velocity here??
            rmt::Vector velXZ = simState->VelocityState().mLinear;
            velXZ.y = 0.0f;
            mfSpeed = velXZ.Magnitude( );
        }
        else if( simState->GetControl() == sim::simSimulationCtrl )
        {
            // If simstate velocities are too great, fudge them here. We don't want characters flying too far away...
            rmt::Vector vel = simState->VelocityState().mLinear;
            float limit = 15.0f;
            rAssert( limit >= 0.0f );
            float linearSpdMps = vel.Length(); // *** SQUARE ROOT! ***
            if( linearSpdMps > limit )
            {
                simState->VelocityState().mLinear.Scale( limit/linearSpdMps );
            }

            // Update Character's speed
            rmt::Vector velXZ = simState->VelocityState().mLinear;
            velXZ.y = 0.0f;
            mfSpeed = velXZ.Magnitude( );

            // Update Character's facing and position
            rmt::Matrix matrix = (rmt::Matrix) simState->GetTransform();
            rmt::Vector negZed( 0.0f, 0.0f, -1.0f );

            poser::Transform transform( matrix );
            mpPuppet->SetRootTransform( transform );

            poser::Pose* pPose = mpPuppet->GetPose( );
            // stuff fixed up root transform into joint
            poser::Joint* joint = pPose->GetJoint( 0 );
            joint->SetWorldTransform( transform );
            joint->SetObjectTransform( transform );
        }
    }
}

/*
==============================================================================
Character::UpdateBBox
==============================================================================
Description:    Comment

Parameters:     ( rmt::Box3D& oldBox )

Return:         void 

=============================================================================
*/
void Character::UpdateBBox( rmt::Box3D& oldBox )
{
    oldBox = mBBox;

    GetPosition( mPosn );   

    mBBox.low   = mPosn;
    mBBox.high  = mPosn;

    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre.Sub(mBBox.high,mBBox.low);
    mSphere.centre *= 0.5f;
    mSphere.centre.Add(mBBox.low);
    mSphere.radius = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mSphereRadius;
}
// Implements CollisionEntityDSG
//
/*
==============================================================================
Character::PreReactToCollision
==============================================================================
Description:    Comment

Parameters:     ( sim::SimState* pCollidedObj, sim::Collision& inCollision )

Return:         bool 

=============================================================================
*/
sim::Solving_Answer Character::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
    if((inCollision.mCollisionVolumeA->Type() == sim::BBoxVolumeType) || 
       (inCollision.mCollisionVolumeB->Type() == sim::BBoxVolumeType))
    {
        return sim::Solving_Continue;
    }

    ////////////////////////////////////////////
    // Deal properly with ignoring targetvehicle pointer
    //
    if( this->mpTargetVehicle && (pCollidedObj->mAIRefPointer == this->mpTargetVehicle))
    {
        return sim::Solving_Aborted;
    }

    if( this->mpTargetVehicle && this->mpTargetVehicle->mVehicleDestroyed)
    {
        if(pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
        {
            Vehicle* v = (Vehicle*)pCollidedObj->mAIRefPointer;
            if(v->mVehicleID == VehicleEnum::HUSKA)
            {
                Vehicle* ov = GetVehicleCentral()->mHuskPool.FindOriginalVehicleGivenHusk(v);
                if(ov && (ov == this->mpTargetVehicle))
                {
                    return sim::Solving_Aborted;
                }
            }
        }
    }

    if( this->mpTargetVehicle && this->mpTargetVehicle->GetDriver() && 
        (pCollidedObj->mAIRefPointer == this->mpTargetVehicle->GetDriver()))
    {
        return sim::Solving_Aborted;
    }
    /////////////////////////////////////////

    // Remember that we've legitimately collided with a vehicle
    if( pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickVehicle )
    {
        mbCollidedWithVehicle = true;
    }

    // Don't store collision data if we're under simulation control
    if( mpSimStateObj->GetControl() == sim::simSimulationCtrl )
    {
        // When a traffic vehicle hits an NPC (or vice versa) while in simulation control...
        if( (pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickVehicle) &&
            (mpSimStateObj->mAIRefIndex == PhysicsAIRef::NPCharacter) && 
            (GetStateManager()->GetState() == CharacterAi::LOCO || GetStateManager()->GetState() == CharacterAi::INSIM) )
        {

            sim::Collision theCollision = inCollision;
            sim::SimState* pSimState = theCollision.mCollisionObjectB->GetSimState();
            if( pSimState->mAIRefPointer == this && 
                (pSimState->mAIRefIndex == PhysicsAIRef::NPCharacter || 
                pSimState->mAIRefIndex == PhysicsAIRef::PlayerCharacter) )
            {
                theCollision.mNormal.Scale( -1.0f );
                if(theCollision.GetPositionB().y - mGroundY < 0.05f)
                {
                    return sim::Solving_Continue;
                }
            }
            else
            {
                if(theCollision.GetPositionA().y - mGroundY < 0.05f)
                {
                    return sim::Solving_Continue;
                }
            }

            // Grab vehicle's speed and if it's greater than threshold, 
            // transit to simulation control.
            Vehicle* v = (Vehicle*) pCollidedObj->mAIRefPointer;
            rAssert( v );

            float vSpeedMps = v->mSpeed;
            float speedThreshold = 1.0f;
            if( vSpeedMps > speedThreshold && v->mVehicleType == VT_TRAFFIC )
            {
                /*
                const int MAX_RAND_MODULUS = 3;
                const float THEORETICAL_MAX_SPEED_MPS = 28.0f;

                float speedRatio = v->mSpeed / THEORETICAL_MAX_SPEED_MPS;

                const float BASE_SPEED_COMPONENT_MOD_MPS = 2.0f;

                // Impart immediate vertical velocity! YA!
                float randX, randY, randZ;
                randX = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;
                randY = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;
                randZ = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;

                rmt::Vector impactLinearVel( 0.0f, randY, 0.0f );
                rmt::Vector impactAngularVel( randX, randY, randZ);

                impactLinearVel += v->GetFacing() + theCollision.mNormal;
                */
                rmt::Vector impactLinearVel = theCollision.mNormal * 1.4f;


                rmt::Vector& linearVel = mpSimStateObj->GetLinearVelocity();
                linearVel.Add( impactLinearVel );
                //rmt::Vector& angularVel = mpSimStateObj->GetAngularVelocity();
                //angularVel.Add( impactAngularVel );
            }
        }
        return sim::Solving_Continue;
    }

    //rAssert( mCurrentCollision < CollisionData::MAX_COLLISIONS );
    if( mCurrentCollision >= CollisionData::MAX_COLLISIONS )
    {
        return sim::Solving_Continue;//false;
    }

    if( //mpSimStateObj->mAIRefIndex == PhysicsAIRef::PlayerCharacter &&
        pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane )
    {
        return sim::Solving_Aborted;
    }

    // 
    // store some value for when player character (me) collides with another character
    //
    if( mpSimStateObj->mAIRefIndex == PhysicsAIRef::PlayerCharacter &&
        pCollidedObj->mAIRefIndex == PhysicsAIRef::NPCharacter )
    {
        GetEventManager()->TriggerEvent( EVENT_PC_NPC_COLLISION, pCollidedObj->mAIRefPointer );
    }
    static bool bTestNormals = true;
    static float sfEdgeTune = 0.6f;
    sim::Collision theCollision = inCollision;
    // recall:
    // mPositionA = mPositionB + mNormal * mDistance
    // We want the normal to point from Object to Homer.
    // Normals always point from B to A.
    // So, if homer is B, then flip the normal.
    //

    sim::SimState* pSimState = theCollision.mCollisionObjectB->GetSimState();
    if( pSimState->mAIRefPointer == this && 
        (pSimState->mAIRefIndex == PhysicsAIRef::NPCharacter || 
         pSimState->mAIRefIndex == PhysicsAIRef::PlayerCharacter) )
    {
        theCollision.mNormal.Scale( -1.0f );
        if(theCollision.GetPositionB().y - mGroundY < 0.05f)
        {
            return sim::Solving_Continue;
        }
    }
    else
    {
        if(theCollision.GetPositionA().y - mGroundY < 0.05f)
        {
            return sim::Solving_Continue;
        }
    }

    if ( theCollision.mCollisionVolumeA == mpStandingCollisionVolume
        || theCollision.mCollisionVolumeB == mpStandingCollisionVolume )
    {
        return sim::Solving_Continue;//false;
    }

    // When a vehicle hits us (or vice versa), transit to simulation control 
    // so physics take over if we are in locomiotion (for now, don't do this 
    // if we are not in loco (i.e. getting in or out of car) or state can get 
    // badly screwed 
    if( (pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickVehicle) && 
        (GetStateManager()->GetState() == CharacterAi::LOCO ||
        GetStateManager()->GetState() == CharacterAi::INSIM) )
    {
        // Grab vehicle's speed and if it's greater than threshold, 
        // transit to simulation control.
        Vehicle* v = (Vehicle*) pCollidedObj->mAIRefPointer;
        rAssert( v );

        float vSpeedMps = v->mSpeed;

        float speedThreshold = 1.0f;
        if( vSpeedMps > speedThreshold )
        {
            if( mpSimStateObj->mAIRefIndex == PhysicsAIRef::NPCharacter )
            {
                if( v == GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle() )
                {
                    GetEventManager()->TriggerEvent( EVENT_PLAYER_MAKES_LIGHT_OF_CAR_HITTING_NPC );
                    GetEventManager()->TriggerEvent( EVENT_PLAYER_CAR_HIT_NPC, this );
                    mHasBeenHit = true;
                }

                /////////////////////////////
                // We're entering simulation 
                /////////////////////////////
                mpSimStateObj->SetControl( sim::simSimulationCtrl );
                GetWorldPhysicsManager()->EnableGroundPlaneCollision(mGroundPlaneIndex);


                // If a traffic vehicle hit us, its simstate will have zero velocity, so
                // it won't be imparting anything upon us... So we fake it...
                if( v->mVehicleType == VT_TRAFFIC )
                {

                    //const int MIN_RAND_MODULUS = 1;
                    const int MAX_RAND_MODULUS = 6;
                    const float THEORETICAL_MAX_SPEED_MPS = 28.0f;

                    float speedRatio = v->mSpeed / THEORETICAL_MAX_SPEED_MPS;

                    //int delta = MAX_RAND_MODULUS - MIN_RAND_MODULUS;
                    //int randMod = (int)(speedRatio * delta) + MIN_RAND_MODULUS;
                    //rAssert( randMod >= MIN_RAND_MODULUS );

                    const float BASE_SPEED_COMPONENT_MOD_MPS = 6.0f;

                    // Impart immediate vertical velocity! YA!
                    float randX, randY, randZ;
                    randX = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;
                    randY = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;
                    randZ = ((float)(rand()%MAX_RAND_MODULUS) + BASE_SPEED_COMPONENT_MOD_MPS) * speedRatio;

                    rmt::Vector impactLinearVel( 0.0f, randY, 0.0f );
                    rmt::Vector impactAngularVel( randX, randY, randZ);

                    impactLinearVel += v->GetFacing() + theCollision.mNormal;


                    rmt::Vector& linearVel = mpSimStateObj->GetLinearVelocity();
                    linearVel.Add( impactLinearVel );
                    rmt::Vector& angularVel = mpSimStateObj->GetAngularVelocity();
                    angularVel.Add( impactAngularVel );
                }

            }
            else // it's the player character that's getting hit by a vehicle
            {
                // Ignore all other cars except VT_AI cars
                // need to break out and keep solving though
                if(((Vehicle*)pCollidedObj->mAIRefPointer)->mVehicleType != VT_AI )
                {
                    goto KeepSolving;
                }

                if( !IsInCar() )
                {

                    // ignore "up" collision normals (presumably we're standing on top 
                    // of a traffic car)
                    const float TOP_OF_VEHICLE_COS_ALPHA = 0.9848077f; // approx 10 degrees
                    float dp = theCollision.mNormal.Dot( mRealGroundNormal );

                    // if deviate by > 10 degrees from ground normal, then we're 
                    // probably not standing on it.
                    if( dp < TOP_OF_VEHICLE_COS_ALPHA )
                    {

                        /////////////////////////////
                        // We're entering simulation 
                        /////////////////////////////

                        // Transit to Simulation control and set up its ground plane... 
                        // Didn't need to be done for NPCs cuz they would have been 
                        // submitted by other dynamics when hit (and would have obtained 
                        // their ground plane at that point)
                        AddToSimulation();

                        rAssert( mGroundPlaneSimState );
                        mGroundPlaneSimState->GetCollisionObject()->SetCollisionEnabled( true );

                        GameplayManager* gameplayManager = GetGameplayManager();
                        if ( gameplayManager != NULL )
                        {
                            ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                            if ( chaseManager != NULL )
                            {
                                if(v->mName)
                                {
                                    if(chaseManager->IsModelRegistered(v->mName))
                                    {
                                        GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_CAUGHT, NULL );
                                    }
                                }
                            }
                        }
                    }
                }
            }
//            return sim::Solving_Continue;
        }
    }

KeepSolving:

    if ( bTestNormals )
    {
        for ( int i = 0; i < mCurrentCollision; i++ )
        {   
            // Test each collision normal to see if it is a duplicate of a collision we 
            // have already stored.
            //
            if( ( mCollisionData[ i ].mpCollisionVolume == theCollision.mCollisionVolumeA ||
                  mCollisionData[ i ].mpCollisionVolume == theCollision.mCollisionVolumeB ) )
            {
                rAssert( mbCollided );

                rmt::Vector facing;
                GetFacing(facing);

                bool store = ( mCollisionData[ i ].mCollisionNormal.DotProduct(facing) > theCollision.mNormal.DotProduct(facing));

                if(store)
                {
                    mCollisionData[ i ].mCollisionDistance = theCollision.mDistance;
                    GetPosition( mCollisionData[ i ].mCollisionPosition );
                    mCollisionData[ i ].mCollisionNormal = theCollision.mNormal;     
                    mCollisionData[ i ].mpCollisionVolume = theCollision.mCollisionVolumeA == mpSimStateObj->GetCollisionObject()->GetCollisionVolume() ? theCollision.mCollisionVolumeB : theCollision.mCollisionVolumeA;
                }


                return sim::Solving_Continue;//true;
            }
        }
    }


    // Tests to see if we should skip this collision.
    //
    mbCollided = true;

    mCollisionData[ mCurrentCollision ].mCollisionDistance = theCollision.mDistance;
    GetPosition( mCollisionData[ mCurrentCollision ].mCollisionPosition );
    mCollisionData[ mCurrentCollision ].mCollisionNormal = theCollision.mNormal;     
    mCollisionData[ mCurrentCollision ].mpCollisionVolume = theCollision.mCollisionVolumeA == mpSimStateObj->GetCollisionObject()->GetCollisionVolume() ? theCollision.mCollisionVolumeB : theCollision.mCollisionVolumeA;

    mCurrentCollision++;

    return sim::Solving_Continue;//true;

}


//=============================================================================
// Character::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer Character::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
    return sim::Solving_Continue;
}


/*
==============================================================================
Character::SolveCollisionWithStatic
==============================================================================
Description:    desiredPos is the new position of the character after the 
                choreo::UpdateRoot( ) call.
                the position of the character at the time of collision 
                was stored in the mCollisionData array.

Parameters:     ( const rmt::Vector& desiredPos )

Return:         rmt

=============================================================================
*/
Character::eCollisionType Character::SolveCollisionWithStatic( const rmt::Vector& desiredPos, rmt::Vector& outPos )
{
    static bool sbOutput = false;

    outPos = desiredPos; //.Set( 0.0f, 0.0f, 0.0f );
    if ( !mbSolveCollisions )
    {
        return NoCollision;
    }
    eCollisionType collisionType = NoCollision;
    int intCollisionType = collisionType;
    int i = 0;

    if(mCurrentCollision > 0)
    {
        if ( sbOutput ) rDebugPrintf( "solving %d\n", mCurrentCollision);
    }

    // unsightly hack to try and deal with multiple collisions trying 
    // to push you through walls
    if(mCurrentCollision > 1)
    {
        bool lock = true;
        
        if(mCurrentCollision == 2)
        {
            if( mCollisionData[ 0 ].mCollisionNormal.Dot(mCollisionData[ 1 ].mCollisionNormal) > 0.5f)
            {
                lock = false;
            }
        }

        int nAway = 0;
        rmt::Vector facing;
        GetFacing(facing);

        for(int i = 0; i < mCurrentCollision; i++)
        {
            if( mCollisionData[ i ].mCollisionNormal.Dot(facing) > 0.0f)
            {
                nAway++;
            }
        }

        if(nAway == mCurrentCollision)
        {
            lock = false;
        }

        for(int i = 0; i < mCurrentCollision; i++)
        {
            if( mCollisionData[ i ].mCollisionNormal.y > 0.1f)
            {
                lock = false;
                break;
            }
        }

        if(lock)
        {
            if ( sbOutput ) rDebugPrintf( "locking\n", mCurrentCollision);
            outPos = mLastFramePos;
            if(this->GetJumpLocomotionAction()->IsInJump())
            {
                outPos.y = desiredPos.y;

                for(int i = 0; i < mCurrentCollision; i++)
                {
                    if ( mCollisionData[ i ].mCollisionNormal.y <= -0.5f )
                    {
                        return HitHead;
                    }
                }
            }
            return HitWall;
        }
    }

    int numSlides = 0;

    for(i = 0; i < mCurrentCollision; i++)
    {
        if( mCollisionData[ i ].mCollisionNormal.y > 0.1f)
        {
            numSlides += 1;
        }
    }

    for ( i = 0; i < mCurrentCollision; i++ )
    {
        if( !CanStandOnCollisionNormal( mCollisionData[ i ].mCollisionNormal ) )
        {

            // Desired motion vector.
            //
            rmt::Vector diffVect;
   
            // outPos is the solved position.  So it will change each time 
            // through the loop.
            //
            static bool sbUseOutpos = true;
            rmt::Vector prevPos = mCollisionData[ i ].mCollisionPosition; //GetPuppet( )->GetPrevPosition( );
            if ( sbUseOutpos )
                diffVect.Sub( outPos, prevPos );
            else
                diffVect.Sub( desiredPos, prevPos );

            // dampen the y a little, to avoid multiple sliding surfaces 
            // actually holding us in the air
            if( mCollisionData[ i ].mCollisionNormal.y > 0.1f)
            {
                diffVect.y *= 1.0f / float(numSlides);
            }

            // Normalized desired motion vector.
            //
            rmt::Vector normalizeDiff = diffVect;

            // The distance we want to travel this frame.
            //
            float dist = normalizeDiff.NormalizeSafe();

            // handle the case when the character is not moving,
            // but an animated collision volume has collided with it.
            //
            if ( dist == 0.0f )
            {
                // Possible solution.  Take the collision normal,
                // and use that for 'normalizeDiff'.  ie assume the character
                // is moving (relatively) in the direction of the normal.
                //
                normalizeDiff = mCollisionData[ i ].mCollisionNormal;
                normalizeDiff.Scale(-1.0f);
                dist = 0.0f;
            }
            // The dot will tell us if we are moving into ( dot < 0 )
            // the collision or away from the collision (dot >= 0 )..
            // 
            float dot = normalizeDiff.DotProduct( mCollisionData[ i ].mCollisionNormal );
            rmt::Vector adjPos = mCollisionData[ i ].mCollisionNormal;
            bool bSolve = true;
            if ( bSolve )
            {
                // Scale the distance against the collision normal.
                //
                float tempDist = dist;
                tempDist *= -dot; 

                // We only want to add the collision distance to
                // tempDist (the distance we will scale the motion vector)
                // when the collDist is gt zero, ie NOT interpentrating.
                // If we are interpenetrating, NOT adding the collDist
                // will snap us to the surface of the collision.
                //
                static bool sbPositive = true;
                static bool sbNegative = true;
                if (  mCollisionData[ i ].mCollisionDistance < 0.0f )
                {
                    if ( sbNegative )
                    {
                        tempDist -= mCollisionData[ i ].mCollisionDistance;
                        if ( sbOutput ) rDebugPrintf( "collision %.4f (%.2f, %.2f, %.2f) %s\n",
                            mCollisionData[ i ].mCollisionDistance,
                            mCollisionData[ i ].mCollisionNormal.x,
                            mCollisionData[ i ].mCollisionNormal.y,
                            mCollisionData[ i ].mCollisionNormal.z,
                            mCollisionData[ i ].mpCollisionVolume->GetCollisionObject( )->GetName( ) );
                    }
                }
                else if ( mCollisionData[ i ].mCollisionDistance > 0.0f )
                {
                    if( sbPositive )
                    {
                        tempDist += mCollisionData[ i ].mCollisionDistance;
                        if ( sbOutput ) rDebugPrintf( "collision %.4f (%.2f, %.2f, %.2f) %s\n",
                            mCollisionData[ i ].mCollisionDistance,
                            mCollisionData[ i ].mCollisionNormal.x,
                            mCollisionData[ i ].mCollisionNormal.y,
                            mCollisionData[ i ].mCollisionNormal.z,
                            mCollisionData[ i ].mpCollisionVolume->GetCollisionObject( )->GetName( ) );
                    }
                }
                adjPos.Scale( tempDist );

                rmt::Vector tmpOutPos = outPos;
                outPos.Add( adjPos );

                // if the collisionNormal is straight down.
                //
                if ( mCollisionData[ i ].mCollisionNormal.y <= -0.5f )
                {
                    // Ouch, you hit your head!
                    //
                    intCollisionType |= HitHead;
                    if(!IsNPC())
                    {
                        outPos.y -= 0.1f;
                    }
                }
                else
                {

                    // D'oh.  You ran into a wall.
                    //
                    // if we're surfing and the wall belongs to the car we're
                    // on, then we can ignore it. Physics gives us weird collisions
                    // from time to time, causing us to get shoved off the car 
                    // we're surfing on.
                    //
                    bool ignoreThisOne = false;

                    if( mbSurfing && (this->GetLocoVelocity().MagnitudeSqr() < 0.001f) && mpStandingCollisionVolume)
                    {
                        sim::SimState* surfSimState = mpStandingCollisionVolume->GetCollisionObject( )->GetSimState();

                        rAssert( surfSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle );

                        sim::SimState* objSimState = mCollisionData[ i ].mpCollisionVolume->GetCollisionObject()->GetSimState();
                        
                        if( objSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle &&
                            (Vehicle*)(objSimState->mAIRefPointer) == (Vehicle*)(surfSimState->mAIRefPointer) )
                        {
                            ignoreThisOne = true;
                        }
                    }

                    if( ignoreThisOne )
                    {
                        outPos = tmpOutPos;
                    }
                    else
                    {
                        intCollisionType |= HitWall;
                    }
                }

                if( intCollisionType != NoCollision )
                {
                    // if we hit a static or a fence piece
                    sim::SimState* simState = mCollisionData[ i ].mpCollisionVolume->
                        GetCollisionObject()->GetSimState();

                    if( mIsNPC && 
                        (simState->mAIRefIndex == PhysicsAIRef::redBrickPhizFence ||
                         simState->mAIRefIndex == PhysicsAIRef::redBrickPhizStatic) )
                    {
                        // if we're not panicking, this call will have no effect
                        ((NPCController*)GetController())->QuellPanic();
                    }

                }
            }
            else
            {
                if ( sbOutput ) rDebugPrintf( "Dot product reject. dot = %.2f, dist = %.6f, %s\n", 
                    dot,
                mCollisionData[ i ].mCollisionDistance,
                mCollisionData[ i ].mpCollisionVolume->GetCollisionObject( )->GetName( )
                );
            }
        }
        else
        {
            if ( sbOutput ) rDebugPrintf( "CollisionNormal reject: %.2f, %.2f, %.2f, %s\n", 
                mCollisionData[ i ].mCollisionNormal.x,
                mCollisionData[ i ].mCollisionNormal.y,
                mCollisionData[ i ].mCollisionNormal.z,
                mCollisionData[ i ].mpCollisionVolume->GetCollisionObject( )->GetName( )
                );
        }
    }
    collisionType = (eCollisionType)intCollisionType;
    return collisionType;
}
/*
==============================================================================
Character::GetMaxSpeed
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float 

=============================================================================
*/
float Character::GetMaxSpeed( void ) const
{
    float fTurbo = 0.0f;
    if ( IsTurbo( ) )
    {
        fTurbo = CharacterTune::sfDashBurstMax;
    }
    return CharacterTune::sfMaxSpeed + fTurbo;
}

/*
==============================================================================
Character::GetTerrainIntersect
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& pos, rmt::Vector& normal )

Return:         void 

=============================================================================
*/
void Character::GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const
{
    GetPosition( pos );
    pos.y = mGroundY;
    normal = mGroundNormal;
}

void Character::GetTerrainType( eTerrainType& TerrainType, bool& Interior ) const
{
    TerrainType = mTerrainType;
    Interior = mInteriorTerrain;
}

void Character::SnapToGround(void)
{
    mbSnapToGround = true;
    UpdateGroundHeight();
}

void Character::UpdateGroundHeight( void )
{
    unsigned int modulo = mbInAnyonesFrustrum ? 0x3 : 0x7;
    if( !mbSnapToGround && IsNPC() && 
        ((GetGame()->GetFrameCount() & modulo) != (mIntersectFrame & modulo)))
    {
        return;
    }

BEGIN_PROFILE("Character::UpdateGroundHeight")

    choreo::Puppet* pPuppet = GetPuppet( );
    rAssert( pPuppet );

    // Before
    //
    rmt::Vector prevPosition = mLastFramePos;//LocalToWorld( pPuppet->GetPrevPosition( ) );

    // And after!
    //
    rmt::Vector position;
    GetPosition( position );
    // Updates the cached value.
    //
//        UpdateGroundHeight( prevPosition, position, mGroundY, mGroundNormal ); 

    rmt::Vector groundPosition = position;
    rmt::Vector outnorm( 0.0f, 1.0f, 0.0f );

    // I don't understand these ones.
    //
    rmt::Vector intersectPos = position;
    rmt::Vector intersectNorm( 0.0f, 1.0f, 0.0f );
    bool bFoundIntersect = false;
    bool bFoundPlane = false;

    mTerrainType = static_cast<eTerrainType>( GetIntersectManager()->FindIntersection(
        groundPosition,   // IN
        bFoundPlane,      // OUT
        outnorm,          // OUT
        groundPosition ) );   // OUT
    mInteriorTerrain = ( (int)mTerrainType & 0x80 ) == 0x80;
    mTerrainType = static_cast<eTerrainType>( ( (int)mTerrainType & ~0x80 ) );

    if( bFoundPlane )
    {
        mRealGroundPos = groundPosition;
        mRealGroundNormal = outnorm;
        float tooHigh = 100000.0f;
        rAssert( -tooHigh <= mRealGroundNormal.x && mRealGroundNormal.x <= tooHigh );
        rAssert( -tooHigh <= mRealGroundNormal.y && mRealGroundNormal.y <= tooHigh );
        rAssert( -tooHigh <= mRealGroundNormal.z && mRealGroundNormal.z <= tooHigh );
    }
    else
    {
        // 
        // If this assert goes off for the player charater when he's hit by traffic
        // it means that Physics has placed him at some location other than his
        // present location. This is a bad thing... possibly related to 
        // collisions with traffic cars.
        //rAssertMsg( false, "We're SOOO far from the ground, we're not intersecting" );
    }
    rmt::Vector collisionPosition;
    rmt::Vector collisionNormal;
    
    collisionNormal.Clear( );
    collisionPosition.Clear( );
 

    rmt::Vector playerPos;
    GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition( playerPos );

    rmt::Vector distVecToPlayer = playerPos - position;
    distVecToPlayer.y = 0.0f;
    float distSqrFromPlayer = distVecToPlayer.MagnitudeSqr();

    float delta = position.y - groundPosition.y;

    const float TOO_FAR_FROM_GROUND_DIST = 2.0f; // 2 meters? Maybe that's good enough
    const float VISIBLE_TO_PLAYER_DIST_SQR = 90000.0f;
    const float CLOSE_ENOUGH_TO_PLAYER_DIST_SQR = 400.0f; // always check if within 20 meters

    bool bOverStatic = false;
    if(IsNPC())
    {
        // discretionally ray-test against objects
        if( delta > TOO_FAR_FROM_GROUND_DIST ||
            (!bFoundPlane && distSqrFromPlayer <= VISIBLE_TO_PLAYER_DIST_SQR) ||
            distSqrFromPlayer < CLOSE_ENOUGH_TO_PLAYER_DIST_SQR ) 
        {
            //rDebugPrintf( "**** NPC %s calling getCollisionHeight *****\n", GetName() );
            bOverStatic = GetCollisionHeight( prevPosition, position, collisionPosition, collisionNormal );
        }
        /*
        else if( distSqrFromPlayer >= CLOSE_ENOUGH_TO_PLAYER_DIST_SQR ) 
        {
            if( strcmp( GetName(), "b_ralph" ) == 0 )
            {
                rDebugPrintf( "%s not within 20 meters of player\n", GetName() );
            }
        }
        */
    }
    else
    {
        bOverStatic = GetCollisionHeight( prevPosition, position, collisionPosition, collisionNormal );
    }

    bool bNpcShouldNotMove = false;

    if ( bOverStatic )
    {
        if ( !bFoundPlane || collisionPosition.y > groundPosition.y )
        {
            mGroundY = collisionPosition.y;
            mGroundNormal = collisionNormal;

            mLastGoodPosOverStatic = position;
            mLastGoodPosOverStatic.y = mGroundY;
        }
        else 
        {
            rAssert( bFoundPlane );
            mGroundY = groundPosition.y;
            mGroundNormal = outnorm;
        }
    }
    else if ( bFoundPlane )
    {
        if( IsNPC() && mLastGoodPosOverStatic.Equals( position, 0.5f ) )
        {
            if( ((NPCController*)GetController())->GetState() == NPCController::TALKING_WITH_PLAYER )
            {
                bNpcShouldNotMove = false; // don't transit out of TALKING_WITH_PLAYER state
            }
            else
            {
                bNpcShouldNotMove = true;
            }
            mGroundY = position.y;
            mGroundNormal.Set( 0.0f, 1.0f, 0.0f );
        }
        else
        {
            mGroundY = groundPosition.y;
            mGroundNormal = outnorm;
        }
    }
    else
    {
        bNpcShouldNotMove = true;
        mGroundY = position.y;
        mGroundNormal.Set( 0.0f, 1.0f, 0.0f );
    }

    if(mpSimStateObj->GetControl() == sim::simSimulationCtrl)
    {
        return;
    }

    if( mIsNPC )
    {
        // if we're dropping more than N meters, we transit to sim,
        // but only if we have a valid ground plane (otherwise we could
        // fall through the world!)
        delta = position.y - mGroundY;
        if( delta > TOO_FAR_FROM_GROUND_DIST &&
            distSqrFromPlayer <= VISIBLE_TO_PLAYER_DIST_SQR &&
            mGroundPlaneIndex != WorldPhysicsManager::INVALID_COLLISION_AREA )
        {
            // transit to sim here so we "fall" to the ground
            mpSimStateObj->SetControl( sim::simSimulationCtrl );
            GetWorldPhysicsManager()->EnableGroundPlaneCollision(mGroundPlaneIndex);
        }
    }


    static float sfFallingTolerance = 0.5f;

    rmt::Vector pos;
    GetPosition( pos );

    rmt::Vector dummy, groundPos;
    GetTerrainIntersect( groundPos, dummy );

    delta = pos.y - groundPos.y;

    JumpAction* pJumpAction = GetJumpLocomotionAction();
    bool inJump = pJumpAction->IsInJump();

    if (( mVelocity.y <= mfGroundVerticalVelocity && (delta < 0.0f )) || 
        ( !inJump && (delta < sfFallingTolerance)) ||
        ( IsNPC() && mpSimStateObj->GetControl() == sim::simAICtrl ) ||
        mbSnapToGround)
    {
        mbIsStanding = true;

        if(mbDoGroundIntersect || mbSnapToGround)
        {
            mbSnapToGround = false;

            if( IsNPC() )
            {
                NPCController* npcController = (NPCController*) GetController();
                if( bNpcShouldNotMove )
                {
                    npcController->TransitToState( NPCController::NONE );
                }
                else
                {
                    if( npcController->GetState() == NPCController::NONE )
                    {
                        npcController->TransitToState( NPCController::FOLLOWING_PATH );
                    }
                }
            }
            SetPosition(groundPos);
            this->SetGroundPoint(groundPos);
            mpJumpLocomotion->SetRootPosition( WorldToLocal(groundPos) );
        }
    }
    else
    {
        mbIsStanding = false;

        if ( !mbIsStanding  && mbDoGroundIntersect && !inJump && (this->mpSimStateObj->GetControl() == sim::simAICtrl))
        {
            //falling.
            //
            pJumpAction->Reset( 0.0f, true );
            GetActionController()->Clear();;
            Sequencer* seq = GetActionController()->GetNextSequencer();
            seq->BeginSequence();
            seq->AddAction(pJumpAction);
            seq->EndSequence();
        }
    }

END_PROFILE("Character::UpdateGroundHeight")
}
/*
==============================================================================
Character::pPosition
==============================================================================
Description:    Comment

Parameters:     ()

Return:         rmt

=============================================================================
*/
rmt::Vector* Character::pPosition()
{
    rAssert( 0 );
    return (Vector*)0;
}

/*
==============================================================================
Character::rPosition
==============================================================================
Description:    Comment

Parameters:     ()

Return:         const 

=============================================================================
*/
const rmt::Vector& Character::rPosition()
{
    GetPosition(lameAssPosition);
    return lameAssPosition;
}

/*
==============================================================================
Character::GetPosition
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* ipPosn )

Return:         void 

=============================================================================
*/
void Character::GetPosition( rmt::Vector* ipPosn )
{
    GetPosition(*ipPosn);
}

//////////////////////////////////////////////////////////////////////////

void Character::SetFadeAlpha( int fadeAlpha )
{
    if( mpCharacterRenderable != NULL )
    {
        mpCharacterRenderable->SetFadeAlpha( fadeAlpha );
    }
}

int Character::CastsShadow()
{
    return mpCharacterRenderable->CastsShadow();
}
/*
==============================================================================
Character::DisplayShadow
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void

=============================================================================
*/
void Character::DisplayShadow()
{
    /*
    if( !IsInCar() && !IsSimpleShadow() )
    {
        mpCharacterRenderable->DisplayShadow( mpPuppet->GetP3DPose() );
    }
    */
}

/*
==============================================================================
Character::DisplaySimpleShadow
==============================================================================
Description:    Draw the simple shadow during the simple shadow pass.

Parameters:     ()

Return:         void

=============================================================================
*/
void Character::DisplaySimpleShadow( void )
{
}


//=============================================================================
//Character::DisplaySimpleShadow
//=============================================================================
//Description:    Draw the simple shadow during the simple shadow pass.
//
//Parameters:     ()
//
//Return:         void
//=============================================================================
bool Character::CanPlayAnimation( const tName& name ) const
{
    choreo::Bank* bank = mpPuppet->GetBank();
    choreo::Animation* anim = choreo::find<choreo::Animation>( bank, name.GetUID() );
    if( anim == NULL )
    {
        PrintAnimations();
    }
    return ( anim != NULL );
}

/*
==============================================================================
Character::CanStandOnCollisionVolume
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool Character::CanStandOnCollisionVolume( void ) const
{
    int i;
    
    for ( i = 0; i < mCurrentCollision; i++ )
    {
        bool bCanStand = CanStandOnCollisionNormal( mCollisionData[ i ].mCollisionNormal );
        if ( bCanStand )
        {
            return true;
        }
    }
    return false;
}

/*
==============================================================================
Character::CanStandOnCollisionNormal
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& normal )

Return:         bool 

=============================================================================
*/
bool Character::CanStandOnCollisionNormal( const rmt::Vector& normal ) const
{
    float dot = normal.DotProduct( vUp );
    const float cos30 = 0.86602540378443864676372317075294f;
    static float sfStandTolerance = cos30;
    if ( dot > sfStandTolerance )
    {
        return true;
    }
    return false;
}

/*
==============================================================================
Character::CanStaggerCollision
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool Character::CanStaggerCollision( void ) const
{
    int i;
    
    for ( i = 0; i < mCurrentCollision; i++ )
    {
        if ( this->mpStandingCollisionVolume != mCollisionData[ i ].mpCollisionVolume )
        {
            bool bCanStagger = CanStaggerCollisionNormal( mCollisionData[ i ].mCollisionNormal );
            if ( bCanStagger )
            {
                return true;
            }
        }
    }
    return false;
}

/*
==============================================================================
Character::CanStaggerCollisionNormal
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& normal )

Return:         bool 

=============================================================================
*/
bool Character::CanStaggerCollisionNormal( const rmt::Vector& normal ) const
{

    static rmt::Vector facing;
    GetFacing( facing );

    float dot = normal.DotProduct( facing );
    static float sfStaggerTolerance = -0.9f;
    if (  dot < sfStaggerTolerance )
    {
        return true;
    }
    return false;
}
/*
==============================================================================
Character::FindStandingVolume
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& inPos, sim::CollisionVolume* inVolume, rmt::Vector& outNormal, float& outDist)

Return:         sim

=============================================================================
*/
sim::CollisionVolume* Character::FindStandingVolume( const rmt::Vector& inPos, sim::CollisionVolume* inVolume, rmt::Vector& outNormal, float& outDist )
{
    sim::CollisionVolume* pOutVolume = 0;

    switch (inVolume->Type())
    {
    case sim::SphereVolumeType:
    case sim::CylinderVolumeType:
    case sim::OBBoxVolumeType:
    case sim::WallVolumeType:
        {
            float currentDist = VERY_LARGE;
            rmt::Vector currentNormal;

            // trivial reject stuff that is actually no where near us (probably a subvolume 
            // of a large volume we did intersect with)
            if((rmt::Fabs(inVolume->mPosition.x - inPos.x) > (inVolume->mBoxSize.x + 1.0f)) ||  
               (rmt::Fabs(inVolume->mPosition.z - inPos.z) > (inVolume->mBoxSize.z + 1.0f)))
            {
                break;
            }

            sim::CollisionVolume* pOutSubVolume = sim::FindClosestPointOnVolume( inPos, inVolume, currentNormal, currentDist);
            if ( pOutSubVolume && pOutSubVolume->GetCollisionObject()->GetCollisionEnabled())
            {
                if ( CanStandOnCollisionNormal( currentNormal ) )
                {
                    // We must be above the object.
                    //
                    if ( currentDist >= 0.0f )
                    {
                        pOutVolume = pOutSubVolume;
                        outDist = currentDist;
                        outNormal = currentNormal;
                    }
                }
            }
            break;
        }
    case sim::BBoxVolumeType:
        {
            float currentDist = VERY_LARGE;
            rmt::Vector currentNormal;
            for (int i=0; i<inVolume->SubVolumeList()->GetSize(); i++)
            {
                sim::CollisionVolume* pOutSubVolume = FindStandingVolume(inPos, inVolume->SubVolumeList()->GetAt(i), currentNormal, currentDist );
                if ( pOutSubVolume && pOutSubVolume->GetCollisionObject()->GetCollisionEnabled())
                {
                    // For each volume returned, keep the closest one only.
                    //
                    if ( currentDist < outDist )
                    {
                        outDist = currentDist;
                        outNormal = currentNormal;
                        pOutVolume = pOutSubVolume;
                    }
                }
            }
        }
        break;
    case sim::MaxCollisionVolumeEnum:
    case sim::CollisionVolumeType:
        break;
    }
    return pOutVolume;
}
/*
==============================================================================
Character::GetCollisionHeight
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& prevPosition, const rmt::Vector& position, rmt::Vector& outPosition, rmt::Vector& collisionNormal )

Return:         bool 

=============================================================================
*/
bool Character::GetCollisionHeight( const rmt::Vector& prevPosition, const rmt::Vector& position, rmt::Vector& outPosition, rmt::Vector& collisionNormal )
{   
BEGIN_PROFILE( "GetCollisionHeight" );

    msIntersectInfo.Clear();
    
    float fOldRayThickness = sim::RayIntersectionInfo::sRayThickness;
    static float sfCharacterRayThickness = 0.3f;
    sim::RayIntersectionInfo::sRayThickness = sfCharacterRayThickness;
    const poser::Joint* pStandingJoint = 0;
    bool bFoundIntersect = false;
    sim::CollisionVolume* oldStanding = NULL;
    tRefCounted::Assign(oldStanding, mpStandingCollisionVolume);
    tRefCounted::Release(mpStandingCollisionVolume);

    // Had to increase the fudge when I moved character::update calls outside
    // of the physics substep.
    //
    static float sfFudge = 0.6f;
    rmt::Vector testPosition;
    testPosition = position;
    testPosition.y = prevPosition.y;
    testPosition.y += sfFudge;

    float outDist = VERY_LARGE;
    float currentDist = outDist;

    HeapMgr()->PushHeap( GMA_TEMP );

    // adds in the list the collision object interfering with the ray and ordered according to their distance to the source.
    // use sim::RayIntersectionInfo::SetMethod(method) to set the method
    // use sim::RayIntersectionInfo::SetReturnClosestOnly(true/false) if you need only the closest object
    // nb. if SetReturnClosestOnly(true) is used, the previous returned list can be used as a cache.
    sim::RayIntersectionInfo::SetReturnClosestOnly( false );
    sim::RayIntersectionInfo::SetMethod( sim::RayIntersectionBBox );
    rmt::Vector rayOrg( 0.0f, 0.0f, 0.0f );
    rmt::Vector rayEnd( 0.0f, -100.0f, 0.0f );
    rayOrg.Add( testPosition );
    rayEnd.Add( testPosition );

    HeapMgr()->PopHeap( GMA_TEMP );

    // Do not allow ray test against the character collision object
    // Otherwise, rayintersection will detect against player volume.
    //
    bool bRayRestore = mpSimStateObj->GetCollisionObject()->GetRayCastingEnabled( );
    bool bCollRestore = mpSimStateObj->GetCollisionObject()->GetCollisionEnabled( );
    mpSimStateObj->GetCollisionObject( )->SetRayCastingEnabled( false );  
    mpSimStateObj->GetCollisionObject( )->SetCollisionEnabled( false );  
    
    bool bGroundPlaneRestore = false;
    if( mGroundPlaneSimState )
    {
        bGroundPlaneRestore = mGroundPlaneSimState->GetCollisionObject( )->GetRayCastingEnabled( );
        mGroundPlaneSimState->GetCollisionObject( )->SetRayCastingEnabled( false );
    }

    // Test ray against remaining collision objects.
    //
    GetWorldPhysicsManager()->mCollisionManager->DetectRayIntersection(msIntersectInfo, rayOrg, rayEnd, false, this->GetCollisionAreaIndex() );
    //GetWorldPhysicsManager()->mCollisionManager->DetectRayIntersection(msIntersectInfo, rayOrg, rayEnd, false, 
    //    GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter()->GetCollisionAreaIndex() );

    // Restore the state.
    //
    mpSimStateObj->GetCollisionObject( )->SetRayCastingEnabled( bRayRestore );
    mpSimStateObj->GetCollisionObject( )->SetCollisionEnabled( bCollRestore  );  
    if( mGroundPlaneSimState )
    {
        mGroundPlaneSimState->GetCollisionObject( )->SetRayCastingEnabled( bGroundPlaneRestore );
    }


    // Iterate through the entire list because of way DetectRayIntersection works.
    // It checks the top level hierarchy of an object, so it will return bad values
    // if the hierarchy is large, and you are standing on top of something in a different 
    // (smaller) hierarchy.  see Level 9, duffTruck BV vs L9 BV.
    //
    int i;
    for ( i = 0; i < msIntersectInfo.GetSize( ); i++ )
    {
        if ( msIntersectInfo.GetSize() > 0 && msIntersectInfo[ i ].mCollisionVolume )
        {
            rmt::Vector outNormal;
            float prevOut = outDist;
            sim::CollisionVolume* pOutVolume = FindStandingVolume( testPosition, msIntersectInfo[ i ].mCollisionVolume, outNormal, outDist );

            bool vehicleIgnore = false;
            if(this->mpTargetVehicle && pOutVolume)
            {
               if(pOutVolume->GetCollisionObject()->GetSimState()->mAIRefPointer == this->mpTargetVehicle)
               {
                   if(oldStanding != pOutVolume)
                   {
                       vehicleIgnore = true;
                       outDist = prevOut;
                   }
               }
            }

            if ( pOutVolume && 
                 pOutVolume->GetCollisionObject()->GetCollisionEnabled() &&
                 (pOutVolume->GetCollisionObject()->GetSimState()->mAIRefIndex != PhysicsAIRef::redBrickPhizVehicleGroundPlane) &&
                 (pOutVolume->GetCollisionObject()->GetSimState()->mAIRefIndex != PhysicsAIRef::redBrickPhizMoveableGroundPlane) &&
                 (!vehicleIgnore))
            {
                if ( outDist < currentDist )
                {
                    currentDist = outDist;
                    tRefCounted::Assign(mpStandingCollisionVolume, pOutVolume);
                    collisionNormal = outNormal;
                }
            }
        }
    }

    mbSurfing = false;

    if ( mpStandingCollisionVolume )
    {
        rmt::Vector newPos = collisionNormal;
        newPos.Scale( -currentDist );
        testPosition.Add( newPos );
        outPosition = testPosition;

        // Test to see if we are standing on the collision volume.
        //    
        if ( position.y - outPosition.y <= 0.1f )
        {
            // We are standing.
            //
            // If this is an animated object, find the transform to parent the character.
            // 
            sim::SimState* pSimState = mpStandingCollisionVolume->GetCollisionObject( )->GetSimState();
            
            if(pSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
            {
                mbSurfing = true;
            }

            if( !( PhysicsAIRef::redBrickPhizStatic & pSimState->mAIRefIndex ) )
            {
                if ( pSimState->IsArticulated( ) )
                {
                    // Find joint based on mpStandingCollisionVolume->ObjRefIndex().
                    //
                    sim::SimStateArticulated* pSimStateArticulated = static_cast<sim::SimStateArticulated*>( pSimState );

                    // Michael - this will assert on articulated objects that were converted to rigid bodies
                    // (these always have their volume::objrefindex set to -1)
                    // commenting out, it doesn't appear to have any problems

                    // rAssert(mpStandingCollisionVolume->ObjRefIndex() != -1);
                    if(mpStandingCollisionVolume->ObjRefIndex() != -1)
                    {
                        const poser::Pose* pPose = pSimStateArticulated->GetPose( );
                        rAssert( pPose );
                        pStandingJoint = pPose->GetJoint( mpStandingCollisionVolume->ObjRefIndex() );
                        rAssert( pStandingJoint );
                    }
                }

            }
        }
        bFoundIntersect = true;
    }

    SetStandingJoint( pStandingJoint );
    sim::RayIntersectionInfo::sRayThickness = fOldRayThickness;
END_PROFILE( "GetCollisionHeight" );
    tRefCounted::Release(oldStanding);
    return bFoundIntersect;
}

#ifdef RAD_DEBUG

void Character::PrintAnimations() const
{
    choreo::Bank* bank = mpPuppet->GetBank();
    choreo::BaseBank::RawIterator* it = bank->NewRawIterator();
    it->AddRef();

    IRefCount* obj = it->First();
    while( obj != NULL )
    {
        choreo::Animation* anim = dynamic_cast< choreo::Animation* >( obj );
        if( anim != NULL )
        {
            tAnimation* tAnim = anim->GetP3DAnimation();
            const char* name = tAnim->GetName();
            rDebugPrintf( "animationName '%s'\n", name );
        }
        obj = it->Next();
    }
    it->Release();
}
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void Character::SetStandingJoint( const poser::Joint* pJoint )
{
    if ( pJoint != mpStandingJoint )
    {
        rmt::Vector position;
        GetPosition( position );

        rmt::Vector desiredFacing;
        GetDesiredFacing( desiredFacing );
        rmt::Vector facingVector = mpPuppet->GetFacingVector( );

        mParentTransform.RotateVector( desiredFacing, &desiredFacing ); 
        mParentTransform.RotateVector( facingVector, &facingVector );

        rmt::Matrix transform;
        if(pJoint)
        {
            transform = pJoint->GetWorldMatrix( );
        }
        else
        {
            transform.Identity();
        }

        SetParentTransform( transform );

        SetPosition( position );
        mInvParentTransform.RotateVector( desiredFacing, &desiredFacing ); 
        mInvParentTransform.RotateVector( facingVector, &facingVector );
        mpPuppet->SetFacingVector( facingVector );
        SetDesiredDir( choreo::GetWorldAngle( desiredFacing.x, desiredFacing.z ) );
        mpJumpLocomotion->SetRootTransform( );
        mpStandingJoint = pJoint;
    }
}

/*
==============================================================================
Character::SetGroundPoint
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& groundPoint )

Return:         void 

=============================================================================
*/
void Character::SetGroundPoint( const rmt::Vector& groundPoint )
{
    //choreo::Puppet* pPuppet = GetPuppet( );
    //if ( pPuppet )
    //{
    //    pPuppet->SetGroundPoint( groundPoint );
    //}
    //SetPosition( groundPoint );
    if ( mpPuppet )
    {
        // Transform from world to object space.
        //
        rmt::Vector transformedPos = groundPoint;
        transformedPos.Transform( mInvParentTransform );

        mpPuppet->SetPosition( transformedPos );

        poser::Pose* pPose = mpPuppet->GetPose( );
        // stuff fixed up root transform into joint
        poser::Joint* joint = pPose->GetJoint( 0 );
        joint->SetObjectTranslation( groundPoint );
        joint->SetWorldTranslation( groundPoint );
    }
}
//=============================================================================
// Character::UpdateTransformToLoco
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void Character::UpdateTransformToLoco( void )
{
    // This will get us the world space position and facing.
    //
    rmt::Vector position;
    GetPosition( position );
    rmt::Vector facing;
    GetFacing( facing );
    mParentTransform.RotateVector( facing, &facing );

    tRefCounted::Release(mpStandingCollisionVolume);
    mbSurfing = false;

    // Go from the car space back to world space.
    //
    UpdateParentTransform( 0.0f );
    
    SetDesiredDir( choreo::GetWorldAngle( facing.x, facing.z ) ); 
    SetFacingDir( choreo::GetWorldAngle( facing.x, facing.z ) );
    SetPosition( position );

    SetInCar(false);
    
    //AssignCollisionAreaIndex( );
    //mpSimStateObj->GetCollisionObject()->SetCollisionEnabled( true );
}
/*
==============================================================================
Character::AssignCollisionAreaIndex
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::AssignCollisionAreaIndex( void )
{
    if ( WorldPhysicsManager::INVALID_COLLISION_AREA == mCollisionAreaIndex )
    {
        mCollisionAreaIndex = GetWorldPhysicsManager()->GetCharacterCollisionAreaIndex();
    }
    rTuneAssert( mCollisionAreaIndex != -1 );
}
//=============================================================================
// Character::UpdateTransformToInCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void Character::UpdateTransformToInCar( void )
{
    if( mbIsJump )
    {
        mpJumpLocomotion->Done();
        mbIsJump = false;
    }

    rmt::Vector pos;
    GetPosition(pos);

    // Update the character parent transform to vehicleToWorld.
    //
    UpdateParentTransform( 0.0f );

    SetStandingJoint(NULL);
    tRefCounted::Release(mpStandingCollisionVolume);
    mbSurfing = false;

    SetPosition(pos);
    SetDesiredDir( rmt::PI );
    SetFacingDir( rmt::PI );
}

/*
==============================================================================
Character::SetInCar
==============================================================================
Description:    Comment

Parameters:     ( bool bInCar )

Return:         void 

=============================================================================
*/
void Character::SetInCar( bool bInCar )
{
    mbInCar = bInCar;

    mTranslucent = !mbInCar;

    if(this == GetCharacterManager()->GetCharacter(0))
    {
        CGuiScreenHud* currentHud = GetCurrentHud();
        if( !mbInCar )
        {
            if(  radTimeGetMicroseconds64()-mLastInteriorLoadCheck > 3000000 ) //(amortise/3s) if it's been more than 3 sec's since we last checked for volumes
            {
                rmt::Vector posn;
                GetPosition(&posn);
                GetTriggerVolumeTracker()->ActivateNearestInteriorLoadZone(0, posn, 40.0f);
                mLastInteriorLoadCheck = radTimeGetMicroseconds64();
            }

            if(mpCurrentActionButtonHandler)
            {
                if ( mpCurrentActionButtonHandler->IsInstanceEnabled() )
                {
                    if( currentHud != NULL )
                    {
                        currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_ACTION_BUTTON );
                    }
                }
            }
        }
        else if(mbInCar && mpCurrentActionButtonHandler)
        {
            if( currentHud != NULL )
            {
                currentHud->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
            }
        }
    }
}

/*
==============================================================================
Character::GetActionButtonHandler
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ActionButton

=============================================================================
*/
ActionButton::ButtonHandler* Character::GetActionButtonHandler( void ) const
{
    return mpCurrentActionButtonHandler;
}

/*
==============================================================================
Character::AddActionButtonHandler
==============================================================================
Description:    Comment

Parameters:     ( ActionButton::ButtonHandler* pActionButtonHandler )

Return:         void 

=============================================================================
*/
void Character::AddActionButtonHandler( ActionButton::ButtonHandler* pActionButtonHandler )
{
    if ( !IsNPC() )
    {
        unsigned int i;
#ifdef RAD_DEBUG
        //Make sure this is only added once.
        for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
        {
            rAssert( mpActionButtonHandlers[ i ] != pActionButtonHandler );
        }
#endif
        for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
        {
            if ( mpActionButtonHandlers[ i ] == NULL )
            {
                //Add here.
                mpActionButtonHandlers[ i ] = pActionButtonHandler;
                mpActionButtonHandlers[ i ]->AddRef();
                break;
            }
        }

        rAssertMsg( i < MAX_ACTION_BUTTON_HANDLERS, "Need to increase the size of MAX_ACTION_BUTTON_HANDLERS" );
        if ( i == MAX_ACTION_BUTTON_HANDLERS )
        {
            return;
        }

        ActionButton::ButtonHandler* newButton = TestPriority( pActionButtonHandler, mpCurrentActionButtonHandler );

        if ( newButton != mpCurrentActionButtonHandler )
        {
            //This is a new action button of highest priority.
            
            mpCurrentActionButtonHandler = pActionButtonHandler;

            if ( !IsInCar() )
            {
                if ( mpCurrentActionButtonHandler->IsInstanceEnabled() )
                {
                    CGuiScreenHud* currentHud = GetCurrentHud();
                    if( currentHud != NULL )
                    {
                        currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_ACTION_BUTTON );
                    }
                }
            }
        }
    }
}

//=============================================================================
// Character::RemoveActionButtonHandler
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionButtonHandler::ButtonHandler* pActionButtonHandler )
//
// Return:      void 
//
//=============================================================================
void Character::RemoveActionButtonHandler( ActionButton::ButtonHandler* pActionButtonHandler )
{
    if ( !IsNPC() )
    {
        unsigned int i;
        for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
        {
            if ( mpActionButtonHandlers[ i ] == pActionButtonHandler )
            {
                //This is the one to remove.
                mpActionButtonHandlers[ i ]->Release();
                mpActionButtonHandlers[ i ] = NULL;

                if ( mpCurrentActionButtonHandler == pActionButtonHandler )
                {
                    mpCurrentActionButtonHandler = NULL;
                    CGuiScreenHud* currentHud = GetCurrentHud();
                    if( currentHud != NULL )
                    {
                        currentHud->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
                    }
                }

                break;
            }
        }

        if ( mpCurrentActionButtonHandler == NULL )
        {
            //Find a new one.
            for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
            {
                if ( mpActionButtonHandlers[ i ] != NULL )
                {
                    mpCurrentActionButtonHandler = TestPriority( mpCurrentActionButtonHandler, mpActionButtonHandlers[ i ] );
                }
            }

            if ( mpCurrentActionButtonHandler != NULL )
            {
                if ( mpCurrentActionButtonHandler->IsInstanceEnabled() )
                {
                    CGuiScreenHud* currentHud = GetCurrentHud();
                    if( currentHud != NULL )
                    {
                        currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_ACTION_BUTTON );
                    }
                }
            }
        }
    }
}

//=============================================================================
// Character::TestPriority
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionButton::ButtonHandler* bA, ActionButton::ButtonHandler* bB )
//
// Return:      ActionButton
//
//=============================================================================
ActionButton::ButtonHandler* Character::TestPriority( ActionButton::ButtonHandler* bA, ActionButton::ButtonHandler* bB )
{
    if ( NULL == bA )
    {
        return bB;
    }
    else if ( NULL == bB )
    {
        return bA;
    }

    ActionButton::ButtonHandler::Type bAType = bA->GetType();
    ActionButton::ButtonHandler::Type bBType = bB->GetType();

    if ( bAType == ActionButton::ButtonHandler::GET_IN_CAR || bBType == ActionButton::ButtonHandler::GET_IN_CAR )
    {
        //Is this the players car?
        Avatar* myAvatar = GetAvatarManager()->FindAvatarForCharacter( this );
        
        if ( myAvatar )
        {
            int id = myAvatar->GetPlayerId();
            rAssert( id == 0 );  //ONly works in single player.

            Vehicle* myVehicle = GetGameplayManager()->GetCurrentVehicle();
            if ( myVehicle )
            {
                int actionId = (int)myVehicle->mpEventLocator->GetData( );   
                ActionButton::ButtonHandler* pActionButtonHandler = GetActionButtonManager()->GetActionByIndex( actionId );
                rAssert( pActionButtonHandler );

                if ( bA == pActionButtonHandler )
                {
                    bAType = ActionButton::ButtonHandler::GET_IN_USER_CAR;
                }

                if ( bB == pActionButtonHandler )
                {
                    bBType = ActionButton::ButtonHandler::GET_IN_USER_CAR;
                }
            }
        }
    }

    return bAType < bBType ? bA : bB;
}

//=============================================================================
// Character::ClearAllActionButtonHandlers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Character::ClearAllActionButtonHandlers()
{
    unsigned int i;
    for ( i = 0; i < MAX_ACTION_BUTTON_HANDLERS; ++i )
    {
        if ( mpActionButtonHandlers[ i ] != NULL )
        {
            mpActionButtonHandlers[ i ]->Exit( this );
            mpActionButtonHandlers[ i ]->Release();
            mpActionButtonHandlers[ i ] = NULL;
        }
    }

    mpCurrentActionButtonHandler = NULL;
    if(this == GetCharacterManager()->GetCharacter(0))
    {
        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            currentHud->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
        }
    }
}

tSkeleton* Character::Prop::spSkeleton = 0;
int Character::Prop::sSkelRefs = 0;

Character::Prop::Prop( void )
    :
mpProp( 0 ),
mpPose( 0 )
{
    if ( !spSkeleton )
    {
        spSkeleton = new tSkeleton( 1 );
        rmt::Matrix mat;
        mat.Identity( );
        spSkeleton->GetJoint( 0 )->worldMatrix = spSkeleton->GetJoint( 0 )->restPose = mat;
        spSkeleton->GetJoint( 0 )->inverseWorldMatrix = spSkeleton->GetJoint( 0 )->worldMatrix;
        spSkeleton->GetJoint( 0 )->inverseWorldMatrix.InvertOrtho( );
    }
    spSkeleton->AddRef( );
    sSkelRefs++;
    tRefCounted::Assign( mpPose, spSkeleton->NewPose( ) );
}

Character::Prop::~Prop( void )
{
    tRefCounted::Release( mpProp );
    tRefCounted::Release( mpPose );
    spSkeleton->Release ();
    sSkelRefs--;
    if (sSkelRefs < 1)
    {
        spSkeleton = 0;
    }
}
/*
==============================================================================
Character::TouchProp
==============================================================================
Description:    Comment

Parameters:     ( InstDynaPhysDSG* pProp )

Return:         void 

=============================================================================
*/
void Character::TouchProp( InstDynaPhysDSG* pProp )
{
    /*
    bool sbPickUp = false;
    if ( sbPickUp )
    {
        mpPropHandler->SetProp( pProp );
        AddActionButtonHandler( mpPropHandler );
        mpPropHandler->Enter( this );
    }
    */
}
/*
==============================================================================
Character::AttachProp
==============================================================================
Description:    Comment

Parameters:     ( InstDynaPhysDSG* pProp )

Return:         void 

=============================================================================
*/
void Character::AttachProp( InstDynaPhysDSG* pProp )
{
    /*
    int i;
    for ( i = 0; i < MAX_PROPS; i++ )
    {
        if ( mPropList[ i ].mpProp == 0 )
        {
            tRefCounted::Assign( mPropList[ i ].mpProp, pProp );
            GetPuppet( )->AttachProp( mPropJoint, mPropList[ i ].mpPose );
            
            mPropList[ i ].mpProp->GetSimState()->GetCollisionObject()->SetCollisionEnabled( false );
            mPropList[ i ].mpProp->GetSimState()->SetControl( sim::simAICtrl );
            break;
        }
    }
    */
}
/*
==============================================================================
Character::RemoveProp
==============================================================================
Description:    Comment

Parameters:     ( InstDynaPhysDSG* pProp )

Return:         void 

=============================================================================
*/
void Character::RemoveProp( InstDynaPhysDSG* pProp )
{
    /*
    int i;
    for ( i = 0; i < MAX_PROPS; i++ )
    {
        if ( mPropList[ i ].mpProp == pProp )
        {
            tRefCounted::Release( mPropList[ i ].mpProp );
            GetPuppet( )->RemoveAttachedProp( mPropList[ i ].mpPose );
            break;
        }
    }
    */
}
/*
==============================================================================
Character::UpdateProps
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Character::UpdateProps( float timeins )
{
    /*
    int i;
    for ( i = 0; i < MAX_PROPS; i++ )
    {
        if ( mPropList[ i ].mpProp != 0 )
        {
            mPropList[ i ].mpProp->GetSimState()->SetTransform( mPropList[ i ].mpPose->GetJoint( 0 )->worldMatrix, timeins );
            mPropList[ i ].mpProp->Update( timeins );

            CharacterController::eIntention theIntention = GetController()->GetIntention();
            if( CharacterController::DoAction == theIntention )
            {
                // Throw the sum'bitch.
                //
                sim::SimState* pSimState = mPropList[ i ].mpProp->GetSimState();
                rAssert( pSimState );
                mPropList[ i ].mpProp->AddToSimulation();
                GetFacing( pSimState->VelocityState( ).mLinear );
                static float sfUpVelocity = 3.0f;
                pSimState->VelocityState( ).mLinear.y = sfUpVelocity;
                pSimState->GetCollisionObject()->SetCollisionEnabled( true );
                
                // Not the most efficient.
                //
                RemoveProp( mPropList[ i ].mpProp );
            }
        }
    }
    */
}



void Character::UpdatePhysicsObjects( float timeins, int area )
{
    rAssert( area != -1 );
    RestTest();
    GetWorldPhysicsManager()->UpdateDynamicObjects(timeins, area );
    GetWorldPhysicsManager()->UpdateAnimCollisions(timeins, area );
}

/*
==============================================================================
Character::SubmitStatics
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::SubmitStatics( void )
{
    BEGIN_PROFILE( "Per Character Submit STatics" );
	if ( GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer() )
	{            
        // Always test because we need to dump stuff while we are in the car.
        // Dynaimc loading rides again
        //
        // TBJ [8/14/2002] 
        //
        if( true ) //!(IsInCar()))
        {
            int collisionAreaIndex = GetCollisionAreaIndex();
            if ( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
            {
                rmt::Vector position;
                GetPosition( position );
                static float sfCollisionRadius = 5.0f;
                GetWorldPhysicsManager()->SubmitStaticsPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState(), true);

                GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState(), true);
            }
        }
    }
    END_PROFILE( "Per Character Submit STatics" );
}

/*
==============================================================================
Character::SubmitAnimCollisions
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::SubmitAnimCollisions( void )
{
    BEGIN_PROFILE( "Per Character Submit Anims" );
    if ( GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer() ) 
    {
        // Always test because we need to dump stuff while we are in the car.
        // Dynaimc loading rides again
        //
        // This is also nice because objects will animate while we are in the car.
        //
        // TBJ [8/14/2002] 
        //
        if( true ) //!(IsInCar()))
        {
            int collisionAreaIndex = GetCollisionAreaIndex();
            if ( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
            {
                rmt::Vector position;
                GetPosition( position );

                static float sfCollisionRadius = 1.5f;
                GetWorldPhysicsManager()->SubmitAnimCollisionsPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState());
                static float sfUpdateRadius = 30.0f;//0.0f;
                GetWorldPhysicsManager()->SubmitAnimCollisionsForUpdateOnly( position, sfUpdateRadius, collisionAreaIndex );
            }
        }
    }
    END_PROFILE( "Per Character Submit Anims" );
}

/*
==============================================================================
Character::SubmitDynamics
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Character::SubmitDynamics( void )
{
    BEGIN_PROFILE( "Per Character Submit Dyn" );
    if ( GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer() )
    {
        // Always test because we need to dump stuff while we are in the car.
        // Dynaimc loading rides again
        //
        // This is also nice because objects will animate while we are in the car.
        //
        // TBJ [8/14/2002] 
        //
        if( true ) //!(IsInCar()))
        {
            int collisionAreaIndex = GetCollisionAreaIndex();
            if ( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
            {
                rmt::Vector position;
                GetPosition( position );

                static float sfCollisionRadius = 10.5f;
                GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState(), true);
            }
        }
    }
    END_PROFILE( "Per Character Submit Dyn" );
}

bool Character::PosInFrustrumOfPlayer( const rmt::Vector& pos, int playerID )
{
    return GetGameplayManager()->TestPosInFrustrumOfPlayer( pos, playerID );
}

void Character::TestInFrustrumOfPlayer( int playerID )
{
    if( PosInFrustrumOfPlayer( mSphere.centre, playerID ) )
    {
        mbInAnyonesFrustrum = true;
    }
}

void Character::SetShadowColour( tColour shadowColour )
{
    mShadowColour = shadowColour;
    if( mpCharacterRenderable )
    {
        mpCharacterRenderable->SetShadowColour( shadowColour );
    }
}

tColour Character::GetShadowColour()
{
    return mShadowColour;
}

void Character::SetSwatch( int swatchNum )
{
    rAssert( mpCharacterRenderable != NULL );
    mpCharacterRenderable->SetSwatch( swatchNum );
}

void Character::SetDrawable( CharacterRenderable* pDrawable )
{
    if(mpCharacterRenderable)
    {
        delete mpCharacterRenderable;
    }
    mpCharacterRenderable = pDrawable;
}

void Character::Shock( float timeInSeconds )
{
    if ( mpCharacterRenderable )
    {
        m_TimeLeftToShock = timeInSeconds;     
        m_IsBeingShocked = true;
        
        mpCharacterRenderable->SetShocked( true );

        GetEventManager()->TriggerEvent( EVENT_WASP_BULLET_HIT_CHARACTER_STYLIZED_VIOLENCE_FOLLOWS );

        //
        // Throw KICK_NPC_SOUND to trigger the HitByW dialogue that Chris and Cory want -- Esan
        //
        GetEventManager()->TriggerEvent( EVENT_KICK_NPC_SOUND, this );
    }
}

void Character::DoKickwave(void)
{
    if(!mKickwave)
    {
        tRefCounted::Assign(mKickwave, p3d::find<tDrawable>("kickwave"));
        tRefCounted::Assign(mKickwaveController, p3d::find<tFrameController>("kickwave"));
    }

    if(mKickwave)
    {
        mDoKickwave = true;
        mKickwaveController->SetFrame(0);
    }
}

void Character::OnTransitToAICtrl()
{
    mPrevSimTransform = GetSimState()->GetTransform();
    mGroundPlaneSimState->GetCollisionObject()->SetCollisionEnabled( false );    

    // get "up" out of sim control
    rmt::Vector up, right, forward;
    //right = mpCharacter->mPrevSimTransform.Row(0);
    up = mPrevSimTransform.Row(1);
    //forward = mpCharacter->mPrevSimTransform.Row(2);

    float dir = choreo::GetWorldAngle( up.x, up.z );
    RelocateAndReset( mPrevSimTransform.Row(3), dir );
}

void Character::Display(void)
{
    if(IS_DRAW_LONG) return;
    DSG_BEGIN_PROFILE("  Character::Display")
    if(!mpCharacterRenderable->GetDrawable())
    {
        return;
    }
    
    if( mbNeedChoreoUpdate)
    {
        mpPuppet->UpdatePose( );
		mbNeedChoreoUpdate = false;
    }

    if(IsMarge() && 
    ((GetStateManager()->GetState() == CharacterAi::INCAR) || 
        (GetStateManager()->GetState() == CharacterAi::GET_IN) || 
        (GetStateManager()->GetState() == CharacterAi::GET_OUT)) &&
        GetTargetVehicle() &&
        GetTargetVehicle()->mHighRoof)
    {
        poser::Pose* p = mpPuppet->GetPose();
        int numJoints = p->GetJointCount();
        if( numJoints >= 36 )
        {
            mpPuppet->GetPose()->GetJoint(33)->SetObjectMatrix(mpCharacterRenderable->GetDrawable()->GetSkeleton()->GetJoint(33)->restPose);
            mpPuppet->GetPose()->GetJoint(34)->SetObjectMatrix(mpCharacterRenderable->GetDrawable()->GetSkeleton()->GetJoint(34)->restPose);
            mpPuppet->GetPose()->GetJoint(35)->SetObjectMatrix(mpCharacterRenderable->GetDrawable()->GetSkeleton()->GetJoint(35)->restPose);
        }
    }

    mpPuppet->GetP3DPose()->SetSkeleton(mpCharacterRenderable->GetDrawable()->GetSkeleton());
    mpPuppet->UpdateEnd( );

    static float JumpRatio = 0.0f;
    if( JumpRatio == 0.0f && ( GetJumpHeight() != 0.0f ) )
    {
        JumpRatio = 0.5f / GetJumpHeight();
    }
	if( !IsInCar() && IsSimpleShadow() )
	{
		rmt::Vector groundPos;
		rmt::Vector groundNormal;
        rmt::Vector characterFacing;
		GetTerrainIntersect( groundPos, groundNormal );
        GetFacing( characterFacing );
        struct BlobShadowParams p( groundPos, groundNormal, characterFacing );
        const rmt::Vector& pos = rPosition();
        p.ShadowScale = 1.0f - ( ( pos.y - groundPos.y ) * JumpRatio );
        p.ShadowAlpha = p.ShadowScale * ( mInteriorTerrain ? 0.5f : 1.0f );
        p3d::pddi->SetZWrite(false);
        mpCharacterRenderable->DisplayShadow( mpPuppet->GetP3DPose(), &p );
        p3d::pddi->SetZWrite( true );
	}

    tPose* pose = mpPuppet->GetP3DPose();

    mLean = pose->GetJoint(17)->worldMatrix.Row(3);
    mLean.Sub(pose->GetJoint(0)->worldMatrix.Row(3));
    mLean.NormalizeSafe();

    rmt::Matrix backToTheOrigin;

    backToTheOrigin.Identity();
    backToTheOrigin.Row(3) = pose->GetJoint(0)->worldMatrix.Row(3);
    backToTheOrigin.InvertOrtho();

    rmt::Vector rootPos = pose->GetJoint(0)->worldMatrix.Row(3);
    
    bool shouldScale = mScale != 1.0f;

    for(int i = 0; i < pose->GetNumJoint(); i++)
    {
        rmt::Matrix tmp;
        tmp.Mult(pose->GetJoint(i)->worldMatrix, backToTheOrigin);
        if(shouldScale)
        {
            rmt::Matrix scale;
            scale.Identity();
            scale.FillScale(mScale, mScale, mScale);
            pose->GetJoint(i)->worldMatrix.Mult(tmp, scale);
        }
        else
        {
            pose->GetJoint(i)->worldMatrix = tmp;
        }
    }

    p3d::stack->Push();
    p3d::stack->Translate(0.0f, mYAdjust, 0.0f);

    p3d::stack->Push();
    p3d::stack->Translate(rootPos);

    mpCharacterRenderable->Display( mSphere.centre, pose );

    p3d::stack->Pop();
    p3d::stack->Pop();

    if(mDoKickwave && mKickwave)
    {
        p3d::stack->Push();
        p3d::stack->Translate(rootPos);
        p3d::stack->Multiply(pose->GetJoint(0)->worldMatrix);
        mKickwave->Display();
        p3d::stack->Pop();
    }
#ifdef DRAW_CHARACTER_COLLISION
#ifdef RAD_RELEASE
    if ( CommandLineOptions::Get( CLO_DEBUGBV ) )
#else
    if ( !mpCharacterRenderable->GetDrawable() || CommandLineOptions::Get( CLO_DEBUGBV ) )
#endif
    {
        // The sim library allocates shaders and stuff for this, so we should ensure they're on the temp heap
        //
        HeapMgr()->PushHeap (GMA_TEMP);

        sim::CollisionVolume* pVolume = GetSimState( )->GetCollisionObject()->GetCollisionVolume( );
        sim::DrawCollisionVolume( pVolume );
        if ( mpStandingCollisionVolume )
            sim::DrawCollisionVolume( mpStandingCollisionVolume );
        int i;
        for ( i = 0; i < mCurrentCollision; i++ )
        {
            sim::CollisionVolume* pVolume = mCollisionData[ i ].mpCollisionVolume;
            if ( pVolume )
                sim::DrawCollisionVolume(pVolume);
        }

        HeapMgr()->PopHeap( GMA_TEMP );
    }
#endif // DRAW_CHARACTER_COLLISION
    DSG_END_PROFILE("  Character::Display")
}

void Character::SetAmbient(const char* location, float radius)
{
    mAmbient = true;
    mAmbientLocator = tEntity::MakeUID(location);
    
    if((mRole != ROLE_REWARD) && (radius != 0.0f))
    {
        tRefCounted::Assign(mAmbientTrigger, new AmbientDialogueTrigger(this, radius));
        EnableAmbientDialogue(true);
    }
}

void Character::EnableAmbientDialogue(bool e)
{
    if(!mAmbientTrigger)
        return;

    if(e)
    {
        GetTriggerVolumeTracker()->AddTrigger(mAmbientTrigger);
    }
    else
    {
        GetTriggerVolumeTracker()->RemoveTrigger(mAmbientTrigger);
    }
}

void Character::ResetAmbientPosition(void)
{
    Locator* l = p3d::find<Locator>(mAmbientLocator);
    if(l)
    {
        rmt::Vector pos;
        l->GetPosition(&pos);
        RelocateAndReset(pos, 0.0f);
    }
    AddToWorldScene();

    static_cast<NPCController*>(GetController())->ClearTempWaypoint();
}

/////////////////////////// NPC STUFF ////////////////////////////////

void NPCharacter::UpdatePhysicsObjects( float timeins, int area )
{
    // NPCs shouldn't be submitting to physics stuff around it.
    //
    RestTest();
}

void NPCharacter::AssignCollisionAreaIndex( void )
{
    Character::AssignCollisionAreaIndex();
}


void NPCharacter::SubmitStatics( void )
{
    BEGIN_PROFILE( "Per NPCharacter Submit Statics" );

    // DUSIT [Oct 29,2002]:
    // HACK:
    // When should we submit statics around ourselves?
    // characters too far away from player shouldn't be submitting statics
    // characters standing still shouldn't submit.
    // characters not "off path" shouldn't submit.
    // characters in street races 1 & 2 shouldn't submit (or they'll pop out side
    //    the race props onto the race track).

    NPCController* npcController = (NPCController*) GetController();
    if( npcController != NULL )
    {
        NPCController::State state = npcController->GetState();
        bool npcStateNeedsToSubmit = 
            (GetStateManager()->GetState() == CharacterAi::INSIM) || 
            (
                (GetStateManager()->GetState() != CharacterAi::INSIM) && 
                (
                    (npcController->mOffPath && state == NPCController::FOLLOWING_PATH) ||
                    (state == NPCController::STOPPED ) ||
                    (state == NPCController::DODGING) ||
                    (state == NPCController::PANICKING) ||
                    (state == NPCController::TALKING_WITH_PLAYER) 
                )
            );

        if( npcStateNeedsToSubmit )
        {
            // We COULD do this to prevent code duplication. But a virtual function call
            // is quite expensive.
            //Character::SubmitStatics();

	        if ( GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer() )
	        {            
                // Always test because we need to dump stuff while we are in the car.
                // Dynaimc loading rides again
                //
                // TBJ [8/14/2002] 
                //
                int collisionAreaIndex = GetCollisionAreaIndex();
                if( collisionAreaIndex == WorldPhysicsManager::INVALID_COLLISION_AREA &&
                    GetRole() != ROLE_PEDESTRIAN )
                {
                    if(!IsInCar())
                    {       
                        AddToPhysics();
                        collisionAreaIndex = GetCollisionAreaIndex();
                    }
                }

                if( collisionAreaIndex != WorldPhysicsManager::INVALID_COLLISION_AREA ) 
                {
                    if ( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
                    {
                        rmt::Vector position;
                        GetPosition( position );
                        static float sfCollisionRadius = 1.0f;
                        GetWorldPhysicsManager()->SubmitStaticsPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState());

                        GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, sfCollisionRadius, collisionAreaIndex, GetSimState());
                    }
                }
            }
        }
        else
        {
            int collisionAreaIndex = GetCollisionAreaIndex();
            if( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
            {
                // 
                // Empty the collision area index of all submissions... 
                // unfortunately, this takes US and our GROUNDPLANE out of the list too
                // so we gotta re-add and re-pair.
                //
                GetWorldPhysicsManager()->EmptyCollisionAreaIndex( collisionAreaIndex );
                GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(
                    mpSimStateObj->GetCollisionObject(), mCollisionAreaIndex );
                //AddToPhysics();
            }
        }
    }


    END_PROFILE( "Per NPCharacter Submit Statics" );
}

void NPCharacter::SubmitDynamics( void )
{
}

void NPCharacter::OnTransitToAICtrl()
{
    // do here what we need to do at the very moment we transit
    // back from simulation control to AI control
    mPrevSimTransform = GetSimState()->GetTransform();
    
}

void NPCharacter::ApplyForce( const rmt::Vector& direction, float force )
{
    if(!IsInCar())
    {
        DynaPhysDSG::ApplyForce( direction, force );
    }
}


void NPCharacter::ApplyKickForce( const rmt::Vector& direction, float force )
{
    if(!IsInCar())
    {
        DynaPhysDSG::ApplyForce( direction, force );

        /*
        rmt::Vector& rAngular = mpSimStateObj->GetAngularVelocity();
        float deltaV = force / 100.0f;
        rAngular += (direction * deltaV);
        */

        rmt::Vector linVel = mpSimStateObj->GetLinearVelocity();
        mPastLinear.SetAverage( linVel.Magnitude() );

        rmt::Vector angVel = mpSimStateObj->GetAngularVelocity();
        mPastAngular.SetAverage( angVel.Magnitude() );

    }
}


