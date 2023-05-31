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

#ifndef HUDMISSIONPROGRESS_H
#define HUDMISSIONPROGRESS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Page;
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudMissionProgress : public HudEventHandler
{
public:
    HudMissionProgress( Scrooby::Page* pPage );
    virtual ~HudMissionProgress();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

private:
    Scrooby::Text* m_stageComplete;

};

#endif // HUDMISSIONPROGRESS_H
