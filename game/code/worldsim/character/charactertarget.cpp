//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:
//
// Description: Implementation of class CharacterTarget
//
// History:     5/13/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <worldsim/character/charactertarget.h>
#include <worldsim/character/character.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

/*
==============================================================================
CharacterTarget::CharacterTarget
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         CharacterTarget

=============================================================================
*/
CharacterTarget::CharacterTarget( Character* pCharacter ) :
    mWalkerID( CharacterEnum::INVALID )
{
    mpCharacter = pCharacter;
}
/*
==============================================================================
CharacterTarget::~CharacterTarget
==============================================================================
Description:    Comment

Parameters:     ()

Return:         CharacterTarget

=============================================================================
*/
CharacterTarget::~CharacterTarget()
{
}

/*
==============================================================================
CharacterTarget::GetPosition
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* position )

Return:         void

=============================================================================
*/
void CharacterTarget::GetPosition( rmt::Vector* position )
{
    mpCharacter->GetPosition( *position );
}
/*
==============================================================================
CharacterTarget::GetHeading
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* heading )

Return:         void

=============================================================================
*/
void CharacterTarget::GetHeading( rmt::Vector* heading )
{
    mpCharacter->GetFacing( *heading );
}
/*
==============================================================================
CharacterTarget::GetVUP
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* vup )

Return:         void

=============================================================================
*/
void CharacterTarget::GetVUP( rmt::Vector* vup )
{
    vup->Set( 0.0f, 1.0f, 0.0f );
}
/*
==============================================================================
CharacterTarget::GetVelocity
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector* velocity )

Return:         void

=============================================================================
*/
void CharacterTarget::GetVelocity( rmt::Vector* velocity )
{
    mpCharacter->GetVelocity( *velocity );
}

//=============================================================================
// CharacterTarget::GetID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
unsigned int CharacterTarget::GetID()
{
    return mWalkerID;
}

/*
==============================================================================
CharacterTarget::IsCar
==============================================================================
Description:    Comment

Parameters:     ()

Return:         bool

=============================================================================
*/
bool CharacterTarget::IsCar() const
{
    return false;
}
/*
==============================================================================
CharacterTarget::IsAirborn
==============================================================================
Description:    Comment

Parameters:     ()

Return:         bool

=============================================================================
*/
bool CharacterTarget::IsAirborn()
{
    return !mpCharacter->IsStanding();
}
/*
==============================================================================
CharacterTarget::IsUnstable
==============================================================================
Description:    Comment

Parameters:     ()

Return:         bool

=============================================================================
*/
bool CharacterTarget::IsUnstable()
{
    return false;
    /*
    return( !mpCharacter->IsNPC() &&
        mpCharacter->GetSimState()->GetControl() == sim::simSimulationCtrl );
    */
}
/*
==============================================================================
 CharacterTarget::IsQuickTurn
 ==============================================================================
 Description:   Comment

Parameters:     ()

Return:         bool

=============================================================================
*/
bool CharacterTarget::IsQuickTurn()
{
    return false;
}

//=============================================================================
// CharacterTarget::GetFirstPersonPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position )
//
// Return:      void 
//
//=============================================================================
void CharacterTarget::GetFirstPersonPosition( rmt::Vector* position )
{
    poser::Pose* pose = mpCharacter->GetPuppet()->GetPose();

    rmt::Vector pos;
    pos = pose->GetJoint(17)->GetWorldMatrix().Row(3); // 17 is the head (should be looking it up by name, but what the hell)
    pos -= pose->GetJoint(0)->GetWorldMatrix().Row(3); // 0 is the motion root
    pos.y += 0.05f; // bump it up a little, the head joint actually a little too low

    //Find the eye spot on the character and return that position.
    position->Set( 0.0f, pos.y, 0.0f );
}


/*
==============================================================================
CharacterTarget::GetName
==============================================================================
Description:    This is only for debugging, so in the implementation go ahead and
                make this return NULL in release.

Parameters:     ()

Return:         const

=============================================================================
*/
const char* const CharacterTarget::GetName()
{
    return 0;
}

//=============================================================================
// CharacterTarget::SetID
//=============================================================================
// Description: Comment
//
// Parameters:  ( CharacterEnum::WalkerID id )
//
// Return:      void 
//
//=============================================================================
void CharacterTarget::SetID( CharacterEnum::WalkerID id )
{
    mWalkerID = id;
}

//=============================================================================
// CharacterTarget::IsInReverse
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool CharacterTarget::IsInReverse()
{
    return false;
}

//=============================================================================
// CharacterTarget::GetTerrainIntersect
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos, rmt::Vector& normal )
//
// Return:      void 
//
//=============================================================================
void CharacterTarget::GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const
{
    mpCharacter->GetTerrainIntersect( pos, normal );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
