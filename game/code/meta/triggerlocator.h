//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        triggerlocator.h
//
// Description: Blahblahblah
//
// History:     04/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRIGGERLOCATOR_H
#define TRIGGERLOCATOR_H

//========================================
// Nested Includes
//========================================
#ifndef WORLD_BUILDER
#include <meta/locator.h>
#else
#include "locator.h"
#endif

//========================================
// Forward References
//========================================
class TriggerVolume;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TriggerLocator : public Locator
{
public:
    TriggerLocator();
    virtual ~TriggerLocator();

    void            SetNumTriggers( unsigned int num, int allocID = 0 );
    unsigned int    GetNumTriggers();
    void            AddTriggerVolume( TriggerVolume* volume );
    TriggerVolume*  GetTriggerVolume( unsigned int i );

    void Trigger( unsigned int playerID, bool bActive );

    unsigned int GetPlayerID();
    bool         GetPlayerEntered();
    // Only for use with fake trigger for first dynamic load.
    void         SetPlayerEntered( bool always = true );

    unsigned int IsPlayerTracked( int playerID ) const;

protected:
    virtual void OnTrigger( unsigned int playerID )
    {
        // HEY! Don't call pure virtual functions from either a constructor
        // or destructor unless you know the vtable has a valid entry
        // I'm changing this from pure virtual to having an empty implementation 
        // as OnTrigger was being called from TriggerLocator's dtor (in a roundabout way)
    };

private:
    TriggerVolume** mTriggerVolumes;
    unsigned short mNumTriggers;
    unsigned short mMaxNumTriggers;

    bool mPlayerEntered; // If the last player entered or left
    int  mPlayerID; // The ID of the last player to trigger this locator

    //Prevent wasteful constructor creation.
    TriggerLocator( const TriggerLocator& triggerlocator );
    TriggerLocator& operator=( const TriggerLocator& triggerlocator );
};


//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// TriggerLocator::GetNumTriggers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned int
//
//=============================================================================
inline unsigned int TriggerLocator::GetNumTriggers()
{
    return mNumTriggers;
}

//=============================================================================
// TriggerLocator::Trigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID, bool bActive )
//
// Return:      void 
//
//=============================================================================
inline void TriggerLocator::Trigger( unsigned int playerID, bool bActive )
{
    mPlayerID = playerID;
    mPlayerEntered = bActive;
    OnTrigger( playerID );    
}

//=============================================================================
// TriggerLocator::GetPlayerID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline unsigned int TriggerLocator::GetPlayerID()
{
    return mPlayerID;
}

//=============================================================================
// TriggerLocator::GetPlayerEntered
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool TriggerLocator::GetPlayerEntered()
{
    return mPlayerEntered;
}


//=============================================================================
// TriggerLocator::SetPlayerEntered
//=============================================================================
// Description: Comment
//
// Parameters:  always true
//
// Return:      None. 
//
//=============================================================================
inline void TriggerLocator::SetPlayerEntered( bool always )
{
    mPlayerEntered = always;
}


#endif //TRIGGERLOCATOR_H
