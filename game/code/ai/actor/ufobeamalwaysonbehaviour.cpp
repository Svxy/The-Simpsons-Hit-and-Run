//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ufobeamalwayson
//
// Description: code for the ai (if you can call it that for l7m3)
//              UFO stays in one spot and its beam is always on
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai\actor\ufobeamalwaysonbehaviour.h>
#include <ai\actor\actor.h>
#include <ai\actor\flyingactor.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float UFO_ROT_SPEED = 0.001f;


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================



//===========================================================================
// Member Functions
//===========================================================================


UFOBeamAlwaysOn::UFOBeamAlwaysOn()
{

}

UFOBeamAlwaysOn::~UFOBeamAlwaysOn()
{

}

void
UFOBeamAlwaysOn::Apply( Actor* actor, unsigned int timeInMS )
{
    if ( actor->IsTractorBeamOn() == false )
    {
        actor->ActivateTractorBeam();
    }
    FlyingActor* flyingActor = static_cast< FlyingActor* >( actor );
    flyingActor->SetDesiredHeightEnabled( true );
    

    m_Rotation += static_cast< float >( timeInMS ) * UFO_ROT_SPEED;
    rmt::Matrix transform;
    actor->GetTransform( &transform );
    transform.FillRotateY( m_Rotation );
    actor->SetTransform( transform );

    const float UFO_HEIGHT_ABOVE_GROUND = 20.0f;
    flyingActor->SetDesiredHeight( UFO_HEIGHT_ABOVE_GROUND );

    if ( actor->IsTractorBeamOn() )
    {

        // Tractor beam is on,
        // anything under the beam is toast.
        // fetch a list of all the targets under the beam
        SwapArray< DynaPhysDSG* > dynaPhysList( 20 );
        rmt::Vector actorPosition;
        actor->GetPosition( &actorPosition );
        GetTargetsInTractorRange( actorPosition, UFO_HEIGHT_ABOVE_GROUND * 1.5f, &dynaPhysList );
        // Iterate through the list and destroy them
        int objectsLeft = 0;
        for ( int i = 0 ; i < dynaPhysList.mUseSize ; i++ )
        {
            if (SuckIntoUFO( actorPosition, dynaPhysList[i], static_cast< float >( timeInMS ) ) )
            {
                objectsLeft++;
            }
            if ( ReachedTopOfTractorBeam( actorPosition, dynaPhysList[i] ) )
            {
                // Destroy the object, and test to see if it damaged the UFO as well
                if ( DestroyObject( dynaPhysList[i] ) )
                    actor->SetState( 1 );

                objectsLeft--;
            }
        }  
    }
}