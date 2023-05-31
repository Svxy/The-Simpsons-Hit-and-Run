//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CHudMap
//
// Description: Implementation of the CHudMap class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/01      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/utility/hudmap.h>
#include <presentation/gui/utility/hudmapcam.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guiscreen.h>

#include <ai/vehicle/vehicleai.h>
#include <interiors/interiormanager.h>
#include <memory/srrmemory.h>
#include <meta/zoneeventlocator.h>
#include <mission/gameplaymanager.h>
#include <render/intersectmanager/intersectmanager.h>
#include <roads/geometry.h>
#include <roads/road.h>
#include <roads/intersection.h>
#include <roads/roadmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/hitnrunmanager.h>
#include <worldsim/redbrick/vehicle.h>

#include <p3d/utility.hpp>

#include <raddebug.hpp> // Foundation

#include <page.h>
#include <group.h>
#include <pure3dobject.h>
#include <sprite.h>
#include <polygon.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#define SHOW_HUD_MAP
#define CIRCULAR_HUD_MAP

#ifdef CIRCULAR_HUD_MAP
    #define SMARTER_ICON_LOCATIONS // only works for circular hud map
#endif

// This is for smooth camera height transitions, to prevent any sudden
// camera cuts.
//
#define SMOOTH_CAMERA_HEIGHT_TRANSITIONS

const int NUM_AMORTIZED_UPDATE_FRAMES = 5; // for DetermineOnRoadLocation()

#ifdef DEBUGWATCH
    #include <raddebugwatch.hpp>

    static const char* WATCHER_NAMESPACE = "GUI System - HUD Map";

    float MIN_CAMERA_HEIGHT = 50.0f;
    float MAX_CAMERA_HEIGHT = 150.0f;

    float MAX_CAMERA_HEIGHT_TRANSITION = 10.0f;

    float CHASE_FLASH_DISTANCE_THRESHOLD = 0.5f;
    float CHASE_FADE_DISTANCE_THRESHOLD = 0.75f;
    float RADAR_VISIBLE_RADIUS = 150.0f; // in metres
#else
    const float MIN_CAMERA_HEIGHT = 50.0f;
    const float MAX_CAMERA_HEIGHT = 150.0f;

    const float MAX_CAMERA_HEIGHT_TRANSITION = 10.0f;

    const float CHASE_FLASH_DISTANCE_THRESHOLD = 0.5f;
    const float CHASE_FADE_DISTANCE_THRESHOLD = 0.75f;
    const float RADAR_VISIBLE_RADIUS = 150.0f; // in metres
#endif

const float DEFAULT_CAMERA_HEIGHT = 150.0f; // in metres

const float MAX_RADAR_CONE_ALPHA = 0.35f;

enum eVisibilityMode
{
    VISIBLE_IN_SUNDAY_DRIVE = 1,
    VISIBLE_IN_MISSION = 2,

    NUM_VISIBILITY_MODES
};

const tColour HIT_N_RUN_COLOUR_RED( 255, 0, 0 );
const tColour HIT_N_RUN_COLOUR_BLUE( 0, 0, 255 );

HudMapIcon CHudMap::s_registeredIcons[ MAX_NUM_REGISTERED_ICONS ];
int CHudMap::s_numRegisteredIcons = 0;
int CHudMap::s_fpIconID = -1;

//===========================================================================
// Public Member Functions
//===========================================================================

void
HudMapIcon::ApplyAICarIconColour()
{
    rAssert( m_iconImage != NULL );

    switch( m_type )
    {
        case ICON_AI_HIT_N_RUN:
        {
            m_iconImage->SetColour( HIT_N_RUN_COLOUR_RED );

            break;
        }
        case ICON_AI_CHASE:
        {
            m_iconImage->SetColour( tColour( 255, 128, 0 ) );

            break;
        }
        case ICON_AI_RACE:
        {
            m_iconImage->SetColour( tColour( 255, 255, 0 ) );

            break;
        }
        case ICON_AI_EVADE:
        {
            m_iconImage->SetColour( tColour( 255, 255, 0 ) );

            break;
        }
        case ICON_AI_TARGET:
        {
            m_iconImage->SetColour( tColour( 255, 0, 0 ) );

            break;
        }
        default:
        {
            rAssertMsg( false, "Invalid AI car icon type!" );

            break;
        }
    }
}

//===========================================================================
// CHudMap::CHudMap
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CHudMap::CHudMap( Scrooby::Page* pPage, int playerID, const char* p3dFile )
:   m_playerID( playerID ),
    m_p3dMap( NULL ),
    m_p3dHole( NULL ),
    m_originalPosX( 0 ),
    m_originalPosY( 0 ),
    m_originalWidth( 0 ),
    m_originalHeight( 0 ),
    m_isVisible( false ),
    m_radar( NULL ),
    m_radarCone( NULL ),
    m_iconsGroup( NULL ),
    m_hudMapCam( NULL ),
    m_currentCameraHeight( DEFAULT_CAMERA_HEIGHT ),
    m_fixedCameraHeight( 0.0f ),
    m_currentAICarDistance( 0.0f ),
    m_maxAICarDistance( 0.0f ),
    m_elapsedTime( 0 ),
    m_lastRoadSeg( NULL ),
    m_lastOnRoadLocation( 0.0f, 0.0f, 0.0f ),
    m_frameCount( -1 )
{
MEMTRACK_PUSH_GROUP( "CHudMap" );
    memset( m_icons, 0, sizeof( m_icons ) );

    m_hudMapCam = new HudMapCam( m_playerID );
    rAssert( m_hudMapCam );
    m_hudMapCam->AddRef();

    m_currentCameraHeight = this->CalculateCameraHeight( RADAR_VISIBLE_RADIUS );

    char name[ 16 ];
    rAssert( pPage != NULL );

    // get 3D HUD map
    //
    sprintf( name, "Map%d", playerID );
    m_p3dMap = pPage->GetPure3dObject( name );
    rAssert( m_p3dMap != NULL );
    m_p3dMap->SetCamera( m_hudMapCam->GetCamera() );

    // store original hud map position and size
    //
    m_p3dMap->GetOriginPosition( m_originalPosX, m_originalPosY );
    m_p3dMap->GetBoundingBoxSize( m_originalWidth, m_originalHeight );

#ifdef SHOW_HUD_MAP
    if( p3dFile != NULL )
    {
        // set pure3d file for hud map
        //
        m_p3dMap->Add3dModel( p3dFile );
    }

    // get 3D HUD hole
    //
    sprintf( name, "Hole%d", playerID );
    m_p3dHole = pPage->GetPure3dObject( name );
    rAssert( m_p3dHole != NULL );
    m_p3dHole->SetCamera( m_hudMapCam->GetCamera() );
    m_p3dHole->SetColourWrite( false );

    if( CGuiScreen::IsWideScreenDisplay() )
    {
        m_p3dMap->SetWideScreenCorrectionEnabled( true );
        m_p3dHole->SetWideScreenCorrectionEnabled( true );
    }
#else
    m_p3dMap->SetVisible( false );
#endif

/*
    sprintf( name, "RadarLight%d", playerID );
    m_radarLight = pPage->GetSprite( name );
    rAssert( m_radarLight != NULL );

    // hide radar light temporarily
    //
    m_radarLight->SetVisible( false );
*/

    // get radar & radar cone
    //
    sprintf( name, "Radar%d", playerID );
    m_radar = pPage->GetGroup( name );
    rAssert( m_radar != NULL );

    sprintf( name, "RadarCone%d", playerID );
    m_radarCone = pPage->GetGroup( name );
    rAssert( m_radarCone != NULL );

    Scrooby::Polygon* radarConePolygon = m_radarCone->GetPolygon( name );
    rAssert( radarConePolygon != NULL );
    radarConePolygon->SetAlpha( MAX_RADAR_CONE_ALPHA );

    // get icons group
    //
    sprintf( name, "Icons%d", playerID );
    m_iconsGroup = pPage->GetGroup( name );
    rAssert( m_iconsGroup != NULL );

#ifdef DEBUGWATCH
    ::radDbgWatchAddFloat( &MIN_CAMERA_HEIGHT,
                           "Mininum Camera Height",
                           WATCHER_NAMESPACE,
                           NULL, NULL, 30.0f, 90.0f );

    ::radDbgWatchAddFloat( &MAX_CAMERA_HEIGHT,
                           "Maximum Camera Height",
                           WATCHER_NAMESPACE,
                           NULL, NULL, 100.0f, 1000.0f );

    ::radDbgWatchAddFloat( &MAX_CAMERA_HEIGHT_TRANSITION,
                           "Maximum Camera Height Change",
                           WATCHER_NAMESPACE,
                           NULL, NULL, 1.0f, 50.0f );

    ::radDbgWatchAddFloat( &RADAR_VISIBLE_RADIUS,
                           "Radar Visible Radius",
                           WATCHER_NAMESPACE,
                           NULL, NULL, 0.0f, 1000.0f );

    ::radDbgWatchAddFloat( &CHASE_FLASH_DISTANCE_THRESHOLD,
                           "Chase Flash Distance (%)",
                           WATCHER_NAMESPACE );

    ::radDbgWatchAddFloat( &CHASE_FADE_DISTANCE_THRESHOLD,
                           "Chase Fade Distance (%)",
                           WATCHER_NAMESPACE );
#endif
MEMTRACK_POP_GROUP("CHudMap");
}

//===========================================================================
// CHudMap::~CHudMap
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CHudMap::~CHudMap()
{
#ifdef DEBUGWATCH
    ::radDbgWatchDelete( &MIN_CAMERA_HEIGHT );
    ::radDbgWatchDelete( &MAX_CAMERA_HEIGHT );
    ::radDbgWatchDelete( &MAX_CAMERA_HEIGHT_TRANSITION );
    ::radDbgWatchDelete( &RADAR_VISIBLE_RADIUS );
    ::radDbgWatchDelete( &CHASE_FLASH_DISTANCE_THRESHOLD );
    ::radDbgWatchDelete( &CHASE_FADE_DISTANCE_THRESHOLD );
#endif

    if( m_hudMapCam != NULL )
    {
        m_hudMapCam->Release();
        m_hudMapCam = NULL;
    }
}

//===========================================================================
// CHudMap::SetCameraTarget
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::SetCameraTarget( ISuperCamTarget* target )
{
    rAssert( m_hudMapCam );
    m_hudMapCam->SetTarget( target );
}

void
CHudMap::EnableFixedCameraHeight( bool enable, float visibleRadius )
{
/*
    if( !enable )
    {
        rWarningMsg( false, "Can't disable fixed camera height in new radar implementation!" );

        // ignore request to disable fixed camera height
        //
        return;
    }
*/
    m_fixedCameraHeight = enable ?
                          visibleRadius / rmt::Tan( m_hudMapCam->GetFOV() / 2.0f ) :
                          0.0f;
}

//===========================================================================
// CHudMap::Update
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::Update( unsigned int elapsedTime )
{
    if( GetInteriorManager()->IsInside() )
    {
        this->SetVisible( false );
    }

    if( !m_isVisible )
    {
        // no need to update if not visible
        //
        return;
    }

/*
    // update radar light (just rotate about radar center point)
    //
    static float RADAR_ROTATION_PERIOD = 2000.0f; // in msec
    float rotation = (elapsedTime / RADAR_ROTATION_PERIOD) * 360.0f;
    rAssert( m_radarLight != NULL );
    m_radarLight->RotateAboutCenter( rotation );
*/

    // get current hud map position and size
    //
    int mapX = 0;
    int mapY = 0;
    int mapWidth = 0;
    int mapHeight = 0;

    rAssert( m_p3dMap != NULL );
    m_p3dMap->GetOriginPosition( mapX, mapY );
    m_p3dMap->GetBoundingBoxSize( mapWidth, mapHeight );

    float newCameraHeight = DEFAULT_CAMERA_HEIGHT;
    if( m_fixedCameraHeight != 0.0f ) // fixed camera height enabled
    {
        newCameraHeight = m_fixedCameraHeight;
    }
    else
    {
        if( s_fpIconID != -1 )
        {
            HudMapIcon* focalPointIcon = &(s_registeredIcons[ s_fpIconID ]);
            rAssert( focalPointIcon->m_iconImage );

            if( focalPointIcon->m_dynamicLocator != NULL )
            {
                focalPointIcon->m_dynamicLocator->GetPosition( &(focalPointIcon->m_location) );
            }

            newCameraHeight = this->CalculateCameraHeight( s_registeredIcons[ 0 ].m_location,
                                                           focalPointIcon->m_location );
        }
    }

#ifdef SMOOTH_CAMERA_HEIGHT_TRANSITIONS
    // for smoother camera height transitions
    //
    float cameraHeightChange = newCameraHeight - m_currentCameraHeight;
    if( rmt::Abs( cameraHeightChange ) > MAX_CAMERA_HEIGHT_TRANSITION )
    {
        m_currentCameraHeight += rmt::Sign( cameraHeightChange ) * MAX_CAMERA_HEIGHT_TRANSITION;
//        rTunePrintf( "Current camera height = %f m\n", m_currentCameraHeight );
    }
    else
#endif
    {
        m_currentCameraHeight = newCameraHeight;
    }

    // update hud map camera
    //
    rAssert( m_hudMapCam != NULL );
    m_hudMapCam->SetHeight( m_currentCameraHeight );
    m_hudMapCam->SetAspect( (float)mapWidth / (float)mapHeight );
    m_hudMapCam->Update( elapsedTime );

    rmt::Vector hudMapCenter;
    m_hudMapCam->GetPosition( &hudMapCenter );

#ifdef CIRCULAR_HUD_MAP
    // update hud hole
    //
    if( m_p3dHole != NULL )
    {
        static float awayFromCam = 4.5f;
        m_p3dHole->SetDrawableTranslation( hudMapCenter.x,
                                           hudMapCenter.y - awayFromCam,
                                           hudMapCenter.z );
    }
#endif

    // update registered icons
    //
    int iconPosX = 0;
    int iconPosY = 0;
    int iconWidth = 0;
    int iconHeight = 0;

    const unsigned int PULSE_PERIOD = 500;
//    bool iconBlinked = false;

//    rWarningMsg( s_fpIconID != -1, "No active focal point icon on hud map!" );

    for( unsigned int i = 0; i < MAX_NUM_REGISTERED_ICONS; i++ )
    {
        if( s_registeredIcons[ i ].m_iconImage != NULL )
        {
            // check if visible in current gameplay mode
            //
            if( GetGameplayManager()->IsSundayDrive() )
            {
                if( (s_registeredIcons[ i ].m_visibilityMask & VISIBLE_IN_SUNDAY_DRIVE) == 0 )
                {
                    s_registeredIcons[ i ].m_iconImage->SetVisible( false );
                    continue;
                }
            }
            else
            {
                if( (s_registeredIcons[ i ].m_visibilityMask & VISIBLE_IN_MISSION) == 0 )
                {
                    s_registeredIcons[ i ].m_iconImage->SetVisible( false );
                    continue;
                }
            }

            // hide icons that are not visible when player is in car
            //
            if( s_registeredIcons[ i ].m_type == HudMapIcon::ICON_PHONE_BOOTH ||
                s_registeredIcons[ i ].m_type == HudMapIcon::ICON_PLAYER_CAR )
            {
                Avatar* player = GetAvatarManager()->GetAvatarForPlayer( m_playerID );
                rAssert( player != NULL );
                if( player->IsInCar() )
                {
                    s_registeredIcons[ i ].m_iconImage->SetVisible( false );
                    continue;
                }
            }

//            s_registeredIcons[ i ].m_iconImage->SetAlpha( 1.0f );

            // center icon in hud map first
            //
            s_registeredIcons[ i ].m_iconImage->GetBoundingBoxSize( iconWidth, iconHeight );
            iconPosX = (mapX + mapWidth / 2) - (iconWidth / 2);
            iconPosY = (mapY + mapHeight / 2) - (iconHeight / 2);
            s_registeredIcons[ i ].m_iconImage->SetPosition( iconPosX, iconPosY );

            s_registeredIcons[ i ].m_iconImage->ResetTransformation();

            // update current location for dynamic icons
            if( s_registeredIcons[ i ].m_dynamicLocator != NULL )
            {
                s_registeredIcons[ i ].m_dynamicLocator->GetPosition( &s_registeredIcons[ i ].m_location );

                // place icon object at sea level
                s_registeredIcons[ i ].m_location.y = 0;

                if( s_registeredIcons[ i ].IsAICarIcon() ||
                    s_registeredIcons[ i ].m_type == HudMapIcon::ICON_PLAYER ||
                    s_registeredIcons[ i ].m_type == HudMapIcon::ICON_PLAYER_CAR )
                {
                    rmt::Vector heading( 0, 0, 0 );
                    s_registeredIcons[ i ].m_dynamicLocator->GetHeading( &heading );

                    this->UpdateIconHeading( s_registeredIcons[ i ].m_iconImage, &heading );
                }
            }

            rmt::Vector iconLocInWorld = s_registeredIcons[ i ].m_location;

#ifdef SMARTER_ICON_LOCATIONS
            if( static_cast< int >( i ) == s_fpIconID )
            {
                // TC: [TODO] need to determine this from the maximum camera height and the
                //            clipping boundary value
                //
                static float visibleRadius = 120.0f;

                this->DetermineOnRoadLocation( hudMapCenter,
                                               iconLocInWorld,
                                               visibleRadius );
            }
#endif

            rmt::Vector iconLoc( 0, 0, 0 );
            rAssert( m_p3dMap->GetCamera() );
            m_p3dMap->GetCamera()->WorldToView( iconLocInWorld, &iconLoc );
            iconLoc.z = 0.0f;

            static float CLIPPING_BOUNDARY = 0.40f;
            float iconScale = 1.0f;

#ifdef CIRCULAR_HUD_MAP
            // apply view clipping (for circular view)
            //
            float iconLength = iconLoc.Magnitude();
            if( iconLength > CLIPPING_BOUNDARY )
            {
                iconScale = CLIPPING_BOUNDARY / iconLength;
                iconLoc.Scale( iconScale );
            }
#else
            // apply view clipping (for rectangular view)
            //

            // x-clipping
            float xScale = 1.0f;
            if( iconLoc.x > CLIPPING_BOUNDARY )
            {
                xScale = CLIPPING_BOUNDARY / iconLoc.x;
                iconLoc.Scale( xScale, xScale, 1 );
            }
            else if( iconLoc.x < -CLIPPING_BOUNDARY )
            {
                xScale = -CLIPPING_BOUNDARY / iconLoc.x;
                iconLoc.Scale( xScale, xScale, 1 );
            }

            // y-clipping
            float yScale = 1.0f;
            if( iconLoc.y > CLIPPING_BOUNDARY )
            {
                yScale = CLIPPING_BOUNDARY / iconLoc.y;
                iconLoc.Scale( yScale, yScale, 1 );
            }
            else if( iconLoc.y < -CLIPPING_BOUNDARY )
            {
                yScale = -CLIPPING_BOUNDARY / iconLoc.y;
                iconLoc.Scale( yScale, yScale, 1 );
            }

            iconScale = xScale * yScale;
#endif

            bool isIconClipped = (iconScale < 1.0f);

            rAssert( m_radarCone != NULL );
            m_radarCone->SetVisible( s_fpIconID != -1 );

            if( static_cast< int >( i ) == s_fpIconID ) // for focal point icon ...
            {
                // update radar cone direction
                //
                m_radarCone->ResetTransformation();
                m_radarCone->RotateAboutCenter( rmt::RadianToDeg( this->CalculatRadarConeAngle( iconLoc ) ) );

                if( !isIconClipped || s_registeredIcons[ i ].IsAICarIcon() )
                {
                    m_radarCone->SetAlpha( iconScale );
                }
                else
                {
                    const float MINIMUM_ICON_SCALE = 0.75f;
                    float alpha = (iconScale - MINIMUM_ICON_SCALE) / (1.0f - MINIMUM_ICON_SCALE);
                    m_radarCone->SetAlpha( alpha > 0.0f ? alpha : 0.0f );
                }
/*
                // apply alpha if focal point icon is clipped on boundary
                //
                if( iconScale < 1.0f )
                {
                    if( iconScale < 0.3f )
                    {
                        s_registeredIcons[ i ].m_iconImage->SetAlpha( 0.3f );
                    }
                    else
                    {
                        s_registeredIcons[ i ].m_iconImage->SetAlpha( iconScale );
                    }
                }
                else
                {
                    s_registeredIcons[ i ].m_iconImage->SetAlpha( 1.0f );
                }
*/

#ifdef SMARTER_ICON_LOCATIONS
                s_registeredIcons[ i ].m_iconImage->SetVisible( true );
#else
                if( !s_registeredIcons[ i ].IsAICarIcon() )
                {
                    // hide non-AI car icons if clipped on boundary
                    //
                    s_registeredIcons[ i ].m_iconImage->SetVisible( !isIconClipped );
                }
#endif

                // special cases
                //
                if( s_registeredIcons[ i ].m_type == HudMapIcon::ICON_MISSION )
                {
                    static float SCALE_AMPLITUDE = 0.25f;

                    float scale = GuiSFX::Pulse( (float)m_elapsedTime,
                                                 (float)PULSE_PERIOD,
                                                 1.0f + SCALE_AMPLITUDE,
                                                 SCALE_AMPLITUDE );

/*
                    float scale = 1.0f + (float)m_elapsedTime / PULSE_PERIOD * SCALE_AMPLITUDE;
                    float alpha = 1.0f - (float)m_elapsedTime / PULSE_PERIOD * ALPHA_AMPLITUDE;

                    s_registeredIcons[ i ].m_iconImage->SetAlpha( alpha );
*/
                    s_registeredIcons[ i ].m_iconImage->ScaleAboutCenter( scale );
                }
/*
                else if( s_registeredIcons[ i ].m_type == HudMapIcon::ICON_AI_CAR )
                {
                    // if focal point is AI car icon, fade out when near
                    // distance limit
                    //
                    if( m_maxAICarDistance > 0.0f )
                    {
                        float distanceRatio = m_currentAICarDistance / m_maxAICarDistance;
                        rAssert( distanceRatio <= 1.0f );

                        if( distanceRatio > CHASE_FLASH_DISTANCE_THRESHOLD )
                        {
                            // blink icon
                            //
                            iconBlinked = GuiSFX::Blink( s_registeredIcons[ i ].m_iconImage,
                                                         (float)m_elapsedTime,
                                                         (float)BLINKING_PERIOD );

                            if( distanceRatio > CHASE_FADE_DISTANCE_THRESHOLD )
                            {
                                float iconAlpha = (1.0f - distanceRatio) /
                                                  (1.0f - CHASE_FADE_DISTANCE_THRESHOLD);

                                s_registeredIcons[ i ].m_iconImage->SetAlpha( iconAlpha );
                            }
                            else
                            {
                                s_registeredIcons[ i ].m_iconImage->SetAlpha( 1.0f );
                            }
                        }
                        else
                        {
                            s_registeredIcons[ i ].m_iconImage->SetVisible( true );
                        }
                    }
                }
*/
            }
            else // for non-focal point icons ...
            {
                if( !s_registeredIcons[ i ].IsAICarIcon() )
                {
                    // hide non-AI car icons if clipped on boundary
                    //
                    s_registeredIcons[ i ].m_iconImage->SetVisible( !isIconClipped );
                }
/*
                // for phone booth icons
                //
                if( s_registeredIcons[ i ].m_type == HudMapIcon::ICON_PHONE_BOOTH )
                {
                    // only show if within visible radius
                    //
                    rmt::Vector playerToPhone;
                    playerToPhone.Sub( s_registeredIcons[ i ].m_location,
                                       s_registeredIcons[ 0 ].m_location );

                    float distanceRatio = playerToPhone.Magnitude() / PHONE_BOOTH_VISIBLE_RADIUS;
                    bool isWithinRadius = (distanceRatio < 1.0f);

                    // fade out nicely to prevent popping
                    //
                    const float FADE_OUT_THRESHOLD = 0.9f;
                    if( !isIconClipped && isWithinRadius )
                    {
                        if( distanceRatio > FADE_OUT_THRESHOLD )
                        {
                            float iconAlpha = (1.0f - distanceRatio) / (1.0f - FADE_OUT_THRESHOLD);
                            s_registeredIcons[ i ].m_iconImage->SetAlpha( iconAlpha );
                        }
                        else
                        {
                            s_registeredIcons[ i ].m_iconImage->SetAlpha( 1.0f );
                        }
                    }
                    else
                    {
                        s_registeredIcons[ i ].m_iconImage->SetAlpha( 0.0f );
                    }
                }
*/
            }

            if( s_registeredIcons[ i ].m_type == HudMapIcon::ICON_AI_HIT_N_RUN )
            {
                if( GetHitnRunManager()->IsHitnRunActive() )
                {
                    s_registeredIcons[ i ].m_iconImage->SetVisible( true );

                    // modulate icon colour
                    //
                    tColour iconColour;
                    GuiSFX::ModulateColour( &iconColour,
                                            (float)m_elapsedTime,
                                            (float)PULSE_PERIOD,
                                            HIT_N_RUN_COLOUR_RED,
                                            HIT_N_RUN_COLOUR_BLUE );

                    s_registeredIcons[ i ].m_iconImage->SetColour( iconColour );
                }
                else
                {
                    // hide icon if H&R is not active
                    //
                    s_registeredIcons[ i ].m_iconImage->SetVisible( false );
                }
            }

            // translate the icon (in screen space wrt. hud map center)
            //
            int iconX = static_cast<int>( iconLoc.x * mapWidth );
            int iconY = static_cast<int>( iconLoc.y * mapHeight );

            s_registeredIcons[ i ].m_iconImage->Translate( iconX, iconY );
        }
    }
/*
    if( iconBlinked )
    {
        m_elapsedTime %= PULSE_PERIOD;
    }
*/
    m_elapsedTime = (m_elapsedTime + elapsedTime) % PULSE_PERIOD;
}

//===========================================================================
// CHudMap::AddIconToInventory
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::AddIconToInventory( HudMapIcon::eIconType type, Scrooby::Sprite* image )
{
    if( image == NULL )
    {
        return;
    }

    unsigned int i = 0;

    for( i = 0; i < MAX_NUM_ICONS_PER_TYPE; i++ )
    {
        // find first empty slot in inventory to add icon
        //
        if( m_icons[ type ][ i ] == NULL )
        {
            m_icons[ type ][ i ] = image;

            // hide icon image
            //
            image->SetVisible( false );

            break;
        }
    }

    rAssertMsg( i < MAX_NUM_ICONS_PER_TYPE,
                "ERROR: *** Exceeded maximum number of icons per type!\n" );
}

//===========================================================================
// CHudMap::RemoveIconFromInventory
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
Scrooby::Sprite*
CHudMap::RemoveIconFromInventory( HudMapIcon::eIconType type )
{
    Scrooby::Sprite* iconImage = NULL;

    // find an available icon of specified type in inventory
    //
    for( unsigned int i = 0; i < MAX_NUM_ICONS_PER_TYPE; i++ )
    {
        if( m_icons[ type ][ i ] != NULL )
        {
            iconImage = m_icons[ type ][ i ];
            iconImage->SetVisible( true );

            // free up slot in inventory
            //
            m_icons[ type ][ i ] = NULL;

            break;
        }
    }

    return iconImage;
}

//===========================================================================
// CHudMap::RegisterIcon
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
int
CHudMap::RegisterIcon( HudMapIcon::eIconType type,
                       rmt::Vector location,
                       IHudMapIconLocator* hudMapIconLocator,
                       bool newFocalPoint )
{
    rAssert( s_numRegisteredIcons < static_cast<int>( MAX_NUM_REGISTERED_ICONS ) );

    int iconID = -1;

    Scrooby::Sprite* iconImage = this->RemoveIconFromInventory( type );
    if( iconImage == NULL )
    {
        rAssertMsg( false, "WARNING: *** No more available icons of this type! Please tell Tony." );

        return -1;
    }

    // convert locations in interiors to world-space location of the interior
    //
    tName interiorName = GetInteriorManager()->ClassifyPoint( location );
    if( interiorName.GetUID() != static_cast< tUID >( 0 ) )
    {
        ZoneEventLocator* locator = p3d::find<ZoneEventLocator>( interiorName.GetUID() );
        locator->GetPosition( &location );

        if( hudMapIconLocator != NULL )
        {
            hudMapIconLocator = locator;
        }
    }

#ifdef RAD_DEBUG
    // check if there's already a static icon of the same type
    // registered at exactly the same location
    //
    for( unsigned int j = 0; j < MAX_NUM_REGISTERED_ICONS; j++ )
    {
        if( s_registeredIcons[ j ].m_iconImage != NULL &&
            s_registeredIcons[ j ].m_type == type )
        {
            rAssertMsg( s_registeredIcons[ j ].m_dynamicLocator != NULL ||
                        s_registeredIcons[ j ].m_location != location,
                        "WARNING: *** Another icon of this type has already been registered at this location. Is this by design??" );
        }
    }
#endif // RAD_DEBUG

    for( unsigned int i = 0; i < MAX_NUM_REGISTERED_ICONS; i++ )
    {
        if( i == 0 && type != HudMapIcon::ICON_PLAYER )
        {
            // icon ID = 0 is reserved for player icon
            continue;
        }

        // find first empty slot to register icon
        if( s_registeredIcons[ i ].m_iconImage == NULL )
        {
            iconID = i;
            s_numRegisteredIcons++;

            // place icon object at sea level
            location.y = 0;

            s_registeredIcons[ iconID ].m_iconImage = iconImage;
            s_registeredIcons[ iconID ].m_type = type;
            s_registeredIcons[ iconID ].m_location = location;
            s_registeredIcons[ iconID ].m_dynamicLocator = hudMapIconLocator;

            // set visible in sunday drive mode for these icon types
            //
            if( type == HudMapIcon::ICON_PLAYER ||
                type == HudMapIcon::ICON_PLAYER_CAR ||
                type == HudMapIcon::ICON_MISSION ||
                type == HudMapIcon::ICON_BONUS_MISSION ||
                type == HudMapIcon::ICON_STREET_RACE ||
                type == HudMapIcon::ICON_WAGER_RACE ||
                type == HudMapIcon::ICON_PURCHASE_CENTRE ||
                type == HudMapIcon::ICON_PHONE_BOOTH ||
                type == HudMapIcon::ICON_AI_HIT_N_RUN )
            {
                s_registeredIcons[ i ].m_visibilityMask |= VISIBLE_IN_SUNDAY_DRIVE;
            }

            // set visible in mission mode for these icon types
            //
            if( type == HudMapIcon::ICON_PLAYER ||
                type == HudMapIcon::ICON_PLAYER_CAR ||
                type == HudMapIcon::ICON_FLAG_CHECKERED ||
                type == HudMapIcon::ICON_FLAG_WAYPOINT ||
                type == HudMapIcon::ICON_COLLECTIBLE ||
                type == HudMapIcon::ICON_MISSION ||
                s_registeredIcons[ i ].IsAICarIcon() )
            {
                s_registeredIcons[ i ].m_visibilityMask |= VISIBLE_IN_MISSION;
            }

            // colour code AI car icons
            //
            if( s_registeredIcons[ i ].IsAICarIcon() )
            {
                s_registeredIcons[ i ].ApplyAICarIconColour();
            }

            break;
        }
    }

    rAssertMsg( iconID != -1, "ERROR: *** There should have been an empty slot!\n" );

    if( newFocalPoint )
    {
        // set as new focal point icon
        //
        s_fpIconID = iconID;

        // whenever the target changes, we want to recompute its closest road segment
        m_lastRoadSeg = NULL;

        m_frameCount = -1; // reset frame count
    }


    return iconID;
}

//===========================================================================
// CHudMap::UnregisterIcon
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::UnregisterIcon( int iconID )
{
    if( iconID != -1 )
    {
        rAssert( iconID < static_cast<int>( MAX_NUM_REGISTERED_ICONS ) );

        if( s_registeredIcons[ iconID ].m_iconImage != NULL )
        {
            // return icon to inventory
            //
            this->AddIconToInventory( s_registeredIcons[ iconID ].m_type,
                                      s_registeredIcons[ iconID ].m_iconImage );

            s_registeredIcons[ iconID ].m_iconImage = NULL;
            s_registeredIcons[ iconID ].m_visibilityMask = 0;
            s_numRegisteredIcons--;

            if( s_fpIconID == iconID )
            {
                s_fpIconID = -1;
            }
        }
    }
    else
    {
        rAssertMsg( false, "This should only happen if you didn't get a valid icon ID upon registration!" );
    }
}

//=============================================================================
// CHudMap::ChangeIconType
//=============================================================================
// Description: Comment
//
// Parameters:  ( HudMapIcon::eIconType type, int iconID )
//
// Return:      void 
//
//=============================================================================
int
CHudMap::ChangeIconType( int iconID, HudMapIcon::eIconType type )
{
    int newIconID = -1;

    if( iconID != -1 )
    {
        bool isFocalPointIcon = (iconID == s_fpIconID);

        // save a copy of old icon
        //
        HudMapIcon* oldIcon = &s_registeredIcons[ iconID ];

        // unregister it first
        //
        this->UnregisterIcon( iconID );

        // re-registered icon of new type
        //
        newIconID = this->RegisterIcon( type, oldIcon->m_location, oldIcon->m_dynamicLocator, isFocalPointIcon );
    }

    return newIconID;
}

//===========================================================================
// CHudMap::SetFocalPointIcon
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::SetFocalPointIcon( int iconID )
{
    if( iconID != -1 )
    {
        rAssert( iconID < static_cast< int >( MAX_NUM_REGISTERED_ICONS ) );
        s_fpIconID = iconID;

        // whenever the target changes, we want to recompute its closest road segment
        m_lastRoadSeg = NULL;

        m_frameCount = -1; // reset frame count
    }
}

//===========================================================================
// CHudMap::FindIcon
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
HudMapIcon*
CHudMap::FindIcon( HudMapIcon::eIconType type ) const
{
    const HudMapIcon* icon = NULL;

    for( unsigned int i = 0; i < MAX_NUM_REGISTERED_ICONS; i++ )
    {
        if( s_registeredIcons[ i ].m_iconImage != NULL &&
            s_registeredIcons[ i ].m_type == type )
        {
            // found it!
            //
            icon = &(s_registeredIcons[ i ]);

            break;
        }
    }

    return const_cast<HudMapIcon*>( icon );
}

//===========================================================================
// CHudMap::Translate
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CHudMap::Translate( int x, int y )
{
    rAssert( m_p3dMap );

    m_originalPosX += x;
    m_originalPosY += y;
    m_p3dMap->SetPosition( m_originalPosX, m_originalPosY );
    if( m_p3dHole != NULL )
    {
        m_p3dHole->SetPosition( m_originalPosX, m_originalPosY );
    }

    this->Update( 0 );
}

//===========================================================================
// CHudMap::Reset
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CHudMap::Reset()
{
    rAssert( m_p3dMap != NULL );

    // restore hud map position and size
    //
    m_p3dMap->SetPosition( m_originalPosX, m_originalPosY );
    m_p3dMap->SetBoundingBoxSize( m_originalWidth, m_originalHeight );

    if( m_p3dHole != NULL )
    {
        m_p3dHole->SetPosition( m_originalPosX, m_originalPosY );
        m_p3dHole->SetBoundingBoxSize( m_originalWidth, m_originalHeight );
    }

    // restore hud map camera
    //
    m_p3dMap->SetCamera( m_hudMapCam->GetCamera() );

    m_elapsedTime = 0;

    this->Update( 0 );
}

//===========================================================================
// CHudMap::SetVisible
//===========================================================================
// Description: Toggle visibility of 3D map.
//
// Constraints:	None.
//
// Parameters:	visibility
//
// Return:      
//
//===========================================================================
void CHudMap::SetVisible( bool isVisible )
{
    m_isVisible = isVisible;

    rAssert( m_p3dMap != NULL );
    m_p3dMap->SetVisible( isVisible );

    rAssert( m_radar != NULL );
    m_radar->SetVisible( isVisible );

    rAssert( m_iconsGroup != NULL );
    m_iconsGroup->SetVisible( isVisible );
}

void
CHudMap::RestoreAllRegisteredIcons()
{
    if( s_numRegisteredIcons > 0 )
    {
        int numIconsRestored = 0;

        for( unsigned int i = 0; i < MAX_NUM_REGISTERED_ICONS; i++ )
        {
            if( s_registeredIcons[ i ].m_iconImage != NULL )
            {
                s_registeredIcons[ i ].m_iconImage = this->RemoveIconFromInventory( s_registeredIcons[ i ].m_type );
                rAssert( s_registeredIcons[ i ].m_iconImage != NULL );

                if( s_registeredIcons[ i ].IsAICarIcon() )
                {
                    s_registeredIcons[ i ].ApplyAICarIconColour();
                }

                numIconsRestored++;
            }
        }

        rAssertMsg( numIconsRestored == s_numRegisteredIcons, "Not all registered icons were restored!" );
    }
}

void
CHudMap::ClearAllRegisteredIcons()
{
    memset( s_registeredIcons, 0, sizeof( s_registeredIcons ) );
    s_numRegisteredIcons = 0;
    s_fpIconID = -1;
}

//===========================================================================
// Private Member Functions
//===========================================================================

//===========================================================================
// CHudMap::CalculateDistanceBetweenPoints
//===========================================================================
// Description: Calculates the scalar distance between 2 points in 3D-space.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
float
CHudMap::CalculateDistanceBetweenPoints( rmt::Vector& a, rmt::Vector& b )
{
    rmt::Vector displacement( 0.0f, 0.0f, 0.0f );
    displacement.Sub( a, b );

    return displacement.Magnitude();
}

//===========================================================================
// CHudMap::UpdateIconHeading
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CHudMap::UpdateIconHeading( Scrooby::Sprite* iconImage, rmt::Vector* iconHeading )
{
    rmt::Vector objectHeading( iconHeading->x, 0, iconHeading->z );

    rmt::Vector camHeading;
    tPointCamera* camera = (tPointCamera*)m_p3dMap->GetCamera();
    rAssert( camera );
    camera->GetVUp( &camHeading );
    camHeading.y = 0.0f;

    if( camHeading.MagnitudeSqr() > 0.0f && objectHeading.MagnitudeSqr() > 0.0f )
    {
        float ratio = camHeading.Dot( objectHeading ) /
                      (camHeading.Magnitude() * objectHeading.Magnitude());

        float rotation = 0.0f;

        if( ratio > 1.0f )
        {
            rotation = 0.0f;
        }
        else if( ratio < -1.0f )
        {
            rotation = rmt::PI;
        }
        else
        {
            rotation = rmt::ACos( ratio );
        }

        rAssert( !rmt::IsNan( rotation ) );

        rmt::Vector normal = camHeading;
        normal.CrossProduct( objectHeading );
        if( normal.y < 0 )
        {
            rotation = -rotation;
        }

        rAssert( iconImage );
        iconImage->RotateAboutCenter( rmt::RadianToDeg( rotation ) );
    }
}

//===========================================================================
// CHudMap::CalculateCameraHeight
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
float
CHudMap::CalculateCameraHeight( rmt::Vector& player, rmt::Vector& mission ) const
{
    // fix locations at sea level
    //
    player.y = 0;
    mission.y = 0;

    float distance = CalculateDistanceBetweenPoints( player, mission );

    // calculate camera height based on distance
    //
    float cameraHeight = this->CalculateCameraHeight( distance * 1.5f );

    if( cameraHeight < MIN_CAMERA_HEIGHT )
    {
        cameraHeight = MIN_CAMERA_HEIGHT;
    }
    else if( cameraHeight > MAX_CAMERA_HEIGHT )
    {
        cameraHeight = MAX_CAMERA_HEIGHT;
    }

    return cameraHeight;
}

//===========================================================================
// CHudMap::CalculateCameraHeight
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
float
CHudMap::CalculateCameraHeight( float visibleRadius ) const
{
    float cameraHeight = visibleRadius;
    if( rmt::Abs( m_hudMapCam->GetFOV() - rmt::PI_BY2 ) > 0.001f )
    {
        cameraHeight = visibleRadius / rmt::Tan( m_hudMapCam->GetFOV() / 2.0f );
    }

    return cameraHeight;
}

//===========================================================================
// CHudMap::CalculatRadarConeAngle
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
float
CHudMap::CalculatRadarConeAngle( rmt::Vector& iconLoc ) const
{
    float radarConeAngle = 0.0f; // zero angle by default

    if( iconLoc.y > 0.0f ) // icon is located in either Quadrant I or II
    {
        radarConeAngle = rmt::ATan( iconLoc.x / iconLoc.y );
    }
    else if( iconLoc.y < 0.0f ) // icon is located in either Quadrant III or IV
    {
        radarConeAngle = rmt::ATan( iconLoc.x / iconLoc.y ) + rmt::PI;
    }
    else // icon is located somewhere along the X-axis
    {
        if( iconLoc.x > 0.0f )
        {
            radarConeAngle = rmt::PI_BY2;
        }
        else if( iconLoc.x < 0.0f )
        {
            radarConeAngle = -rmt::PI_BY2;
        }
    }

    return radarConeAngle;
}

//===========================================================================
// CHudMap::DetermineOnRoadLocation
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================

// We return the target vector (where we want to direct the navigation cone)
void
CHudMap::DetermineOnRoadLocation( rmt::Vector& source, rmt::Vector& target, float visibleRadius )
{
    // place source and target at sea level
    //
    source.y = 0;
    target.y = 0;

    float distance = CalculateDistanceBetweenPoints( source, target );
    if( distance <= visibleRadius )
    {
        // no need to do any calculations if target is within 'visibleRadius' from source
        //
        return;
    }

    // check to see if we should skip this update and return cached information instead
    //
    m_frameCount = (m_frameCount + 1) % NUM_AMORTIZED_UPDATE_FRAMES;
    if( m_frameCount != 0 )
    {
        target = m_lastOnRoadLocation;

        return;
    }

    // get reference to road manager
    //
    RoadManager* roadManager = RoadManager::GetInstance();
    rAssert( roadManager != NULL );

    //////////////
    // find path between source and target
    //
    bool isPathElementFound = false;
    float searchRadius = 200.0f;
    float dummy;

    // Find source's closest path element
    RoadSegment* sourceSeg = NULL;
    float sourceSegT = 0.0f;
    RoadManager::PathElement sourceElem;
    float sourceRoadT = 0.0f;

    isPathElementFound = VehicleAI::FindClosestPathElement( source, 
        sourceElem, sourceSeg, sourceSegT, sourceRoadT, false ); // don't want shortcuts on hudmap

    if( !isPathElementFound )
    {
        GetIntersectManager()->FindClosestRoad( source, searchRadius, sourceSeg, dummy ); // ignore shortcuts
        rAssertMsg( sourceSeg, "Should always find a nearby road seg! Increase search radius!" );
        if( !sourceSeg )
        {
            return;
        }

        sourceElem.type = RoadManager::ET_NORMALROAD;
        sourceElem.elem = sourceSeg->GetRoad();
        sourceSegT = roadManager->DetermineSegmentT( source, sourceSeg );
        sourceRoadT = roadManager->DetermineRoadT( sourceSeg, sourceSegT );
    }
    rAssert( sourceElem.elem );


    // Find target's closest path element
    RoadSegment* targetSeg = NULL;
    float targetSegT = 0.0f;
    RoadManager::PathElement targetElem;
    float targetRoadT = 0.0f;

    isPathElementFound = VehicleAI::FindClosestPathElement( target, 
        targetElem, targetSeg, targetSegT, targetRoadT, false ); // don't want shortcuts on hudmap

    if( !isPathElementFound )
    {
        RoadSegment* closestRoadSeg = NULL;
        if( m_lastRoadSeg )
        {
            closestRoadSeg = m_lastRoadSeg;
        }
        else
        {
            GetIntersectManager()->FindClosestRoad( target, searchRadius, closestRoadSeg, dummy );
            rAssertMsg( closestRoadSeg, "Should always find a nearby road seg! Increase search radius!" );
            if( !closestRoadSeg )
            {
                return;
            }
            m_lastRoadSeg = closestRoadSeg;
        }
        targetElem.type = RoadManager::ET_NORMALROAD;
        targetElem.elem = closestRoadSeg->GetRoad();
        targetSeg = closestRoadSeg;
        targetSegT = roadManager->DetermineSegmentT( target, targetSeg );
        targetRoadT = roadManager->DetermineRoadT( targetSeg, targetSegT );
    }
    else
    {
        // need to make this check cuz FindClosestPathElement could return an intersection
        if( targetSeg ) 
        {
            m_lastRoadSeg = targetSeg;
        }
    }
    rAssert( targetElem.elem );


    HeapMgr()->PushHeap( GMA_TEMP );

    SwapArray<RoadManager::PathElement> pathElements;
    pathElements.Allocate( roadManager->GetMaxPathElements() );

    RoadManager::PathElement tmpSrcElem = sourceElem;
    float roadDistance = roadManager->FindPathElementsBetween( false,
                                                               tmpSrcElem, sourceRoadT, source,
                                                               targetElem, targetRoadT, target,
                                                               pathElements );

    rAssert( pathElements.mUseSize > 0 );

    // Temp stuff we use over and over again
    RoadManager::PathElement* prevElem = &(pathElements[0]);
    bool isRoadBackwards = false;
    int numIntersects = 0;
    rmt::Vector intPts[2];

    rmt::Sphere visibleSphere( source, visibleRadius );     // this is the hud map circle

    // find closest path element that intersects visible sphere (or circle, really)
    //
    bool isClosestPathElementFound = false;

    if( pathElements.mUseSize == 1 )
    {
        // if only one element returned
        if( pathElements[0].type == RoadManager::ET_INTERSECTION )
        {
            // if the only elem is an intersection, just point to target
            numIntersects = IntersectLineSphere( source, target, visibleSphere, intPts );
            rAssert( numIntersects == 1 );
            target = intPts[0];
        }
        else // the only element is a road
        {
            Road* theRoad = (Road*) targetElem.elem;

            // we know here that one element returned is either cuz:
            // A) source and target lie on same road 
            // or 
            // B) source and target lie on opposite roads that
            // describe the same physical road (target road is returned)

            // case B
            if( sourceElem != targetElem ) 
            {
                // iterate through target road's segments to find the closest seg
                // to source, the t value of this segment will tell us whether to
                // traverse the segments backwards or forwards...
                //
                rmt::Vector closestPos;
                float closestDist;
                int closestSegIndex;

                RoadManager::FindClosestPointOnRoad( theRoad, source, closestPos, closestDist, closestSegIndex );

                rAssert( 0 <= closestSegIndex && closestSegIndex < (int) theRoad->GetNumRoadSegments() );
                RoadSegment* closestSeg = theRoad->GetRoadSegment( (unsigned int) closestSegIndex );
                rAssert( closestSeg );

                sourceElem.elem = theRoad;
                sourceSeg = closestSeg;
                sourceSegT = RoadManager::DetermineSegmentT( closestPos, closestSeg );
                sourceRoadT = RoadManager::DetermineRoadT( sourceSeg, sourceSegT );
            }

            rAssert( sourceElem == targetElem );

            // this should now work for either A or B
            if( sourceRoadT > targetRoadT )
            {
                isRoadBackwards = true;
            }

            unsigned int startIndex = sourceSeg->GetSegmentIndex();
            unsigned int endIndex = targetSeg->GetSegmentIndex();

            bool foundIntersect = false;
            if( !isRoadBackwards )
            {
                for( unsigned int i=startIndex; i<=endIndex; i++ )
                {
                    RoadSegment* seg = theRoad->GetRoadSegment( i );

                    rmt::Vector vec0, vec1, vec2, vec3, start, end;
                    seg->GetCorner( 0, vec0 );
                    seg->GetCorner( 1, vec1 );
                    seg->GetCorner( 2, vec2 );
                    seg->GetCorner( 3, vec3 );

                    start = (vec0 + vec3) * 0.5f;
                    start.y = 0.0f;
                    end = (vec1 + vec2) * 0.5f;
                    end.y = 0.0f;

                    int numIntersects = IntersectLineSphere( start, end, visibleSphere, intPts );
                    rAssert( 0 <= numIntersects && numIntersects <= 1 );

                    if( numIntersects == 1 )
                    {
                        target = intPts[0];
                        isClosestPathElementFound = true;

                        break;
                    }
                }
            }
            else
            {
                for( int i=(int)startIndex; i>=(int)endIndex; i-- )
                {
                    RoadSegment* seg = theRoad->GetRoadSegment( (unsigned int)i );

                    rmt::Vector vec0, vec1, vec2, vec3, start, end;
                    seg->GetCorner( 0, vec0 );
                    seg->GetCorner( 1, vec1 );
                    seg->GetCorner( 2, vec2 );
                    seg->GetCorner( 3, vec3 );

                    start = (vec0 + vec3) * 0.5f;
                    start.y = 0.0f;
                    end = (vec1 + vec2) * 0.5f;
                    end.y = 0.0f;

                    int numIntersects = IntersectLineSphere( start, end, visibleSphere, intPts );
                    rAssert( 0 <= numIntersects && numIntersects <= 1 );

                    if( numIntersects == 1 )
                    {
                        target = intPts[0];
                        isClosestPathElementFound = true;

                        break;
                    }
                }
            }
        }
    }
    else
    {
        // At this point, we're dealing with multiple path elements
        //
        Intersection* previousIntersection = NULL;
        for( int i = 0; i < pathElements.mUseSize; i++ )
        {
            if( isClosestPathElementFound )
            {
                // we're done! we found the closest path element
                //
                break;
            }

            RoadManager::PathElement* currentPathElement = &( pathElements[ i ] );
            rAssert( currentPathElement != NULL );

            switch( currentPathElement->type )
            {
                case RoadManager::ET_NORMALROAD:
                {
                    Road* road = static_cast<Road*>( currentPathElement->elem );
                    rAssert( road != NULL );

                    int numRoadSegments = static_cast<int>( road->GetNumRoadSegments() );

                    // determine which direction to iterate over all road segments
                    //
                    bool isRoadBackwards = false;

                    if( i == 0 ) // special case if first path element is a road
                    {
                        for( int j = 1; j < pathElements.mUseSize; j++ )
                        {
                            RoadManager::PathElement* pathElement = &( pathElements[ j ] );
                            rAssert( pathElement != NULL );

                            if( pathElement->type == RoadManager::ET_INTERSECTION )
                            {
                                isRoadBackwards = ( road->GetSourceIntersection() == static_cast<Intersection*>( pathElement->elem ) );

                                break;
                            }
                        }
                    }
                    else
                    {
                        isRoadBackwards = ( road->GetSourceIntersection() != previousIntersection );
                    }

                    int currentRoadSegmentIndex = isRoadBackwards ? numRoadSegments - 1 : 0;

                    while( currentRoadSegmentIndex >= 0 && currentRoadSegmentIndex < numRoadSegments )
                    {
                        RoadSegment* currentRoadSegment = road->GetRoadSegment( currentRoadSegmentIndex );
                        rmt::Vector vec0, vec1, vec2, vec3, start, end;

                        currentRoadSegment->GetCorner( 0, vec0 );
                        currentRoadSegment->GetCorner( 1, vec1 );
                        currentRoadSegment->GetCorner( 2, vec2 );
                        currentRoadSegment->GetCorner( 3, vec3 );

                        start = (vec0 + vec3) * 0.5f;
                        start.y = 0.0f;
                        end = (vec1 + vec2) * 0.5f;
                        end.y = 0.0f;

                        rmt::Vector intersectPoints[ 2 ];
                        int numIntersectPoints = IntersectLineSphere( start, end, visibleSphere, intersectPoints );
                        if( numIntersectPoints > 0 )
                        {
                            rmt::Vector closestIntersectPoint = intersectPoints[ 0 ];

                            // TC [TODO]: if more than one intersection points, need to determine which
                            //            one is closest to source
                            //
                            if( numIntersectPoints > 1 )
                            {
                                rTuneWarningMsg( false, "Multiple intersection points not yet handled properly, but this should not happen here!" );
                            }

                            // check to make sure that this point is, in fact, closer to the target than the source
                            //
                            if( i == 0 ) // only need to check this for the first path element
                            {
                                float roadSegT = roadManager->DetermineSegmentT( closestIntersectPoint, const_cast<RoadSegment*>( currentRoadSegment ) );
                                float intersectRoadT = roadManager->DetermineRoadT( const_cast<RoadSegment*>( currentRoadSegment ), roadSegT );

                                if( ( isRoadBackwards && intersectRoadT < sourceRoadT) ||
                                    (!isRoadBackwards && intersectRoadT > sourceRoadT) )
                                {
                                    target = closestIntersectPoint;
                                    isClosestPathElementFound = true;

                                    break; // stop iterating over road segments
                                }
                                else
                                {
                                    int dummy = 0;
                                }
                            }
                            else
                            {
                                target = closestIntersectPoint;
                                isClosestPathElementFound = true;

                                break; // stop iterating over road segments
                            }
                        }

                        currentRoadSegmentIndex += isRoadBackwards ? -1 : +1;
                    }

                    break;
                }
                case RoadManager::ET_INTERSECTION:
                {
                    Intersection* intersection = static_cast<Intersection*>( currentPathElement->elem );
                    rAssert( intersection != NULL );

                    // keep track of previous intersection
                    //
                    previousIntersection = intersection;

                    rmt::Vector intersectionLocation;
                    intersection->GetLocation( intersectionLocation );
                    intersectionLocation.y = 0.0f;
                    float distance = CalculateDistanceBetweenPoints( source, intersectionLocation );

                    if( rmt::Abs( distance - visibleRadius ) <= intersection->GetRadius() )
                    {
                        // project target onto radar edge, on a point that is closest to the
                        // intersection center point
                        //
                        rmt::Vector sourceToIntersection;
                        sourceToIntersection.Sub( intersectionLocation, source );
                        sourceToIntersection.Scale( visibleRadius / distance );

                        target.Add( source, sourceToIntersection );

                        isClosestPathElementFound = true;
                    }

                    break;
                }
                default:
                {
                    rAssertMsg( false, "Invalid path element type!" );

                    break;
                }
            }
        }
    }

    HeapMgr()->PopHeap( GMA_TEMP );

    target.y = 0.0f;
    m_lastOnRoadLocation = target;

#ifndef RAD_RELEASE
    if( !isClosestPathElementFound )
    {
        // TC: [INVESTIGATE] if closest path element is not found, there must be some discontinuity
        //                   in the closest path found
        //
        rTuneWarningMsg( false, "Closest path element not found! Please go tell Tony." );
    }
#endif
}

