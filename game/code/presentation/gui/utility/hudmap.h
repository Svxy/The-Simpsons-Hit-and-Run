//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CHudMap
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/01      TChu        Created for SRR2
//
//===========================================================================

#ifndef HUDMAP_H
#define HUDMAP_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <p3d/p3dtypes.hpp>
#include <radmath/radmath.hpp>

//===========================================================================
// Forward References
//===========================================================================

#ifndef NULL
    #define NULL 0
#endif

const unsigned int MAX_NUM_REGISTERED_ICONS = 64;
const unsigned int MAX_NUM_ICONS_PER_TYPE = 32;

struct HudMapCam;
class ISuperCamTarget;
class tPointCamera;
class RoadSegment;

namespace Scrooby
{
    class Page;
    class Group;
    class Pure3dObject;
    class Sprite;
}

// pure virtual interface for dynamic hud map icons
//
struct IHudMapIconLocator
{
    virtual void GetPosition( rmt::Vector* currentLoc ) = 0;
    virtual void GetHeading( rmt::Vector* heading ) = 0;
};

struct HudMapIcon
{
    enum eIconType
    {
        UNKNOWN_ICON_TYPE = -1,

        ICON_PLAYER,
        ICON_PLAYER_CAR,

        ICON_AI_TYPES_BEGIN = ICON_PLAYER_CAR,  // not an icon type
        ICON_AI_HIT_N_RUN,
        ICON_AI_CHASE,
        ICON_AI_RACE,
        ICON_AI_EVADE,
        ICON_AI_TARGET,
        ICON_AI_TYPES_END,                  // not an icon type

        ICON_FLAG_CHECKERED = ICON_AI_TYPES_END,
        ICON_FLAG_WAYPOINT,
        ICON_COLLECTIBLE,
        ICON_MISSION,
        ICON_BONUS_MISSION,
        ICON_PHONE_BOOTH,
        ICON_PURCHASE_CENTRE,
        ICON_STREET_RACE,
        ICON_WAGER_RACE,

        NUM_ICON_TYPES
    };

    Scrooby::Sprite* m_iconImage;
    eIconType m_type;
    rmt::Vector m_location;
    IHudMapIconLocator* m_dynamicLocator;
    unsigned int m_visibilityMask;

    HudMapIcon()
    :   m_iconImage( NULL ),
        m_type( UNKNOWN_ICON_TYPE ),
        m_location( 0, 0, 0 ),
        m_dynamicLocator( NULL ),
        m_visibilityMask( 0 )
    {
    }

    bool IsAICarIcon() const
    {
        return( static_cast<int>( m_type ) > ICON_AI_TYPES_BEGIN &&
                static_cast<int>( m_type ) < ICON_AI_TYPES_END );
    }

    void ApplyAICarIconColour();

};

//===========================================================================
// Interface Definitions
//===========================================================================
class CHudMap
{
public:
    CHudMap( Scrooby::Page* pPage, int playerID, const char* p3dFile = NULL );
    virtual ~CHudMap();

    // Camera Target must be set before doing anything else!
    //
    void SetCameraTarget( ISuperCamTarget* target );

    // enable/disable fixed camera height (if enabling, the fixed camera
    // height will be calculated from the visible radius)
    //
    void EnableFixedCameraHeight( bool enable,
                                  float visibleRadius = 50.0f );

    // Update
    //
    void Update( unsigned int elapsedTime );

    // for AI Car icons only
    //
    void UpdateAICarDistance( float currentDistance, float maxDistance );

    // Map Icons
    //
    void AddIconToInventory( HudMapIcon::eIconType type, Scrooby::Sprite* image );
    Scrooby::Sprite* RemoveIconFromInventory( HudMapIcon::eIconType type );

    int RegisterIcon( HudMapIcon::eIconType type, rmt::Vector location,
                      IHudMapIconLocator* hudMapIconLocator = NULL,
                      bool newFocalPoint = false );

    void UnregisterIcon( int iconID );

    int ChangeIconType( int iconID, HudMapIcon::eIconType type );
    void SetFocalPointIcon( int iconID );
    HudMapIcon* FindIcon( HudMapIcon::eIconType type ) const;

    // Reset position and size to original settings
    //
    void Translate( int x, int y );
    void Reset();

    void SetVisible( bool isVisible );

    // Accessors
    //
    Scrooby::Pure3dObject* GetMap() const { return m_p3dMap; }
    Scrooby::Pure3dObject* GetHole() const { return m_p3dHole; }
    HudMapCam* GetHudMapCam() const { return m_hudMapCam; }

    void RestoreAllRegisteredIcons();
    static void ClearAllRegisteredIcons();

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or asignment. Declare but don't define.
    //
    CHudMap( const CHudMap& );
    CHudMap& operator= ( const CHudMap& );

    static float CalculateDistanceBetweenPoints( rmt::Vector& a, rmt::Vector& b );

    void UpdateIconHeading( Scrooby::Sprite* iconImage, rmt::Vector* iconHeading );

    float CalculateCameraHeight( rmt::Vector& player, rmt::Vector& mission ) const;
    float CalculateCameraHeight( float visibleRadius ) const;
    float CalculatRadarConeAngle( rmt::Vector& iconLoc ) const;

    void DetermineOnRoadLocation( rmt::Vector& source, rmt::Vector& target, float visibleRadius );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    int m_playerID;

    Scrooby::Pure3dObject* m_p3dMap;
    Scrooby::Pure3dObject* m_p3dHole;
    int m_originalPosX;
    int m_originalPosY;
    int m_originalWidth;
    int m_originalHeight;

    bool m_isVisible : 1;
    Scrooby::Group* m_radar;
    Scrooby::Group* m_radarCone;
    Scrooby::Group* m_iconsGroup;

    HudMapCam* m_hudMapCam;
    float m_currentCameraHeight;
    float m_fixedCameraHeight;

    Scrooby::Sprite* m_icons[ HudMapIcon::NUM_ICON_TYPES ][ MAX_NUM_ICONS_PER_TYPE ];

    // for AI Car icons only
    //
    float m_currentAICarDistance;
    float m_maxAICarDistance;

    unsigned int m_elapsedTime;

    RoadSegment* m_lastRoadSeg;
    rmt::Vector m_lastOnRoadLocation;
    int m_frameCount;

    // gameplay-persistent data
    //
    static HudMapIcon s_registeredIcons[ MAX_NUM_REGISTERED_ICONS ];
    static int s_numRegisteredIcons;
    static int s_fpIconID;

};

inline void CHudMap::UpdateAICarDistance( float currentDistance, float maxDistance )
{
    m_currentAICarDistance = currentDistance;
    m_maxAICarDistance = maxDistance;
}

#endif // HUDMAP_H
