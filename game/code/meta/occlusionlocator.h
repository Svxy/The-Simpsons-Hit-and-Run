//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        occlusionlocator.h
//
// Description: Blahblahblah
//
// History:     02/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef OCCLUSIONLOCATOR_H
#define OCCLUSIONLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/locatortypes.h>
#include <meta/triggerlocator.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class OcclusionLocator : public TriggerLocator
{
public:
    OcclusionLocator();
    virtual ~OcclusionLocator();

    virtual LocatorType::Type GetDataType() const;

    unsigned int GetNumOccTriggers() const;

protected:
    friend class LocatorLoader;
    void SetNumOccTriggers( unsigned int num );

private:
    virtual void OnTrigger( unsigned int playerID );

    unsigned int mNumOccTriggers;

    //Prevent wasteful constructor creation.
    OcclusionLocator( const OcclusionLocator& occlusionlocator );
    OcclusionLocator& operator=( const OcclusionLocator& occlusionlocator );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// OcclusionLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type OcclusionLocator::GetDataType() const
{
    return( LocatorType::OCCLUSION );
}


//=============================================================================
// OcclusionLocator::GetNumOccTriggers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int OcclusionLocator::GetNumOccTriggers() const
{
    return mNumOccTriggers;
}

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// OcclusionLocator::SetNumOccTriggers
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num )
//
// Return:      void 
//
//=============================================================================
inline void OcclusionLocator::SetNumOccTriggers( unsigned int num )
{
    mNumOccTriggers = num;
}

#endif //OCCLUSIONLOCATOR_H
