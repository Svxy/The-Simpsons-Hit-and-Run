//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogueobjective.h
//
// Description: Blahblahblah
//
// History:     03/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef DIALOGUEOBJECTIVE_H
#define DIALOGUEOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>
#include <events/eventdata.h>

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
class CarStartLocator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DialogueObjective : public MissionObjective
{
public:
    enum { MAX_CHAR_NAME_LEN = 30 };

    DialogueObjective();
    virtual ~DialogueObjective();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    void SetChar1Name( const char* name );
    void SetChar2Name( const char* name );
    void SetDialogueName( const char* name );
    void SetCameraDistance( const float distance );

    void SetPositions( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos, bool dontReset );

    bool CharactersReset();

    void DontReset() { mDontReset = true; };

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    
private:
    char mChar1Name[MAX_CHAR_NAME_LEN];
    char mChar2Name[MAX_CHAR_NAME_LEN];
    radKey32 mDialogueName;
    float mCameraDistance;

    DialogEventData mDialogEventData;

    CarStartLocator* mChar1Pos;
    CarStartLocator* mChar2Pos;
    CarStartLocator* mCarPos;

    rmt::Vector mChar1OldPos;
    rmt::Vector mChar2OldPos;
    rmt::Vector mCarOldPos;
    float mChar1Rotation;
    float mChar2Rotation;
    float mCarRotation;

    bool mReset : 1;
    bool mMoved : 1;
    bool mCharacter1WasInCarToStartWith : 1;
    bool mCharacter2WasInCarToStartWith : 1;
    bool mHidTheCar : 1;
    bool mHidDefault : 1;
    bool mDontReset : 1;  //This is for mission complete.

    void ResetCharacterPositions();

    //Prevent wasteful constructor creation.
    DialogueObjective( const DialogueObjective& dialogueobjective );
    DialogueObjective& operator=( const DialogueObjective& dialogueobjective );
};


#endif //DIALOGUEOBJECTIVE_H
