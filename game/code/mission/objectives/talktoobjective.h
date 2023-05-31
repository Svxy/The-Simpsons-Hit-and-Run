//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        talktoobjective.h
//
// Description: Blahblahblah
//
// History:     29/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TALKTOOBJECTIVE_H
#define TALKTOOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>

#include <render/enums/renderenums.h>

#include <presentation/gui/utility/hudmap.h>
#include <input/inputmanager.h>
//========================================
// Forward References
//========================================
class Character;
class EventLocator;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TalkToObjective : public MissionObjective, public IHudMapIconLocator
{
public:
    enum { MAX_CHARACTER_NAME = 64 };
    enum IconType { EXCLAMATION, GIFT, DOOR };

    TalkToObjective();
    virtual ~TalkToObjective();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    void SetTalkToTarget( const char* name, IconType type, float yOffset, float rad );

    //Interface for IHudMapIconLocator
    void GetPosition( rmt::Vector* currentLoc );
    void GetHeading( rmt::Vector* heading );


protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

    PathStruct mArrowPath;

private:
    char mCharacterName[MAX_CHARACTER_NAME];
    Character* mTalkToTarget;
    int mHudMapIconID;
    EventLocator* mEvtLoc;

    AnimatedIcon* mAnimatedIcon;
    IconType mIconType;
    float mYOffset;
    float mTriggerRadius;

    RenderEnums::LayerEnum mRenderLayer;
    
    bool mIsDisabled;

    int mActionID;

    //Prevent wasteful constructor creation.
    TalkToObjective( const TalkToObjective& talktoobjective );
    TalkToObjective& operator=( const TalkToObjective& talktoobjective );

	Input::ActiveState m_PrevActiveGameState;
};


#endif //TALKTOOBJECTIVE_H
