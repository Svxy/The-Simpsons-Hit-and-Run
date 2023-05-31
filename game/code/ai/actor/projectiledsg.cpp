//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ProjectileDSG
//
// Description: ProjectileDSG is a statepropdsg with one minor difference
//              upon contact with an object in prereact, it destroys the other
//              object
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/projectiledsg.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================




//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

ProjectileDSG::ProjectileDSG():
mHasHit( false )
{

}

ProjectileDSG::~ProjectileDSG()
{

}



sim::Solving_Answer 
ProjectileDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{

    return sim::Solving_Aborted;
}



sim::Solving_Answer
ProjectileDSG::PostReactToCollision( rmt::Vector& impulse, sim::Collision& inCollision )
{
    // No point in computing expensive sim calculations, abort immediately
    return sim::Solving_Aborted;
}