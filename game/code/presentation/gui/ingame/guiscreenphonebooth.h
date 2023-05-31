//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPhoneBooth
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

#ifndef GUISCREENPHONEBOOTH_H
#define GUISCREENPHONEBOOTH_H

#ifndef RAD_E3
    // enable car selection overlay for selecting any car
    //
    #define SRR2_OVERRIDE_CAR_SELECTION
#endif

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenrewards.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPhoneBooth : public IGuiScreenRewards
{
public:
    CGuiScreenPhoneBooth( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPhoneBooth();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

#ifdef RAD_WIN32
    virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    virtual void On3DModelLoaded( const PreviewObject* previewObject );
    virtual const PreviewObject* GetCurrentPreviewObject() const;
    virtual void InitMenu();

private:
    virtual void OnUpdate( unsigned int elapsedTime );
    void LoadSelectedReward();
    void UpdateDamagedInfo();

    Scrooby::Group* m_damagedInfo;
    Scrooby::Text* m_vehicleDamaged;
    Scrooby::Group* m_repairCostInfo;
    Scrooby::Text* m_vehicleRepairCost;

#ifdef RAD_WIN32
    Scrooby::Sprite* m_leftArrow;
    Scrooby::Sprite* m_rightArrow;
#endif

#ifdef SRR2_OVERRIDE_CAR_SELECTION
    void HandleMessageForCar( eGuiMessage message, 
                              unsigned int param1 = 0,
                              unsigned int param2 = 0 );

    CGuiMenu* m_pMenu;
    Scrooby::Layer* m_carSelectOverlay;
    bool m_menuTeleport;

    static int s_currentDebugVehicleSelection;
    static unsigned s_currentTeleportSelection;
#endif

};

#endif // GUISCREENPHONEBOOTH_H
