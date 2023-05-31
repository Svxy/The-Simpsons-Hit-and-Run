//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IGuiScreenRewards
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

#ifndef GUISCREENREWARDS_H
#define GUISCREENREWARDS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <loading/loadingmanager.h>
#include <memory/srrmemory.h>

#ifdef RAD_WIN32
#define LOAD_DAMAGED_VEHICLE_IMAGES
#endif

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu;
class Reward;

struct PreviewObject
{
    char name[ 16 ];            // object name
    char filename[ 64 ];        // object filename

    char nameModel[ 16 ];       // model drawable name
    char filenameModel[ 64 ];   // model filename

    bool isUnlocked : 1;        // unlocked flag
    Reward* pReward;            // reference to reward object
};

class PhoneBoothStars
{
public:
    PhoneBoothStars( Scrooby::Page* pPage, const char* namePrefix );
    void SetNumStars( float numStars );

private:
    static const int MAX_NUM_STARS = 5;
    Scrooby::Sprite* m_stars[ MAX_NUM_STARS ];

};

//===========================================================================
// Interface Definitions
//===========================================================================
class IGuiScreenRewards : public CGuiScreen,
                          public LoadingManager::ProcessRequestsCallback
{
public:
    IGuiScreenRewards( Scrooby::Screen* pScreen,
                       Scrooby::Page* pPage,
                       CGuiEntity* pParent,
                       eGuiWindowID windowID );

    virtual ~IGuiScreenRewards();

    virtual void HandleMessage( eGuiMessage message, 
                                unsigned int param1 = 0,
                                unsigned int param2 = 0 );

    void OnProcessRequestsComplete( void* pUserData );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    virtual void On3DModelLoaded( const PreviewObject* previewObject ) = 0;
    virtual const PreviewObject* GetCurrentPreviewObject() const = 0;
    virtual void InitMenu() = 0;

    int InsertPreviewObject( PreviewObject* previewObjects,
                             int numPreviewObjects,
                             Reward* pReward,
                             bool isSorted = false );

    virtual void OnUpdate( unsigned int elapsedTime );

    void SetLightOpening( float amount ); // 0.0f <= amount <= 1.0f

    void On3DModelSelectionChange( const PreviewObject* nextModel );

    void Load3DModel( const PreviewObject* previewObject );
    void Unload3DModel();

    void SetVehicleStatsVisible( bool isVisible );
    void UpdateVehicleStats();

    static const int MAX_NUM_PREVIEW_VEHICLES = 64;
    static const int MAX_NUM_PREVIEW_CLOTHING = 4;

    CGuiMenu* m_pRewardsMenu;

    enum eRewardsScreenState
    {
        REWARDS_SCREEN_OPENING_LIGHT,
        REWARDS_SCREEN_LIGHT_OPENED,
        REWARDS_SCREEN_CLOSING_LIGHT,
        REWARDS_SCREEN_LIGHT_CLOSED,

        NUM_REWARDS_SCREEN_STATES
    };

    eRewardsScreenState m_currentState;
    float m_elapsedTime;

    Scrooby::Polygon* m_previewLightCover;
    Scrooby::Pure3dObject* m_previewWindow;
    Scrooby::Pure3dObject* m_previewPedestal;
    Scrooby::Pure3dObject* m_previewBgd;
    Scrooby::Sprite* m_previewImage;
    Scrooby::Text* m_previewName;

    // list of garage vehicles
    //
    PreviewObject m_previewVehicles[ MAX_NUM_PREVIEW_VEHICLES ];
    int m_numPreviewVehicles;
    int m_currentPreviewVehicle;

    // list of character skins
    //
    PreviewObject m_previewClothing[ MAX_NUM_PREVIEW_CLOTHING ];
    int m_numPreviewClothing;
    int m_currentPreviewClothing;

    bool m_isLoading : 1;
    bool m_isLoadingReward : 1;

    Scrooby::Group* m_lockedOverlay;
    Scrooby::Text* m_lockedLevel;
    Scrooby::Text* m_rewardPrice;

    Scrooby::Group* m_statsOverlay;
    Scrooby::Group* m_statsOverlayButton;
    Scrooby::Text* m_statsOverlayButtonLabel;
    bool m_statsOverlayToggle;

    enum eVehicleRatingType
    {
        VEHICLE_RATING_SPEED,
        VEHICLE_RATING_ACCELERATION,
        VEHICLE_RATING_TOUGHNESS,
        VEHICLE_RATING_STABILITY,

        NUM_VEHICLE_RATINGS
    };

    PhoneBoothStars* m_vehicleRatings[ NUM_VEHICLE_RATINGS ];

};

#endif // GUISCREENREWARDS_H
