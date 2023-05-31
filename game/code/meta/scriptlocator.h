//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        scriptlocator.h
//
// Description: Blahblahblah
//
// History:     04/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SCRIPTLOCATOR_H
#define SCRIPTLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/triggerlocator.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ScriptLocator : public TriggerLocator
{
public:
    ScriptLocator();
    virtual ~ScriptLocator();

    virtual void OnTrigger( unsigned int playerID );
    virtual LocatorType::Type GetDataType() const;

    void SetString( const char* text );
    radKey32 GetKey();

private:

    radKey32 m_key;

    //Prevent wasteful constructor creation.
    ScriptLocator( const ScriptLocator& scriptlocator );
    ScriptLocator& operator=( const ScriptLocator& scriptlocator );
};

//=============================================================================
// ScriptLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type ScriptLocator::GetDataType() const
{
    return( LocatorType::SCRIPT );
}

//=============================================================================
// ScriptLocator::SetString
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* text )
//
// Return:      void 
//
//=============================================================================
inline void ScriptLocator::SetString( const char* text )
{
    m_key = ::radMakeKey32( text );
}

//=============================================================================
// ScriptLocator::GetKey
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      radKey32 
//
//=============================================================================
inline radKey32 ScriptLocator::GetKey()
{
    return m_key;
}

#endif //SCRIPTLOCATOR_H
