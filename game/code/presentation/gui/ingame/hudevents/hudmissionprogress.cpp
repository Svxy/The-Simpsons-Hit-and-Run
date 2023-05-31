//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudMissionProgress
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudmissionprogress.h>
#include <presentation/gui/utility/specialfx.h>

// Scrooby
#include <group.h>
#include <page.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudMissionProgress::HudMissionProgress( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "MissionProgress" ) ),
    m_stageComplete( NULL )
{
    rAssert( pPage != NULL );

    Scrooby::Group* missionProgress = pPage->GetGroup( "MissionProgress" );
    rAssert( missionProgress != NULL );

    m_stageComplete = missionProgress->GetText( "StageComplete" );
    rAssert( m_stageComplete != NULL );
    m_stageComplete->SetTextMode( Scrooby::TEXT_WRAP );
}

HudMissionProgress::~HudMissionProgress()
{
}

void
HudMissionProgress::Start()
{
    this->OnStart();
}

void
HudMissionProgress::Stop()
{
    this->OnStop();
}

void
HudMissionProgress::Update( float elapsedTime )
{
    if( m_currentState == STATE_RUNNING )
    {
        m_elapsedTime += elapsedTime;

        static float DURATION_TIME = 1000.0f;

        bool isDone = GuiSFX::Flash( m_stageComplete,
                                     m_elapsedTime,
                                     DURATION_TIME,
                                     1,
                                     1.25f,
                                     1.0f );
        if( isDone )
        {
            this->Stop();
        }
    }
}

