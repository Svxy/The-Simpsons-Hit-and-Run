//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoading
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

#ifndef GUISCREENLOADING_H
#define GUISCREENLOADING_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/utility/slider.h>

#include <loading/loadingmanager.h>

//===========================================================================
// Forward References
//===========================================================================

class tSprite;
namespace Scrooby
{
    class Group;
}

const int NUM_LOADING_OVERLAYS = 2;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLoading : public CGuiScreen,
                          public LoadingManager::ProcessRequestsCallback
{
public:
    CGuiScreenLoading( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenLoading();

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
    unsigned int m_elapsedTime;
    unsigned int m_elapsedSpiralTime;

    Scrooby::Group* m_loadingBarGroup;
    Slider m_loadingBar;
    float m_currentMemoryUsage;     // in percent
    int m_startingMemoryAvailable;  // in bytes
    unsigned int m_elapsedFireTime;

    Scrooby::Sprite* m_loadingImage;
    Scrooby::Sprite* m_loadingOverlays[ NUM_LOADING_OVERLAYS ];
    bool m_isSpirallingDone : 1;

    tSprite* m_loadingImageSprite;
    char m_loadingImageName[ 32 ];

    Scrooby::Layer* m_explosionLayer;
    Scrooby::Polygon* m_explosion;
    unsigned int m_elapsedExplosionTime;

};

#endif // GUISCREENLOADING_H
