//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ActorDSG
//
// Description: ActorDSG is a statepropdsg that has an id that will indicate the
//              relationship between an object and the objects that it can shoot
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/actordsg.h>
#include <ai/actor/actormanager.h>
#include <render/breakables/breakablesmanager.h>
#include <stateprop/statepropdata.hpp>
#include <p3d/matrixstack.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <debug/profiler.h>
#include <worldsim/coins/coinmanager.h>
#include <render/intersectmanager/intersectmanager.h>
#include <ai/actor/intersectionlist.h>
#include <constants/actorenum.h>
#include <p3d/view.hpp>
#include <p3d/light.hpp>
#include <raddebug.hpp>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

unsigned int ActorDSG::s_IDCounter = 0;

static const int REMOVE_FROM_WORLD = 0;
static const int SPAWN_5_COINS = 1;
static const int REMOVE_COLLISION_VOLUME = 2;
static const int FIRE_ENERGY_BOLT = 3;
static const int KILL_SPEED = 4;
static const int SPAWN_10_COINS = 5;
static const int SPAWN_15_COINS = 6;
static const int SPAWN_20_COINS = 7;

// States for the shield
static const unsigned int SHIELD_STATE_POWER_UP = 0;
static const unsigned int SHIELD_STATE_IDLE = 1;
static const unsigned int SHIELD_STATE_HIT = 2;
static const unsigned int SHIELD_STATE_DESTROYED = 3;

// States for the tractor beam
static const unsigned int TRACTOR_BEAM_ACTIVATE = 0;
static const unsigned int TRACTOR_BEAM_ON = 1;
static const unsigned int TRACTOR_BEAM_DEACTIVATE = 2;
static const unsigned int TRACTOR_BEAM_OFF = 3;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

class CStateProp;
static tUID s_UfoName = tName::MakeUID("spaceship");

//===========================================================================
// Member Functions
//===========================================================================



//===========================================================================
// Debugging / Watching
//===========================================================================

static float AMB_RED_SCALE = 2.0f;
static float AMB_GREEN_SCALE = 2.0f;
static float AMB_BLUE_SCALE = 2.0f;
static float DIR_RED_SCALE = 1.0f;
static float DIR_GREEN_SCALE = 1.0f;
static float DIR_BLUE_SCALE = 1.0f;



ActorDSG::ActorDSG():
m_ID( ++s_IDCounter ),
m_ShieldProp( NULL ),
m_TractorBeamProp( NULL ),
m_ShieldEnabled( false ),
mPhysicsProperties( NULL )
{
}

ActorDSG::~ActorDSG()
{
    if ( m_ShieldProp != NULL )
    {
        m_ShieldProp->Release();
        m_ShieldProp = NULL;
    }
    if ( m_TractorBeamProp != NULL )
    {
        m_TractorBeamProp->ReleaseVerified();
        m_TractorBeamProp = NULL;
    }
    if ( mPhysicsProperties != NULL )
    {
        mPhysicsProperties->Release();
    }
}

sim::Solving_Answer 
ActorDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{  
    // Check to see that we aren't going to collide with an object that has the same ID
    // that we do
    if ( pCollidedObj->mAIRefIndex == PhysicsAIRef::ActorStateProp )
    {
        rAssert( dynamic_cast< ActorDSG* >( static_cast< tRefCounted* >( pCollidedObj->mAIRefPointer) ) );
        ActorDSG* pStatePropDSG = static_cast< ActorDSG* >( pCollidedObj->mAIRefPointer );
        if ( pStatePropDSG->GetID() == GetID() )
        {
            return sim::Solving_Aborted;
        }
    }
    HandleEvent( StatePropDSG::FEATHER_TOUCH );
   
    return sim::Solving_Continue;
}
        
void 
ActorDSG::SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector)
{
    if ( mpStateProp->GetUID() != s_UfoName )
    {
        IEntityDSG::SetRank( irRefPosn, mViewVector );
    }
    else
    {
        // Final hack to make the UFO always draw last
        // The UFO has a *huge* bounding sphere that makes it
        // always drawn after the vehicle explosion, cutting it
        // off nastily, lets fix it and force the thing to be drawn first
        mRank = FLT_MAX;
    }
}


sim::Solving_Answer 
ActorDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
    // Calculate impulse
    float impulseMagSqr = impulse.MagnitudeSqr();
    // If the shield is on, impulse has to be above an very high number to cause it be
    // be destroyed

    const float FORCE_REQUIRED_TO_BREAK_SHIELD = 1600000;
    const float FORCE_REQUIRE_TO_DESTROY = 800000;

    if ( m_ShieldEnabled )
    {
        if ( impulseMagSqr > FORCE_REQUIRED_TO_BREAK_SHIELD )
        {   
            DestroyShield();
        }
        else
        {
            PlayShieldGettingHit();
        }
    }
    else
    {
        if ( impulseMagSqr > FORCE_REQUIRE_TO_DESTROY )
        {        
            HandleEvent( DESTROYED );
        }
    }

    AddToSimulation();

    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);
}

        
void ActorDSG::ApplyForce( const rmt::Vector& direction, float force )
{
    // Force over a certain     
    const float SHIELD_BREAKING_FORCE = 400.0f;
    const float FORCE_REQUIRED_TO_DESTROY = 400.0f;
    if ( m_ShieldEnabled )
    {
        if ( force < SHIELD_BREAKING_FORCE )
        {
            PlayShieldGettingHit();    
        }
        else
        {
            DestroyShield();
        }
    }
    else
    {
        if ( force >= FORCE_REQUIRED_TO_DESTROY )
        {
            DestroyShield();
            HandleEvent( StatePropDSG::DESTROYED );
            DynaPhysDSG::ApplyForce( direction, force );
        }
    }
}


void 
ActorDSG::RecieveEvent( int callback , CStateProp* stateProp )
{
    switch ( callback )
    {
    case FIRE_ENERGY_BOLT:
        {
            tName name = "waspray";
            GetActorManager()->FireProjectile( name.GetUID(), mPosn, -mTransform.Row(2), m_ID );
        }
        break;
    case REMOVE_FROM_WORLD:
        GetActorManager()->RemoveActorByDSGPointer( this, true );
        break;
    default:
        StatePropDSG::RecieveEvent( callback, stateProp );
        break;
    };
}

void 
ActorDSG::HandleEvent( StatePropDSG::Event event )
{
    // If a shield is enabled, then only destroyed events should get 
    StatePropDSG::HandleEvent( event );
}


bool 
ActorDSG::LoadShield( const char* statePropName )
{
    bool loadedOk = false;
    CStatePropData* statePropData = p3d::find< CStatePropData > ( statePropName );
    if ( statePropData != NULL )
    {
        CStateProp* stateprop = CStateProp::CreateStateProp( statePropData, 0 );
        if ( stateprop != NULL )
        {
            m_ShieldEnabled = true;
            tRefCounted::Assign( m_ShieldProp, stateprop);
            loadedOk = true;
            SetPhysicsProperties( 100.0f, 0.8f, 1.15f, 0.0f );
        }
    }
    return loadedOk;
}

bool 
ActorDSG::LoadTractorBeam( const char* statePropName )
{
    bool loadedOk = false;
    CStatePropData* statePropData = p3d::find< CStatePropData > ( statePropName );
    if ( statePropData != NULL )
    {
        CStateProp* stateprop = CStateProp::CreateStateProp( statePropData, TRACTOR_BEAM_OFF );
        if ( stateprop != NULL )
        {
            tRefCounted::Assign( m_TractorBeamProp, stateprop);
            loadedOk = true;
        }
    }
    return loadedOk;
}

void 
ActorDSG::ActivateTractorBeam()
{
    if ( m_TractorBeamProp != NULL )
    {
        if ( m_TractorBeamProp->GetState() == TRACTOR_BEAM_OFF )
        {
            m_TractorBeamProp->SetState( TRACTOR_BEAM_ACTIVATE );
        }
    }
}
 
void 
ActorDSG::DeactivateTractorBeam()
{
    if ( m_TractorBeamProp != NULL )
    {
        if ( m_TractorBeamProp->GetState() == TRACTOR_BEAM_ON ||
             m_TractorBeamProp->GetState() == TRACTOR_BEAM_ACTIVATE )
        {
            m_TractorBeamProp->SetState( TRACTOR_BEAM_DEACTIVATE );
        }
    }
}

bool 
ActorDSG::IsTractorBeamOn()const
{
    if ( m_TractorBeamProp == NULL )
        return false;

    return m_TractorBeamProp->GetState() != TRACTOR_BEAM_OFF;
}


void 
ActorDSG::Display()
{
    // Have the rad debugging functions been attached yet?
    // Check, and if not, add them once
#ifndef RAD_RELEASE

    static bool radattached = false;

    if ( radattached == false )
    {
        radDbgWatchAddFloat( &AMB_RED_SCALE, "Amb Red Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radDbgWatchAddFloat( &AMB_GREEN_SCALE, "Amb Green Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radDbgWatchAddFloat( &AMB_BLUE_SCALE, "Amb Blue Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radDbgWatchAddFloat( &DIR_RED_SCALE, "Dir Red Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radDbgWatchAddFloat( &DIR_GREEN_SCALE, "Dir Green Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radDbgWatchAddFloat( &DIR_BLUE_SCALE, "Dir Blue Scale", "Wasp lighting", NULL, NULL, 0, 4.0f );
        radattached = true;
    }


#endif

    // Aryan and Yayoi want the beecamera brightened artificially
    // There doesnt appear to be a nice + quick solution 
    // Do a quick UID compare then brighten the level lights
    // We will reset them after drawing is finished
    static tUID beecameraUID = tName::MakeUID( "beecamera" );
    tColour origColours[MAX_VIEW_LIGHTS];
    tColour origAmbient;
    if ( mpStateProp->GetUID() == beecameraUID )
    {
        tView* view = p3d::context->GetView();
        for ( unsigned int i = 0 ; i < MAX_VIEW_LIGHTS ; i ++ )
        {
            tLight* light = view->GetLight( i );
            if ( light )
            {
                origColours[i] = light->GetColour();
 
                int r = (int)( (float) origColours[i].Red() * DIR_RED_SCALE );
                int g = (int)( (float) origColours[i].Green() * DIR_GREEN_SCALE );
                int b = (int)( (float) origColours[i].Blue() * DIR_BLUE_SCALE );

                if ( r > 255 ) r = 255;
                if ( g > 255 ) g = 255;
                if ( b > 255 ) b = 255;

                tColour newColour( r, g, b );
                light->SetColour( newColour );
            }
        }
        origAmbient = p3d::pddi->GetAmbientLight();

        int r = (int)( (float) origAmbient.Red() * AMB_RED_SCALE );
        int g = (int)( (float) origAmbient.Green() * AMB_GREEN_SCALE );
        int b = (int)( (float) origAmbient.Blue() * AMB_BLUE_SCALE );

        if ( r > 255 ) r = 255;
        if ( g > 255 ) g = 255;
        if ( b > 255 ) b = 255;

        tColour scaledAmbient( r, g, b );
        p3d::pddi->SetAmbientLight( scaledAmbient );
    }


#ifdef PROFILER_ENABLED
    char profileName[] = "  ActorDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    StatePropDSG::Display();
    if ( m_ShieldProp != NULL )
    {
        // Draw the shield
        p3d::stack->PushMultiply( mTransform );
        m_ShieldProp->UpdateFrameControllersForRender();
        m_ShieldProp->GetDrawable()->Display();
        p3d::stack->Pop();
    }
    if ( m_TractorBeamProp != NULL )
    {
        // Draw the tractor beam
        p3d::stack->PushMultiply( mTransform );
        m_TractorBeamProp->Display();
        p3d::stack->Pop();
    }

    // Reset the level lights for the beecamera
    if ( mpStateProp->GetUID() == beecameraUID )
    {
        tView* view = p3d::context->GetView();
        for ( unsigned int i = 0 ; i < MAX_VIEW_LIGHTS ; i ++ )
        {
            tLight* light = view->GetLight( i );
            if ( light )
            {
                light->SetColour(  origColours[i] );
            }
        }
        // Reset the ambient    
        p3d::pddi->SetAmbientLight( origAmbient );
    }


    DSG_END_PROFILE(profileName)

}

void 
ActorDSG::AdvanceAnimation( float timeInMS )
{
    if ( m_ShieldProp )
    {
        m_ShieldProp->Update( timeInMS );
    }
    if ( m_TractorBeamProp )
    {
        m_TractorBeamProp->Update( timeInMS );
    }


    StatePropDSG::AdvanceAnimation( timeInMS );




}

void 
ActorDSG::DestroyShield()
{
    if ( m_ShieldProp != NULL )
    {
        m_ShieldEnabled = false;
        m_ShieldProp->SetState( SHIELD_STATE_DESTROYED );
    }
}
        
void 
ActorDSG::PlayShieldGettingHit()
{
    if ( m_ShieldProp != NULL )
    {
        if ( m_ShieldProp->GetState() != SHIELD_STATE_HIT )
        {
            m_ShieldProp->SetState( SHIELD_STATE_HIT );
        }
    }    
}
        
void 
ActorDSG::RestoreShield()
{
    if ( m_ShieldProp )
    {
        m_ShieldEnabled = true;
        m_ShieldProp->SetState( SHIELD_STATE_POWER_UP );
    }
}

void 
ActorDSG::SetPhysicsProperties( float mass, float friction, float rest, float tang )
{

    if ( mPhysicsProperties == NULL )
        return;

    // We want to grab the volume so we can compute the density via the given mass
    // and the simstate's volume
    sim::SimStateArticulated* pSimStateArt = ( sim::SimStateArticulated* )( mpSimStateObj );
    sim::ArticulatedPhysicsObject* physicsObject = static_cast< sim::ArticulatedPhysicsObject*>( pSimStateArt->GetSimulatedObject( -1 ));
    float volume = physicsObject->GetVolume();

    rAssert( volume != 0 );

    float density = mass / volume;

    mPhysicsProperties->SetRestCoeffCGS( rest );
    mPhysicsProperties->SetFrictCoeffCGS( friction );
    mPhysicsProperties->SetTangRestCoeffCGS( tang );
    mPhysicsProperties->SetDensityCGS( density );

    mpSimStateObj->SetPhysicsProperties( mPhysicsProperties );
}

void 
ActorDSG::LoadSetup( CStatePropData* statePropData, 
                     int startState, 
                     const rmt::Matrix& transform,
                     CollisionAttributes* ipCollAttr,
                     bool isMoveable,
                     bool isDynaLoaded,
                     tEntityStore* ipSearchStore  )
{

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    sim::PhysicsProperties* physicsProperties = new sim::PhysicsProperties;

    StatePropDSG::LoadSetup( statePropData, startState, transform, ipCollAttr, isDynaLoaded, ipSearchStore );

    tRefCounted::Assign( mPhysicsProperties, physicsProperties );
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

}

void 
ActorDSG::GenerateCoins( int numCoins )
{

    // Generate coins, but take into account the fact that this object might be hovering 
    // over the ground.
    // So generate coins at the top of the object below the stateprop
    // First find the ground plane
    bool foundPlane;
    rmt::Vector deepestPosn, deepestNormal, groundPlaneNormal, groundPlanePosn;
    rmt::Vector searchPosn = rPosition();
    searchPosn.y += 100.0f;
    GetIntersectManager()->FindIntersection( searchPosn, foundPlane, groundPlaneNormal, groundPlanePosn );
    if ( foundPlane == false )
    {
        groundPlanePosn = rPosition();
        groundPlanePosn.y -= 30.0f;
    }
    // Check to see if there any static objects directly underneath us
    // find the midpoint between the current position and the ground position
    rmt::Vector midpoint = ( groundPlanePosn + rPosition() ) * 0.5f;
    float radius = (rPosition().y - groundPlanePosn.y );
    ReserveArray< StaticPhysDSG* > spList(200);
    GetIntersectManager()->FindStaticPhysElems( midpoint, radius , spList );
    // Iterate through the static phys list and add them to an intersection list
    IntersectionList staticList;
    for ( int i = 0 ; i < spList.mUseSize; i++ )
    {
        staticList.AddStatic( spList[i]->GetSimState() );
    }
    // Now query our list using subcollision volume accuracy to pinpoint the precise location
    // of intersection
    rmt::Vector staticIntersection;
    rmt::Vector start = rPosition();
    rmt::Vector end = groundPlanePosn;
    if ( staticList.TestIntersectionStatics( start, end, &staticIntersection ))
    {
        // There is a static in the way. spawn teh coins on top of this object
        GetCoinManager()->SpawnCoins( numCoins, rPosition(), staticIntersection.y );           
    }
    else
    {
        // The wasp is hovering above solid ground. Spawn the coins on the ground
        GetCoinManager()->SpawnCoins( numCoins, rPosition(), groundPlanePosn.y );
    }

}

int
ActorDSG::CastsShadow()
{
    // Dont cast a shadow in the first (fade in) and last(exploded) states
    int castsShadow;

    unsigned int statepropState = GetState();
    if ( statepropState == ActorEnum::eFadeIn || statepropState == ActorEnum::eDestroyed )
        castsShadow = 0;
    else
        castsShadow = StaticPhysDSG::CastsShadow();  

    return castsShadow;
}




