//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPurchaseRewards
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

#ifndef GUISCREENPURCHASEREWARDS_H
#define GUISCREENPURCHASEREWARDS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenrewards.h>
#include <mission/rewards/merchandise.h>
#include <p3d/anim/multicontroller.hpp>

//===========================================================================
// Forward References
//===========================================================================

class tMultiController;
class tPoseAnimationController;
class tPose;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPurchaseRewards : public IGuiScreenRewards
{
public:
    CGuiScreenPurchaseRewards( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPurchaseRewards();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    void OnProcessRequestsComplete( void* pUserData );

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
    void OnUpdate( unsigned int elapsedTime );
    void UpdateRewardPrice();
    void UpdateBankValue();
    void PurchaseReward();
    void LoadSelectedReward();

    Merchandise::eSellerType m_currentType;
    bool m_isPurchasingReward;
    unsigned int m_elapsedCoinDecrementTotalTime;
    unsigned int m_elapsedCoinDecrementTime;
    int m_bankValueBeforePurchase;
    Scrooby::Text* m_purchaseLabel;

#ifdef RAD_WIN32
    Scrooby::Sprite* m_leftArrow;
    Scrooby::Sprite* m_rightArrow;
#endif

    tMultiController::TrackInfo m_skinTrackInfo;
    tMultiController* m_skinMultiController;
    tPoseAnimationController* m_skinAnimController;

};

#endif // GUISCREENPURCHASEREWARDS_H
