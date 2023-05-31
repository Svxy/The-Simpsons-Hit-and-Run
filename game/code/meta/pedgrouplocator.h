//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedgrouplocatorPedGroupLocator.h
//
// Description: Blahblahblah
//
// History:     12/4/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef PEDGROUPLOCATOR_H
#define PEDGROUPLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/eventlocator.h>

#include <radmath/radmath.hpp>

#include <meta/triggerlocator.h>
#include <meta/locatortypes.h>
#include <meta/locatorevents.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class PedGroupLocator : public EventLocator
{
public:
    PedGroupLocator() : mGroupNum( 0 ) { SetEventType(LocatorEvent::LOAD_PED_MODEL_GROUP); };
    virtual ~PedGroupLocator() {};

    void SetGroupNum( unsigned int num );
    unsigned int GetGroupNum() const;

    virtual LocatorType::Type GetDataType() const { return LocatorType::PED_GROUP; };

private:
    unsigned int mGroupNum;

    //Prevent wasteful constructor creation.
	PedGroupLocator( const PedGroupLocator& pedgrouplocator );
	PedGroupLocator& operator=( const PedGroupLocator& pedgrouplocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// PedGroupLocator::SetGroupNum
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num )
//
// Return:      void 
//
//=============================================================================
inline void PedGroupLocator::SetGroupNum( unsigned int num )
{
    mGroupNum = num;
}

//=============================================================================
// PedGroupLocator::GetGroupNum
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int PedGroupLocator::GetGroupNum() const
{
    return mGroupNum;
}

#endif //PEDGROUPLOCATOR_H
