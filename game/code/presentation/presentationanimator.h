//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        presentationanimator.h
//
// Description: Blahblahblah
//
// History:     9/24/2002 + Created -- NAME
//
//=============================================================================

#ifndef PRESENTATIONANIMATOR_H
#define PRESENTATIONANIMATOR_H

//========================================
// Nested Includes
//========================================
#include <vector>

#include <memory/stlallocators.h>


//========================================
// Forward References
//========================================

class Character;
class MouthFlapper;

//=============================================================================
//
// Synopsis:    PresentationAnimator
//
//=============================================================================

class PresentationAnimator
{
public:
    PresentationAnimator();
	virtual ~PresentationAnimator();

    void SetCharacter( Character* pCharacter );
    Character* GetCharacter();

    typedef std::vector< tName, s2alloc<tName> > TNAMEVECTOR;
    void AddAmbientAnimations( const TNAMEVECTOR& animations );
    void ClearAmbientAnimations( void );
    void PlaySpecialAmbientAnimation();
    void SetRandomSelection( const bool random );
    const bool GetRandomSelection() const;
    void StartTalking();
    void StopTalking();
    void TalkFor(int time);
    void Update( int elapsedTime );

private:
    const tName ChooseNextAnimation();
    const tName ChooseRandomAnimation() const;
	PresentationAnimator( const PresentationAnimator& presentationanimator );
	PresentationAnimator& operator=( const PresentationAnimator& presentationanimator );

    Character* mCharacter;
    MouthFlapper* mMouthFlapper;
    TNAMEVECTOR mAnimationNames;
    bool mRandomSelection;
    int mTalkTime;
};


#endif //PRESENTATIONANIMATOR_H
