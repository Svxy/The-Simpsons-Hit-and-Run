//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wbcamtarget.h
//
// Description: Blahblahblah
//
// History:     19/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WBCAMTARGET_H
#define WBCAMTARGET_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

#include  "..\..\..\game\code\camera\isupercamtarget.h"

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WBCamTarget : public ISuperCamTarget
{
public:
    WBCamTarget();
    WBCamTarget( MObject& target );
    virtual ~WBCamTarget();

    virtual void GetPosition( rmt::Vector* position );
    virtual void GetHeading( rmt::Vector* heading );
    virtual void GetVUP( rmt::Vector* vup );
    virtual void GetVelocity( rmt::Vector* velocity );
    virtual unsigned int GetID();
    virtual bool IsCar();
    virtual bool IsAirborn();
    virtual bool IsUnstable(); 
    virtual bool IsQuickTurn();
    virtual bool IsInReverse() { return false; };
    virtual void GetFirstPersonPosition( rmt::Vector* position );

    virtual const char* const GetName();

    void SetTarget( MObject& target );
    const MObject& GetTarget() const;

private:
    MObject& mTarget;
};

//=============================================================================
// WBCamTarget::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& target )
//
// Return:      void 
//
//=============================================================================
inline void WBCamTarget::SetTarget( MObject& target )
{
    mTarget = target;
}

//=============================================================================
// WBCamTarget::GetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MObject
//
//=============================================================================
inline const MObject& WBCamTarget::GetTarget() const
{
    return mTarget;
}

#endif //WBCAMTARGET_H
