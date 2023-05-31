//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/evasionbehaviour.h>
#include <ai/actor/actor.h>
#include <worldsim/avatarmanager.h>
#include <render/IntersectManager/IntersectManager.h>
#include <ai/actor/actordsg.h>
#include <main/game.h>
#include <radtime.hpp>
//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

static const float MAX_EVADE_RANGE_SQR = 4.0f * 4.0f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

rmt::Randomizer EvasionBehaviour::s_Randomizer(0);
bool EvasionBehaviour::s_RandomizerSeeded = false;

       
EvasionBehaviour::EvasionBehaviour( float minEvadeDistHoriz, float maxEvadeDistHoriz, float minEvadeDistVert, float maxEvadeDistVert, float speedInKPH ) :
m_MinEvadeDistHoriz( minEvadeDistHoriz ),
m_MaxEvadeDistHoriz( maxEvadeDistHoriz ),
m_MinEvadeDistVert( minEvadeDistVert ),
m_MaxEvadeDistVert( maxEvadeDistVert ),
m_DeltaTPerMilliSecond( 0 ),
m_T( 0 )
{
    SetSpeed( speedInKPH );

    if (!s_RandomizerSeeded)
    {
        s_Randomizer.Seed (Game::GetRandomSeed ());
        s_RandomizerSeeded = true;
    }
}


EvasionBehaviour::~EvasionBehaviour()
{

}

void
EvasionBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{

    Avatar* currAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rmt::Vector currAvatarPos;     
    currAvatar->GetPosition( currAvatarPos );

    currAvatarPos.y += 0.5f;
    actor->LookAt( currAvatarPos, timeInMS );

    // Test that we are in range of the object
    if ( actor->IsMoving() )
    {
    }
    else
    {
        // Get distance to player
        Avatar* currAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        rmt::Vector currAvatarPos;     
        currAvatar->GetPosition( currAvatarPos );

        rmt::Vector actorPosition;
        actor->GetPosition( &actorPosition );

        float distanceToPlayerSqr = (actorPosition - currAvatarPos).MagnitudeSqr();
        if ( distanceToPlayerSqr < MAX_EVADE_RANGE_SQR )
        {                    
            rmt::Vector dest;
            if ( FindEvasionDestination( actorPosition, &dest ) )
            {
                actor->MoveTo( dest, m_EvadeSpeed );
                // If the wasp is moving, don't allow the actor to do anything else
                SetExclusive( actor->IsMoving() );
            }
            else
            {
                SetExclusive( false );
            }
        }
        else
        {
            // No reason to evade
            SetExclusive( false );
        }
    }

}

// Enable this behaviour
void EvasionBehaviour::Activate()
{
    SetExclusive( false );
}
// Disable this behaviour
void EvasionBehaviour::Deactivate()
{
    SetExclusive( false );
}

void EvasionBehaviour::SetSpeed( float kph )
{
    // Convert from kph to meters per millisecond
    // Precision loss here?
    m_EvadeSpeed = kph * 1000.0f / 3600000.0f;    
}


bool EvasionBehaviour::FindEvasionDestination( const rmt::Vector& actorPosition, rmt::Vector* dest )
{   
    // Pick 2 random numbers in between the designer's given input ranges for horizontal and vertical distance
    float distHoriz;
    float distVert;
    distHoriz = ( m_MaxEvadeDistHoriz - m_MinEvadeDistHoriz ) * s_Randomizer.Float() + m_MinEvadeDistHoriz;

    // Direction vector
    rmt::Vector distOffset( 0, 0, distHoriz );
    rmt::Matrix rotation;
    // Calc a random angle between 0 and 2PI
    float angle = s_Randomizer.Float() * 2.0f * 3.1415927f;
    rotation.Identity();
    rotation.FillRotateY( angle );
    rmt::Vector evasionDest;
    rotation.Transform( distOffset, &evasionDest );
    // Grab actorposition and apply it to get world position
    evasionDest += actorPosition;
    float groundHeight;
    if ( FindGroundHeight( evasionDest.x, evasionDest.z, &groundHeight ) == false )
    {
        rDebugString( "Evasion - ground plane not found!" );
        groundHeight = 0;
    }

    distVert = ( m_MaxEvadeDistVert - m_MinEvadeDistVert ) * s_Randomizer.Float() + m_MinEvadeDistVert;
    evasionDest.y = groundHeight + distVert;
    
    *dest = evasionDest;

    return true;
}
bool 
EvasionBehaviour::FindGroundHeight( float x, float z, float* out_height )const
{

  	const float INTERSECT_TEST_RADIUS = 1.0f;  
	rmt::Vector deepestIntersectPos, deepestIntersectNormal;
    rmt::Vector position( x, 100.0f, z);
    rmt::Vector groundNormal, groundPlaneIntersectionPoint;
    bool foundPlane;
    
    GetIntersectManager()->FindIntersection( position,
                                             foundPlane,
                                             groundNormal,
                                             groundPlaneIntersectionPoint );
        
    if ( foundPlane )
    {
        *out_height = groundPlaneIntersectionPoint.y;
    }
    return foundPlane;
}
