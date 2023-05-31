//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        blinker.h
//
// Description: Blahblahblah
//
// History:     9/24/2002 + Created -- NAME
//
//=============================================================================

#ifndef BLINKER_H
#define BLINKER_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

class Character;
class tMultiController;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class Blinker
{
public:
    Blinker();
	virtual ~Blinker();

    void SetCharacter( Character* pCharacter );

    void Update( int elapsedTime );

    void StartBlinking();
    void StopBlinking();

private:

    //Prevent wasteful constructor creation.
	Blinker( const Blinker& blinker );
	Blinker& operator=( const Blinker& blinker );

    Character*        mCharacter;
    tMultiController* mController;

    int mTimeSinceBlink;
    int mTimeTarget;

    enum BlinkState
    {
        STATE_INVALID,
        STATE_WAITING,
        STATE_BLINKING
    };

    BlinkState mState;
};


#endif //BLINKER_H
