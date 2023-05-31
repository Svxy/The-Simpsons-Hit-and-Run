//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   statepropcollectible
//
// Description: Exactly like a normal stateprop, except it will
//              identify low-speed collisions with the player vehicle and 
//              then attach itself to the car
//
// Authors:     Michael Riegger
//
//===========================================================================


//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/statepropcollectible.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/rendermanager.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <ai/actor/intersectionlist.h>
#include <events/eventenum.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <events/eventmanager.h>
#include <constants/statepropenum.h>
#include <worldsim/avatarmanager.h>
#include <events/eventdata.h>
#include <worldsim/avatar.h>
#include <worldsim/character/character.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

// How long to hold a camera shake
const unsigned int STATEPROP_COLLECTIBLE_CAMERA_SHAKE_TIME = 1000;
// Shake intensity.
float STATEPROP_SHAKE_FORCE = 300;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

StatePropCollectible::StatePropCollectible():
m_HudIcon( -1 ),
m_IsInDSG( false ),
m_CollisionTestingEnabled( false ),
m_ShakeStartTime( 0 ),
m_CameraShaking( false )
{
#ifndef RAD_RELEASE
    static bool wasAdded = false;
    if ( wasAdded == false )
    {
        radDbgWatchAddFloat( &STATEPROP_SHAKE_FORCE, "Stateprop shake force", "Stateprop", NULL, NULL, 0, 400.0f );
        wasAdded = true;
    }
#endif
}

StatePropCollectible::~StatePropCollectible()
{
    EnableHudIcon( false );
    RemoveFromDSG();
}


sim::Solving_Answer
StatePropCollectible::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
    // What we want to do is check to see if the collided object is a player vehicle. If it is, then 
    // check its velocity
    // if the velocity is low
    // abort the collision and attach the object to the vehicle.
    sim::Solving_Answer answer = sim::Solving_Aborted;
    
    bool computeStatePropPreReact = true;

    if ( pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickVehicle )
    {
        rAssert( dynamic_cast< Vehicle* >( static_cast< tRefCounted* >( pCollidedObj->mAIRefPointer) ) );
        Vehicle* vehicle = static_cast< Vehicle* >( pCollidedObj->mAIRefPointer );
        if ( vehicle->IsUserDrivingCar() )
        {
            // Attach it to the vehicle, if there is room
            // Remember to check return value, there may not be a slot left
            // and it won't attach  

            // Lets not attach it the object is moving around
            sim::SimState* pSimState = GetSimState();
            const rmt::Vector& linearVelocity = pSimState->GetLinearVelocity();
            if ( linearVelocity.MagnitudeSqr() < 100.0f )
            {
                if ( vehicle->AttachCollectible( this ) )
                {   // It attached ok.
                    rmt::Matrix origin;
                    origin.Identity();
                    // Send this thing to the origin, as when the vehicle
                    // draws it, it can't be translated out to worldspace
                    SetTransform( origin );
                    // Remove it from the DSG. No point in doing DSG stuff
                    // when the car is also in the DSG
                    computeStatePropPreReact = false;
                    answer = sim::Solving_Aborted;
                }
            }
        }
    }

    if ( computeStatePropPreReact )
        answer = StatePropDSG::PreReactToCollision( pCollidedObj, inCollision );
    
    return answer;
}

// Overloaded AdvanceAnimation. Same as the normal statepropdsg::advanceanimation
// but also keeps the object in sim mode
void 
StatePropCollectible::AdvanceAnimation( float timeInMS )
{
    // Lets make sure that the collectibles never go out of sim mode
    sim::SimState* pSimState = GetSimState();
 
    // There is a problem with dynamic objects under physics that move outside the update
    // sphere around vehicles or players. Namely they will happily

    mpStateProp->Update( timeInMS );
    if ( IsCollisionTestingEnabled() )
        DoCollisionTesting();
    // Update position
    GetPosition( &m_PreviousPosition );

    // Check camera shake
    if ( m_CameraShaking )
    {
        // SHAKE!
        CameraShake();

        // Check that we should still be shaking
        unsigned int currTime = radTimeGetMilliseconds();
        if ( currTime > m_ShakeStartTime + STATEPROP_COLLECTIBLE_CAMERA_SHAKE_TIME )
            m_CameraShaking = false;
    }
}

void StatePropCollectible::SetTransform( const rmt::Matrix& matrix )
{
    m_PreviousPosition = matrix.Row(3);
    if ( IsInDSG() )
    {
        StatePropDSG::SetTransform( matrix );
    }
}

void StatePropCollectible::SetPosition( const rmt::Vector& position )
{
    m_PreviousPosition = position;
    StatePropDSG::SetPosition( position );
}

void StatePropCollectible::LoadSetup( CStatePropData* statePropData, 
                        int startState,
                        const rmt::Matrix& transform,
                        CollisionAttributes* ipCollAttr,
                        bool isDynaLoaded,
                        tEntityStore* ipSearchStore,
                        bool useSharedtPose,
                        sim::CollisionObject* collisionObject,
                        sim::PhysicsObject* physicsObject )
{
    StatePropDSG::LoadSetup( statePropData, startState, transform, ipCollAttr, isDynaLoaded, ipSearchStore, useSharedtPose, collisionObject, physicsObject );
    m_PreviousPosition = transform.Row(3);                             
    mOriginalLocation = transform;
}


void StatePropCollectible::AddToDSG()
{
    if ( m_IsInDSG == false )
    {
        WorldRenderLayer* wrl = static_cast< WorldRenderLayer* > ( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ) );
        wrl->pWorldScene()->Add( this );
        m_IsInDSG = true;
    }
}

void StatePropCollectible::RemoveFromDSG()
{
    if ( m_IsInDSG )
    {
        WorldRenderLayer* wrl = static_cast< WorldRenderLayer* > ( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ) );
        wrl->pWorldScene()->Remove( this );   
        m_IsInDSG = false;
    }
}

void StatePropCollectible::Update( float dt )
{
    if ( IsInDSG() )
        StatePropDSG::Update( dt );
}

void StatePropCollectible::RecieveEvent( int callback , CStateProp* stateProp )
{
    switch ( callback )
    {
        case StatePropEnum::eRemoveFromWorld:
            RemoveFromDSG();
            break;        
        case StatePropEnum::eObjectDestroyed:
            GetEventManager()->TriggerEvent( EVENT_STATEPROP_COLLECTIBLE_DESTROYED );
            break;
        case StatePropEnum::eCamShake:
            
            m_CameraShaking = true;
            m_ShakeStartTime = radTimeGetMilliseconds();

            break;
        default:
            StatePropDSG::RecieveEvent( callback, stateProp );
            break;
    }


}

void StatePropCollectible::Explode()
{
    SetState( 2 );
    GetEventManager()->TriggerEvent( EVENT_BARREL_BLOWED_UP );
}

void StatePropCollectible::GetPosition( rmt::Vector* currentLoc )
{
    *currentLoc = mTransform.Row(3);
}

void StatePropCollectible::GetHeading( rmt::Vector* heading )
{
    *heading = mTransform.Row(2);
}

void StatePropCollectible::EnableHudIcon( bool enable )
{
        
    CGuiScreenHud* currentHud = GetCurrentHud();       
    if ( currentHud == NULL )
        return;

    if ( enable )
    {
        if ( m_HudIcon == -1 )
            m_HudIcon = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_COLLECTIBLE, mTransform.Row(3), this, true );
    }
    else
    {
        if ( m_HudIcon != -1 )
        {
            currentHud->GetHudMap( 0 )->UnregisterIcon( m_HudIcon );
            m_HudIcon = -1;
        }
    }

}   


void 
StatePropCollectible::DoCollisionTesting()
{
    return;
    IntersectionList intersectList;       
    rmt::Vector currPosition;
    GetPosition( &currPosition );
    float radiusSqr = ( currPosition - m_PreviousPosition ).MagnitudeSqr(); 
    // Lets use radiusSqr instead of doing the sqrt call
    // It *should* be a small value anyway, < 1 meter.
    intersectList.FillIntersectionListStatics( currPosition, radiusSqr );
 
    bool los = intersectList.LineOfSight( currPosition, m_PreviousPosition );
    if ( los == false )
    {
        // We just travelled through a static object ( staticphys or fence piece )
        // Damn physics
        // Lets kill any velocity that this thing has and set position to be m_PreviousPosition
        GetSimState()->ResetVelocities();
        GetSimState()->SetControl( sim::simAICtrl );
        SetPosition( m_PreviousPosition );
    }

}
void
StatePropCollectible::CameraShake()
{ 
    // Shake the camera by a goodly amount
    ShakeEventData shakeData; 
    shakeData.playerID = 0;  //Hack... 
    shakeData.force = STATEPROP_SHAKE_FORCE;  // 

    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer(0);
    // Get direction, player to stateprop
    if ( avatar )
    {   
        rmt::Vector contactPointToCenter; 

        Character* character = avatar->GetCharacter();
        rmt::Vector charPosition;
        character->GetPosition( charPosition );
               
        contactPointToCenter.Sub( charPosition, rPosition() ); 
        contactPointToCenter.NormalizeSafe(); 
        shakeData.direction = rmt::Vector(0,1,0); 
        GetEventManager()->TriggerEvent( EVENT_CAMERA_SHAKE, &shakeData ); 
    }        
}