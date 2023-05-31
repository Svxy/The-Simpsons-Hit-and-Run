//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoadingFE
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/23      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENLOADINGFE_H
#define GUISCREENLOADINGFE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/utility/slider.h>

#include <contexts/contextenum.h>
#include <loading/loadingmanager.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLoadingFE : public CGuiScreen,
                            public LoadingManager::ProcessRequestsCallback
{
public:
    CGuiScreenLoadingFE( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenLoadingFE();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    void LoadResources();
    virtual void OnProcessRequestsComplete( void* pUserData );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    float GetCurrentMemoryUsage( ContextEnum currentContext ) const;

    Scrooby::Text* m_loadingText;
    Scrooby::Pure3dObject* m_itchyAndScratchy;

    unsigned int m_elapsedTime;

    Scrooby::Group* m_loadingBarGroup;
    Slider m_loadingBar;
    float m_currentMemoryUsage;     // in percent
    int m_startingMemoryAvailable;  // in bytes
    unsigned int m_elapsedFireTime;

    Scrooby::Layer* m_explosionLayer;
    Scrooby::Polygon* m_explosion;
    unsigned int m_elapsedExplosionTime;

};

#endif // GUISCREENLOADINGFE_H
