//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actioneventlocator.h
//
// Description: Blahblahblah
//
// History:     30/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ACTIONEVENTLOCATOR_H
#define ACTIONEVENTLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/locatortypes.h>
#include <meta/locatorevents.h>
#include <meta/triggerlocator.h>
#include <worldsim/character/charactercontroller.h>

//========================================
// Forward References
//========================================
class tEntityStore;
//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ActionEventLocator : public TriggerLocator
{
public:
    ActionEventLocator();
    virtual ~ActionEventLocator();

    //From Locator
    virtual LocatorType::Type GetDataType() const;

    void SetObjNameSize( unsigned char size );
    void SetObjName( const char* name );
    const char* GetObjName() const;

    void SetJointNameSize( unsigned char size );
    void SetJointName( const char* name );
    const char* GetJointName() const;

    void SetActionNameSize( unsigned char size );
    void SetActionName( const char* name );
    const char* GetActionName() const;

    void SetButtonInput( CharacterController::eIntention button );
    CharacterController::eIntention GetButtonInput() const;

    void SetShouldTransform( bool should );
    bool GetShouldTransform() const;
    
    bool AddToGame( tEntityStore* store );
    void Reset( void );

    void SetMatrix( const rmt::Matrix& mat );
    rmt::Matrix& GetMatrix();

private:
    unsigned char mObjNameSize;
    unsigned char mJointNameSize;
    unsigned char mActionNameSize;

    bool mShouldTransform;

    char* mObjName;
    char* mJointName;
    char* mActionName;

    CharacterController::eIntention mButton;
    rmt::Matrix mMatrix;
    
    
    
    //From TriggerLocator
    virtual void OnTrigger( unsigned int playerID );

    //Prevent wasteful constructor creation.
    ActionEventLocator( const ActionEventLocator& actioneventlocator );
    ActionEventLocator& operator=( const ActionEventLocator& actioneventlocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ActionEventLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorType
//
//=============================================================================
inline LocatorType::Type ActionEventLocator::GetDataType() const
{
    return LocatorType::ACTION;
}

//=============================================================================
// ActionEventLocator::GetObjName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* ActionEventLocator::GetObjName() const
{
    return mObjName;
}


//=============================================================================
// ActionEventLocator::GetJointName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* ActionEventLocator::GetJointName() const
{
    return mJointName;
}

//=============================================================================
// ActionEventLocator::GetActionName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* ActionEventLocator::GetActionName() const
{
    return mActionName;
}

//=============================================================================
// ActionEventLocator::SetButtonInput
//=============================================================================
// Description: Comment
//
// Parameters:  ( CharacterController::eIntention button )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocator::SetButtonInput( CharacterController::eIntention button )
{
    mButton = button;
}

//=============================================================================
// ActionEventLocator::GetButtonInput
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      CharacterController
//
//=============================================================================
inline CharacterController::eIntention ActionEventLocator::GetButtonInput() const
{
    return mButton;
}

//=============================================================================
// ActionEventLocator::SetShouldTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool should )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocator::SetShouldTransform( bool should )
{
    mShouldTransform = should;
}

//=============================================================================
// ActionEventLocator::GetShouldTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool ActionEventLocator::GetShouldTransform() const
{
    return mShouldTransform;
}

//=============================================================================
// ActionEventLocator::SetMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Matrix& mat )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocator::SetMatrix( const rmt::Matrix& mat )
{
    mMatrix = mat;
}

//=============================================================================
// ActionEventLocator::GetMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
inline rmt::Matrix& ActionEventLocator::GetMatrix()
{
    return mMatrix;
}


#endif //ACTIONEVENTLOCATOR_H
