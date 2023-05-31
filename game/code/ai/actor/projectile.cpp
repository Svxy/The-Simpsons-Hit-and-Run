//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   projectile
//
// Description: a weapon actor
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/projectile.h>   
#include <stateprop/statepropdata.hpp>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <atc/atcmanager.h>
#include <ai/actor/projectiledsg.h>
#include <render/IntersectManager/IntersectManager.h>
#include <worldsim/character/character.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/redbrick/vehicle.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float VEHICLE_DAMAGE_ON_PROJECTILE_HIT = 0.1f;
const int COINS_LOST_BY_PLAYER_ON_HIT = 5;


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

float Projectile::s_Speed = 20.0f;
const float PROJECTILE_MAX_RANGE = 25.0f;

//===========================================================================
// Member Functions
//===========================================================================

Projectile::Projectile():
mpStateProp( NULL ),
m_WillHitStatic( false )
{
}

Projectile::~Projectile()
{
    if ( mpStateProp )
    {
        mpStateProp->Release();
        mpStateProp = NULL;
    }
}

void
Projectile::SetSpeed( float kph )
{
    // convert to meters / millisecond
    s_Speed = kph * 1000.0f / ( 3600.0f * 1000.0f );
}

bool Projectile::Init( const char* statePropName, const char* instanceName )
{
    bool success;

    CStatePropData* pStatePropData = p3d::find< CStatePropData > ( statePropName );
    if ( pStatePropData )
    {
        mp_StateProp = new ProjectileDSG();
        mp_StateProp->AddRef();

        CollisionAttributes* pCollAttr = GetATCManager()->CreateCollisionAttributes( PROP_MOVEABLE,  1, 2.2425f );
        rmt::Matrix transform;
        transform.Identity();
        mp_StateProp->LoadSetup( pStatePropData, 0, transform, pCollAttr, true, false ); 
        mp_StateProp->SetName( instanceName );       
        mp_StateProp->EnableCollisionVolume( false );

        success = true;
    }
    else 
    {
        success = false;
    }
    return success;
}

void
Projectile::Update( unsigned int timeInMS )
{
    rmt::Matrix transform;
    GetTransform( &transform );
    m_CurrentPosition = transform.Row(3);

 
    // Lets see if we would have hit anything
    int currState = mp_StateProp->GetState();
    if ( currState == 0 ) 
    {
        // Test dynamics
        FillIntersectionListDynamics( m_CurrentPosition, 10.0f );
        rmt::Vector dynamicsIntersectionPoint;
        DynaPhysDSG* objectHit;
        if ( m_IntersectionList.TestIntersectionDynamics( m_PreviousPosition, m_CurrentPosition, &dynamicsIntersectionPoint, &objectHit ) )
        {
            if ( IsJumpingPlayer( objectHit ) == false )
            {
                // we hit something, make it explode at the collisionpoint
                m_Speed = 0;
                rmt::Vector adjustedIntersection;
                AdjustCollisionPosition( dynamicsIntersectionPoint, m_CurrentPosition, m_PreviousPosition, &adjustedIntersection );
                SetPosition( adjustedIntersection );
                mp_StateProp->SetState( 1 );   
                ApplyDamage( objectHit );
            }
        }



        if ( m_WillHitStatic )
        {
            // Test to see if the projectile hit the static this frame
            // Take the dot between (Intersection - Current) and ( Intersection - Previous )
            // If negative, collision
            rmt::Vector toPrev = m_PreviousPosition - m_StaticIntersectionPoint;
            rmt::Vector toCurr = m_CurrentPosition - m_StaticIntersectionPoint;
            if ( toCurr.Dot( toPrev ) < 0 )
            {
                // we hit something, make it explode at the collisionpoint
                m_Speed = 0;
                SetPosition( m_StaticIntersectionPoint );
                rmt::Vector adjustedIntersection;
                AdjustCollisionPosition( m_StaticIntersectionPoint, m_CurrentPosition, m_PreviousPosition, &adjustedIntersection );
                SetPosition( adjustedIntersection );
                mp_StateProp->SetState( 1 );
            }
        }
        m_IntersectionList.ClearDynamics();

        {
            // Keep flying
            rmt::Vector velocityVectory = transform.Row(2) * m_Speed;
            rmt::Vector position;
            GetPosition( &position );       
            position = position + velocityVectory * static_cast< float >( timeInMS );
            SetPosition( position );
        }       
    }


    m_PreviousPosition = m_CurrentPosition;
}




// Can't see a use for projectile behaviours at this point
void Projectile::AddBehaviour( Behaviour* )
{
    
}

void Projectile::Fire()
{
    m_Speed = s_Speed;
}


void Projectile::CalculateIntersections()
{

    // Calculate the static intersection
    rmt::Matrix transform;
    GetTransform( &transform );
    const rmt::Vector& position = transform.Row(3);

    // Lets query all objects around the projectile and see if it will (eventually) 
    // hit a static object
    FillIntersectionListStatics( position, PROJECTILE_MAX_RANGE );

    rmt::Vector destination;
    destination = position + PROJECTILE_MAX_RANGE * transform.Row(2);
    m_WillHitStatic = m_IntersectionList.TestIntersectionStatics( position, destination, &m_StaticIntersectionPoint );

    // No use for the static list anymore, ditch it
    m_IntersectionList.ClearStatics();
}


void Projectile::AdjustCollisionPosition( const rmt::Vector& intersection, 
                                          const rmt::Vector& currPos,
                                          const rmt::Vector& prevPos,
                                          rmt::Vector* newIntersection )
{
    // ToIntersection
    if( currPos != prevPos )
    {
        rmt::Vector toIntersection = currPos - prevPos;
        toIntersection.Normalize();
        *newIntersection = intersection + toIntersection * 0.75f;
    }
    else
    {
        *newIntersection = intersection;
    }
}

void Projectile::ApplyDamage( DynaPhysDSG* object )
{
    // Determine the type of the object and perform an action depending
    // upon the AI ref
    switch (object->GetAIRef())
    {
        case PhysicsAIRef::redBrickVehicle:
        {
            Vehicle* v = (Vehicle*)object;
            if ( v->mVehicleType == VT_TRAFFIC )
            {
                v->TriggerDamage( VEHICLE_DAMAGE_ON_PROJECTILE_HIT );
            }
            break;
        }
        case PhysicsAIRef::PlayerCharacter:
        {
            // Make the character get shocked
            rAssert( dynamic_cast< Character* >( static_cast< tRefCounted* >( object ) ) );
            Character* character = static_cast< Character* >( object );
            rmt::Vector characterPosition;
            character->GetPosition( &characterPosition );
            GetCoinManager()->LoseCoins( COINS_LOST_BY_PLAYER_ON_HIT , &characterPosition );
            character->Shock( 0.5f );
            break;
        }
        case PhysicsAIRef::NPCharacter:
            {
                // Make the character get shocked
                rAssert( dynamic_cast< Character* >( static_cast< tRefCounted* >( object ) ) );
                Character* character = static_cast< Character* >( object );
     
                // Lets try a little velocity change to get the peds moving a bit after they get 
                // shot
                sim::SimState* pSimState = character->GetSimState();
                // Modifying the velocity will be done using a non-const reference to
                // the velocity vector
                rmt::Vector& rVelocity = pSimState->GetLinearVelocity();
                // Apply delta velocity
                // Lets play with the direction the NPC will fly to

                rmt::Vector direction( 0, 0.707f, 0.707f );
                rmt::Matrix transform;
                GetTransform( &transform );
                transform.RotateVector( direction, &direction );

                float deltaV = 10.0f;
                rVelocity += (direction * deltaV);
                // Make it interact with the world
                character->AddToSimulation();

                break;
            }
        case PhysicsAIRef::StateProp:
            {
                // Earl wants these things to ignore vending machines and crates
                const char* ignore_list[] = 
                {
                    "l1_crate", "l2_crate", "l3_crate", "l4_crate", "l5_crate", "l6_crate", "l7_crate",
                    "l1_vending", "l2_vending", "l3_vending", "l4_vending", "l5_vending", "l6_vending", "l7_vending"
                };
                const int num_items_to_ignore = sizeof( ignore_list ) / sizeof( ignore_list[0] );

                StatePropDSG* spdsg = static_cast< StatePropDSG* >( object );
                tUID uid = spdsg->GetStatePropUID();


                bool isSpecialObject = false;
                for ( int i = 0 ; i < num_items_to_ignore ; i++ )
                {
                    if( tName::MakeUID( ignore_list[i] ) == uid )
                    {
                        // A special object, do nothing
                        isSpecialObject = true;
                        break;   
                    }
                }
                if ( isSpecialObject == false )
                {  
                    rmt::Vector direction( 0, 0.707f, 0.707f );
                    rmt::Matrix transform;
                    GetTransform( &transform );
                    transform.RotateVector( direction, &direction );
                    object->ApplyForce( direction, 400.0f );
                }
               break;
            }
            break;
            default:
            {
                rmt::Vector direction( 0, 0.707f, 0.707f );
                rmt::Matrix transform;
                GetTransform( &transform );
                transform.RotateVector( direction, &direction );
                object->ApplyForce( direction, 400.0f );
            }
            break;
    }
}

bool Projectile::IsJumpingPlayer( DynaPhysDSG* object )
{
    bool jumpingCharacter = false;
    if ( object->GetAIRef() == PhysicsAIRef::PlayerCharacter )
    {
        Character* character = static_cast< Character* >( object );
        if ( character->IsJumping() )
            jumpingCharacter = true;
    }
    return jumpingCharacter;
}


