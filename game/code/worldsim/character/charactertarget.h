// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        charactertarget.h
//
// Description: Blahblahblah
//
// History:     5/13/2002 + Created -- NAME
//
//=============================================================================

#ifndef CHARACTERTARGET_H
#define CHARACTERTARGET_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
#include <camera/isupercamtarget.h>
#include <constants/characterenum.h>
#include <presentation/gui/utility/hudmap.h>

//========================================
// Forward References
//========================================
class Character;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CharacterTarget
:
public ISuperCamTarget, public IHudMapIconLocator
{
public:
    CharacterTarget( Character* pCharacter );
    ~CharacterTarget();
    
    virtual void GetPosition( rmt::Vector* position );
    virtual void GetHeading( rmt::Vector* heading );
    virtual void GetVUP( rmt::Vector* vup );
    virtual void GetVelocity( rmt::Vector* velocity );
    virtual unsigned int GetID();
    virtual bool IsCar() const;
    virtual bool IsAirborn();
    virtual bool IsUnstable(); 
    virtual bool IsQuickTurn();
    virtual bool IsInReverse();
    virtual void GetFirstPersonPosition( rmt::Vector* position );
    virtual void GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const;



    //This is only for debugging, so in the implementation go ahead and 
    //make this return NULL in release.
    virtual const char* const GetName();

    void SetID( CharacterEnum::WalkerID id );
private:

    //Prevent wasteful constructor creation.
    CharacterTarget( void );
	CharacterTarget( const CharacterTarget& charactertarget );
    CharacterTarget& operator=( const CharacterTarget& charactertarget );

    Character* mpCharacter;
    CharacterEnum::WalkerID mWalkerID;
};

#endif //CHARACTERTARGET_H