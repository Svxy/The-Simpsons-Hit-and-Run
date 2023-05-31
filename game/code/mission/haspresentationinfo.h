//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        haspresentationinfo.h
//
// Description: accessors/data members for stages with presentation information
//
// History:     06/05/2003 + Created -- Ian Gipson
//
//=============================================================================

#ifndef HASPRESENTATIONINFO_H
#define HASPRESENTATIONINFO_H

//========================================
// Nested Includes
//========================================
#include <memory/stlallocators.h>
#include <vector>

//========================================
// Forward References
//========================================
class tName;


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class HasPresentationInfo
{
public:
    HasPresentationInfo();
    ~HasPresentationInfo();
    void AmbientCharacterAnimationSetRandom( const unsigned int character, const bool random );
    void AddAmbientCharacterAnimation( const unsigned int character, const tName& animationName );
    void CharacterIsChild( const int index );
    void ClearAmbientAnimations();
    void GoToPattyAndSelmaScreenWhenDone();
    void OnStageCompleteSuccessful() const;
    void Reset(); 
    void SetCameraForDialogLine   ( const unsigned int dialogLine, const tName& camera );
    void SetConversationCamName   ( const tName& name );
    void SetConversationCamPcName ( const tName& name );
    void SetConversationCamNpcName( const tName& name );
    void SetBestSideLocator       ( const tName& name );

protected:
private:
    //Prevent wasteful constructor creation.
	HasPresentationInfo( const HasPresentationInfo& bonusmissioninfo );
	HasPresentationInfo& operator=( const HasPresentationInfo& bonusmissioninfo );

    tName mConversationCamName;
    tName mConversationCamNpcName;
    tName mConversationCamPcName;
    typedef std::vector< tName, s2alloc<tName> > TNAMEVECTOR;
    bool mAmbientPcAnimationsRandom;
    bool mAmbientNpcAnimationsRandom;
    TNAMEVECTOR mAmbientPcAnimations;
    TNAMEVECTOR mAmbientNpcAnimations;
    bool mPcIsChild : 1;
    bool mNpcIsChild : 1;
    bool mGoToPattyAndSelmaScreenWhenDone : 1;
    TNAMEVECTOR mCamerasForLinesOfDialog;
    tName mBestSideLocator;
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************
#endif //HASPRESENTATIONINFO_H
