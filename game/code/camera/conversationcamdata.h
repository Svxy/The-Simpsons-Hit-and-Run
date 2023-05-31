//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ConversationCamdata.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ConversationCamDATA_H
#define ConversationCamDATA_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Data for the Camera
//
//=============================================================================

class ConversationCamData
{
public:
    ConversationCamData();
    virtual ~ConversationCamData() {};

    void GetFrontPosition( rmt::Vector* front );
    void SetFrontPosition( rmt::Vector front );
    void GetFrontTarget( rmt::Vector* front );
    void SetFrontTarget( rmt::Vector front );

   

    float GetFOV() const;
    void SetFOV( float fov );

    rmt::Vector mFrontPos;
    rmt::Vector mFrontTarg;
   
    float mFOV;

private:
    //Prevent wasteful constructor creation.
    ConversationCamData( const ConversationCamData& ConversationCamdata );
    ConversationCamData& operator=( const ConversationCamData& ConversationCamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ConversationCamData::ConversationCamData
//=============================================================================
// Description: Constructor
//
// Parameters:  ()
//
// Return:      ConversationCamData
//
//=============================================================================
inline ConversationCamData::ConversationCamData() :
    mFOV( 0.75f )
{
    mFrontPos.Set( 0.0f, 1.5f, -2.8f );    
    mFrontTarg.Set( 0.0f, 1.5f, 2.3f );    
   
}

//=============================================================================
// ConversationCamData::GetFrontPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* front )
//
// Return:      void 
//
//=============================================================================
inline void ConversationCamData::GetFrontPosition( rmt::Vector* front )
{
    *front = mFrontPos;
}

//=============================================================================
// ConversationCamData::SetFrontPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector front )
//
// Return:      void 
//
//=============================================================================
inline void ConversationCamData::SetFrontPosition( rmt::Vector front )
{
    mFrontPos = front;
}

//=============================================================================
// ConversationCamData::GetFrontTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* front )
//
// Return:      void 
//
//=============================================================================
inline void ConversationCamData::GetFrontTarget( rmt::Vector* front )
{
    *front = mFrontTarg;
}

//=============================================================================
// ConversationCamData::SetFrontTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector front )
//
// Return:      void 
//
//=============================================================================
inline void ConversationCamData::SetFrontTarget( rmt::Vector front )
{
    mFrontTarg = front;
}


//=============================================================================
// ConversationCamData::GetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ConversationCamData::GetFOV() const
{
    return mFOV;
}

//=============================================================================
// ConversationCamData::SetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fov )
//
// Return:      void 
//
//=============================================================================
inline void ConversationCamData::SetFOV( float fov )
{
    mFOV = fov;
}

#endif //ConversationCamDATA_H
