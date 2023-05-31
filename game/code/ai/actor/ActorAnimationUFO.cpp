//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains the implementation of...
//
// Authors:     Name Here
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/actoranimationufo.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float DEGREES_PER_SECOND = 20.0f;
const float RADIANS_PER_MILLISECOND = rmt::DegToRadian( DEGREES_PER_SECOND ) / 1000.0f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// ActorAnimationUFO::ActorAnimationUFO
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

ActorAnimationUFO::ActorAnimationUFO():
m_NumUpdates( 0 )
{

}

//===========================================================================
// ActorAnimationUFO::~ActorAnimationUFO
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

ActorAnimationUFO::~ActorAnimationUFO()
{
    
}

//===========================================================================
// ActorAnimationUFO::Update
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

bool ActorAnimationUFO::Update( const rmt::Matrix& currTransform, 
                                rmt::Matrix* newTransform, 
                                float deltaTime, 
                                tCompositeDrawable* compDraw )
{
    float angle = deltaTime * RADIANS_PER_MILLISECOND;

    // Rotate the ufo
    rmt::Matrix rotationMatrix;
    rotationMatrix.Identity();
    rotationMatrix.FillRotateY( angle );
    newTransform->Mult( rotationMatrix, currTransform );

    if ( (m_NumUpdates % 100) == 0 )
    {
        rotationMatrix.OrthoNormal();
    }

    m_NumUpdates++;
    // Matrix always updated
    return true;
}


