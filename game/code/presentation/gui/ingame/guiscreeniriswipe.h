//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenIrisWipe
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/21      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENIRISWIPE_H
#define GUISCREENIRISWIPE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
namespace Scrooby
{
    class Pure3dObject;
};
class tMultiController;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenIrisWipe : public CGuiScreen
{
public:
    CGuiScreenIrisWipe( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenIrisWipe();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    inline void SetRelativeSpeed( float speed ) { m_relativeSpeed = speed; };
    static void DoNotOpenOnNextOutro();
    static bool IsIrisClosed();

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    Scrooby::Pure3dObject* m_pIris;
    tMultiController*      m_pMultiController;
    float                  m_numFrames;
    float                  m_relativeSpeed;
    static bool            g_IsIrisClosed;

    bool m_isIrisActive : 1;

    Scrooby::Text* m_loadingText;
    unsigned int m_elapsedIdleTime;
    unsigned int m_elapsedBlinkTime;

};

#endif // GUISCREENIRISWIPE_H
