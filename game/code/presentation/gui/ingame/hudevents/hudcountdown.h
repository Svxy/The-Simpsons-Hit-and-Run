//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCountDown
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDCOUNTDOWN_H
#define HUDCOUNTDOWN_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>
#include <mission/missionstage.h>
#include <events/eventdata.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Page;
    class Sprite;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudCountDown : public HudEventHandler
{
private:
    typedef HudEventHandler Parent;

public:
    HudCountDown( Scrooby::Page* pPage );
    virtual ~HudCountDown();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

protected:
    void OnStart();
    void QueueDisableInput();

private:
    void GetNextSequenceUnit();

    Scrooby::Sprite* m_countDownMessage;

    MissionStage* m_missionStage;
    MissionStage::CountdownSequenceUnit* m_currentSequenceUnit;
    int m_nextSequenceIndex;
    bool m_InputDisablePending;

    bool m_isDialogTriggerPending;
    DialogEventData m_dialogData;

};

#endif // HUDCOUNTDOWN_H
