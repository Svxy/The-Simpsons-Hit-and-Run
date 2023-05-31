//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenHud
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISCREENHUD_H
#define GUISCREENHUD_H

//#define SRR2_MOVABLE_HUD_MAP // enable user to move HUD map

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenmultihud.h>
#include <presentation/gui/utility/slider.h>

#include <events/eventlistener.h>
#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

class HudEventHandler;
class GetOutOfCarCondition;

class MessageQueue
{
    // This is a very simple queue data structure that uses a circular
    // array to store unsigned integer values.
    //
    // m_front - points to the front slot of the queue
    // m_back  - points to the next free slot right behind the back of the queue
    //
public:
    MessageQueue();
    virtual ~MessageQueue();

    void Enqueue( unsigned int messageID );
    unsigned int Dequeue();

    void Clear();
    bool IsEmpty() const { return m_isEmpty; }
    bool IsFull() const { return m_isFull; }

private:
    static const unsigned short MESSAGE_QUEUE_CAPACITY = 8;

    unsigned int m_messages[ MESSAGE_QUEUE_CAPACITY ];
    unsigned short m_front;
    unsigned short m_back;
    bool m_isEmpty : 1;
    bool m_isFull : 1;


};

//===========================================================================
// Interface Definitions
//===========================================================================

enum eHudOverlay
{
    // The following are ordered in precendence priority when more than
    // one mission-related overlay is enabled (w/ the first one being
    // the highest priority).
    //
    HUD_DAMAGE_METER,
	HUD_PROXIMITY_METER,
    HUD_COLLECTIBLES,
    HUD_TIMER,
    HUD_RACE_POSITION,
    HUD_LAP_COUNTER,
    HUD_TIMER_TEMP,
    HUD_PAR_TIME,

    NUM_HUD_MISSION_OVERLAYS, // not really an overlay

    HUD_MISSION_COMPLETE,
    HUD_ACTION_BUTTON,
    HUD_MESSAGE,
    HUD_MAP,

    NUM_HUD_OVERLAYS
};

class CGuiScreenHud : public CGuiScreenMultiHud,
                      public EventListener
{
public:
    CGuiScreenHud( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenHud();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    void Update( unsigned int elapsedTime );

    inline bool IsActive() const;

    // Turns Entire HUD On/Off
    //
    void SetVisible( bool isVisible );

    // Set HUD Overlay Info
    //
    void SetTimerBlinkingInterval( int startTime, int endTime );
    void SetParTime( int parTime );
    void SetCollectibles( int numCollected, int numToCollect );
    void SetRacePosition( int currentPosition, int numPositions );
    void SetLap( int currentLap, int numLaps );
    void SetDamageMeter( float value );
	void SetProximityMeter( float value );
    void SetHitAndRunMeter( float value );

    // Show/Hide Text Message
    //
    void DisplayMessage( bool show, const int index = 0 );

    // Show/Hide All Mission Overlays
    //
    void DisplayMissionOverlays( bool show );

    enum eHudEventHandler
    {
        HUD_EVENT_HANDLER_CARD_COLLECTED,
        HUD_EVENT_HANDLER_COIN_COLLECTED,
        HUD_EVENT_HANDLER_MISSION_PROGRESS,
        HUD_EVENT_HANDLER_MISSION_OBJECTIVE,
        HUD_EVENT_HANDLER_COUNTDOWN,
        HUD_EVENT_HANDLER_HIT_N_RUN,
        HUD_EVENT_HANDLER_WASP_DESTROYED,
        HUD_EVENT_HANDLER_ITEM_DROPPED,

        NUM_HUD_EVENT_HANDLERS
    };

    HudEventHandler* GetEventHandler( eHudEventHandler eventHandler ) const;

    // Implements EventListener
    //
    virtual void HandleEvent( EventEnum id, void* pEventData );

    static void SetNumCoinsDisplay( Scrooby::Sprite* pSprite );
    static void UpdateNumCoinsDisplay( int numCoins, bool show = true );
    void AbortFade();

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();
    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif

private:
    static bool IsMissionOverlay( unsigned int overlayIndex );

    void UpdateOverlays( unsigned int elapsedTime );
    void UpdateEventHandlers( unsigned int elapsedTime );
    void UpdateTimer( unsigned int elapsedTime );

#ifdef SRR2_MOVABLE_HUD_MAP
    void UpdateHudMapPosition( unsigned int elapsedTime );
    void MoveHudMap( int x, int y );
#endif

    static const unsigned int BITMAP_TEXT_BUFFER_SIZE = 8;
    static const int NUM_HIT_N_RUN_SECTORS = 8;

    static const int MAX_NUM_COIN_DIGITS = 8;
    static Scrooby::Sprite* s_numCoinsDisplay;

    Scrooby::Group* m_overlays[ NUM_HUD_OVERLAYS ];
    unsigned int m_elapsedTime[ NUM_HUD_OVERLAYS ];

    Scrooby::Group* m_missionOverlays;
    Scrooby::Sprite* m_missionFailedSprite;

    Scrooby::Text* m_helpMessage;
    Scrooby::Sprite* m_messageBox;
    MessageQueue m_helpMessageQueue;

#ifdef RAD_WIN32
    Scrooby::Text* m_actionButton;
    Scrooby::Text* m_actionLabel;
#else
    Scrooby::Sprite* m_actionButton;
#endif

    Scrooby::Sprite* m_missionComplete;

    Scrooby::Sprite* m_timer;
    tColour m_defaultTimerColour;
    int m_timerBlinkingStartTime;
    int m_timerBlinkingEndTime;

    Scrooby::Sprite* m_parTime;

    Scrooby::Sprite* m_collectibles;
    int m_collectiblesUpdated;

    Scrooby::Sprite* m_position;
    Scrooby::Text* m_positionOrdinal;

    Scrooby::Sprite* m_lap;
    int m_lapUpdated;

    ImageSlider m_damageMeter;
    bool m_isSlidingDamageMeter;

    ImageSlider m_proximityMeter;

    bool m_isFadingInRadar : 1;
    bool m_isFadingOutRadar : 1;

    Scrooby::Sprite* m_hnrSectors[ NUM_HIT_N_RUN_SECTORS ];
    Scrooby::Sprite* m_hnrLights;
    Scrooby::Sprite* m_hnrMeter;
    Scrooby::Sprite* m_hnrMeterBgd;
    unsigned int m_hnrElapsedTime;

    HudEventHandler* m_hudEventHandlers[ NUM_HUD_EVENT_HANDLERS ];

    bool m_isFadingIn : 1;
    bool m_isFadingOut : 1;
    float m_elapsedFadeTime;

    bool m_isBonusMissionJustCompleted : 1;

    bool m_isAvatarOffRoad : 1;
    unsigned int m_avatarOffRoadDurationTime;

    GetOutOfCarCondition* m_pGetOutOfCarCondition;

    float m_elapsedFgdFadeInTime;

};

inline bool CGuiScreenHud::IsActive() const
{
    // TC: if the start button was just pressed, we'll treat the screen
    //     as in-active, since it's about to transition out anyways
    //
    return ( m_state == GUI_WINDOW_STATE_RUNNING && !m_isStartButtonPressed );
}

inline HudEventHandler* CGuiScreenHud::GetEventHandler( eHudEventHandler eventHandler ) const
{
    return m_hudEventHandlers[ eventHandler ];
}

inline bool CGuiScreenHud::IsMissionOverlay( unsigned int overlayIndex )
{
    return( static_cast<int>( overlayIndex ) < NUM_HUD_MISSION_OVERLAYS );
}

#endif // GUISCREENHUD_H
