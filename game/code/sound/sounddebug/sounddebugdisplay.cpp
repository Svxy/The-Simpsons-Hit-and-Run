//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sounddebugdisplay.cpp
//
// Description: Implement SoundDebugDisplay
//
// History:     10/26/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <pddi/pddi.hpp>
#include <p3d/utility.hpp>

#include <raddebugwatch.hpp>
#include <radsound_hal.hpp>
#include <radtypeinfo.hpp>

//========================================
// Project Includes
//========================================
#include <sound/sounddebug/sounddebugdisplay.h>

#include <sound/sounddebug/sounddebugpage.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/playermanager.h>

#include <render/IntersectManager/IntersectManager.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/DynaPhysDSG.h>
#include <render/DSG/animcollisionentitydsg.h>
#include <worldsim/avatarmanager.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
//
// Watcher tunables
//
bool SoundDebugDisplay::s_isVisible = false;
int SoundDebugDisplay::s_red = 255;
int SoundDebugDisplay::s_green = 255;
int SoundDebugDisplay::s_blue = 0;
// Text position
int SoundDebugDisplay::s_leftOffset = 0; 
int SoundDebugDisplay::s_topOffset = 0;
// Displayed page
unsigned int SoundDebugDisplay::s_page = 0;
bool SoundDebugDisplay::s_dumpToWindow = false;
// Name display radius
float SoundDebugDisplay::s_radius = 2.0f;
// Type info
bool SoundDebugDisplay::s_dumpTypeInfoToWindow = false;

static const int NUM_VISIBLE_LINES = 15;
static const int NUM_ENTITIES = ( NUM_VISIBLE_LINES * SoundDebugDisplay::MAX_DEBUG_PAGES );

static const int LEFT = 40;
static const int TOP = 40;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundDebugDisplay::SoundDebugDisplay
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundDebugDisplay::SoundDebugDisplay( Sound::daSoundRenderingManager* soundMgr ) :
    m_renderMgr( soundMgr )
{
    int i;
    const char* sectionName = "Sound Info";

    //
    // Register the watcher variables
    //
    radDbgWatchAddBoolean( &s_isVisible, "Set Visibility", sectionName, 0, 0 );
    radDbgWatchAddBoolean( &s_dumpToWindow, "Dump To Window", sectionName, 0, 0 );
    radDbgWatchAddUnsignedInt( &s_page, "Select Page", sectionName, 0, 0, 0, MAX_DEBUG_PAGES - 1 );
    radDbgWatchAddInt( &s_leftOffset, "Left Position", sectionName, 0, 0, -1000, 1000 );
    radDbgWatchAddInt( &s_topOffset, "Top Position", sectionName, 0, 0, -1000, 1000 );
    radDbgWatchAddInt( &s_red, "Text - Red", sectionName, 0, 0, 0, 255 );
    radDbgWatchAddInt( &s_green, "Text - Green", sectionName, 0, 0, 0, 255 );
    radDbgWatchAddInt( &s_blue, "Text - Blue", sectionName, 0, 0, 0, 255 );
    radDbgWatchAddFloat( &s_radius, "Name Display Radius", sectionName, 0, 0, 1.0f, 10.0f );
#ifdef RAD_DEBUG
    // RadScript only provides this capability in debug
    radDbgWatchAddBoolean( &s_dumpTypeInfoToWindow, "Dump Type Info To Window", sectionName );
#endif

    for( i = 0; i < MAX_DEBUG_PAGES; i++ )
    {
        m_debugPages[i] = NULL;
    }
}

//==============================================================================
// SoundDebugDisplay::~SoundDebugDisplay
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundDebugDisplay::~SoundDebugDisplay()
{
    radDbgWatchDelete( &s_isVisible );
    radDbgWatchDelete( &s_dumpToWindow );
    radDbgWatchDelete( &s_page );
    radDbgWatchDelete( &s_leftOffset );
    radDbgWatchDelete( &s_topOffset );
    radDbgWatchDelete( &s_red );
    radDbgWatchDelete( &s_green );
    radDbgWatchDelete( &s_blue );
    radDbgWatchDelete( &s_radius );
#ifdef RAD_DEBUG
    radDbgWatchDelete( &s_dumpTypeInfoToWindow );
#endif
}

//=============================================================================
// SoundDebugDisplay::RegisterPage
//=============================================================================
// Description: Register a debug page for display
//
// Parameters:  page - page to register
//
// Return:      void 
//
//=============================================================================
void SoundDebugDisplay::RegisterPage( SoundDebugPage* page )
{
    int i;

    for( i = 0; i < MAX_DEBUG_PAGES; i++ )
    {
        if( m_debugPages[i] == NULL )
        {
            m_debugPages[i] = page;
            break;
        }
    }
}

//=============================================================================
// SoundDebugDisplay::DeregisterPage
//=============================================================================
// Description: Deregister a debug page for display
//
// Parameters:  page - page to deregister
//
// Return: void
//
//=============================================================================
void SoundDebugDisplay::DeregisterPage( SoundDebugPage* page )
{
    int i;

    for( i = 0; i < MAX_DEBUG_PAGES; i++ )
    {
        if( m_debugPages[i] == page )
        {
            m_debugPages[i] = NULL;
            break;
        }
    }
}

//=============================================================================
// SoundDebugDisplay::Render
//=============================================================================
// Description: Draw the sound debug info
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundDebugDisplay::Render()
{
    if ( Sound::daSoundRenderingManager::GetInstance( ) )
    {
        Sound::daSoundRenderingManager::GetInstance( )->Render( );
    }
    
#ifndef RAD_RELEASE
    Avatar* theAvatar;
    rmt::Vector position;
    rmt::Vector* positionPtr = NULL;
    int i;
    tColour stringColour;

    if( s_dumpTypeInfoToWindow )
    {
        s_dumpTypeInfoToWindow = false;
#ifdef RADSCRIPT_DEBUG
        ::radTypeInfoSystemGet()->DebugDump();
#endif
    }

    if( !s_isVisible )
    {
        return;
    }

    //
    // The new, good way
    //
    if( s_page > 1 )
    {
        stringColour = tColour(s_red, s_green, s_blue);

        for( i = 0; i < MAX_DEBUG_PAGES; i++ )
        {
            if( ( m_debugPages[i] != NULL )
                && ( m_debugPages[i]->GetPage() == s_page ) )
            {
                m_debugPages[i]->Render( LEFT + s_leftOffset, TOP + s_topOffset,
                                         stringColour, s_dumpToWindow );
                break;
            }
        }
    }
    else
    {
        //
        // The old, bad way
        //

        //
        // Get the avatar position
        //
        theAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        if( theAvatar != NULL )
        {
            //
            // Presumably we're in game if we get here
            //
            theAvatar->GetPosition( position );
            positionPtr = &position;
        }

        if( s_page == 0 )
        {
            renderPositionAndHeapInfo( positionPtr );
        }
        else
        {
            renderNearbyObjectNames( positionPtr );
        }
    }
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void SoundDebugDisplay::renderPositionAndHeapInfo( rmt::Vector* position )
{
#ifndef RAD_RELEASE
    char buffy[128];
    tColour stringColour = tColour(s_red, s_green, s_blue);
    IRadSoundHalSystem::Stats radSoundStats;

    //
    // Display the avatar position
    //
    if( position != NULL )
    {
        sprintf( buffy, "Avatar posn: %f %f %f", position->x, position->y, position->z );
        renderTextLine( buffy, LEFT, TOP, stringColour );
    }

    //
    // Display the radSound memory stats
    //
    strcpy( buffy, "RadSound stats:" );
    renderTextLine( buffy, LEFT, TOP + 20, stringColour );

    ::radSoundHalSystemGet()->GetStats( &radSoundStats );
    sprintf( buffy, "Buffers: %d  Voices: %d/%d  PosVoices: %d/%d",
             radSoundStats.m_NumBuffers,
             radSoundStats.m_NumVoicesPlaying,
             radSoundStats.m_NumVoices,
             radSoundStats.m_NumPosVoicesPlaying,
             radSoundStats.m_NumPosVoices );
    renderTextLine( buffy, LEFT, TOP + 40, stringColour );

    sprintf( buffy, "Buffer memory used: %d  Effects memory used: %d",
             radSoundStats.m_BufferMemoryUsed,
             radSoundStats.m_EffectsMemoryUsed );
    renderTextLine( buffy, LEFT, TOP + 60, stringColour );

    sprintf( buffy, "Sound memory free: %d",
             radSoundStats.m_TotalFreeSoundMemory );
    renderTextLine( buffy, LEFT, TOP + 80, stringColour );

    sprintf( buffy, "Sound renderer stats:" );
    renderTextLine( buffy, LEFT, TOP + 120, stringColour );

    sprintf( buffy, "Clip players used: %d", m_renderMgr->GetPlayerManager()->GetNumUsedClipPlayers() );
    renderTextLine( buffy, LEFT, TOP + 140, stringColour );

    sprintf( buffy, "Stream players used: %d", m_renderMgr->GetPlayerManager()->GetNumUsedStreamPlayers() );
    renderTextLine( buffy, LEFT, TOP + 160, stringColour );

    s_dumpToWindow = false;
#endif
}

void SoundDebugDisplay::renderNearbyObjectNames( rmt::Vector* position )
{
#ifndef RAD_RELEASE
    char buffy[128];
    tColour stringColour = tColour(s_red, s_green, s_blue);
    const char* noSound = "NO SOUND";
    ReserveArray<StaticPhysDSG*> statics;
    ReserveArray<DynaPhysDSG*> dynamics;
    ReserveArray<AnimCollisionEntityDSG*> animatics;
    CollisionEntityDSG* collEntityArray[NUM_ENTITIES];
    int arrayIndex = 0;
    int i;

    if( position == NULL )
    {
        //
        // No character to find nearby objects for, draw nothing
        //
        return;
    }

    //
    // Get the intersect goodness
    //
    GetIntersectManager()->FindStaticPhysElems( *position, s_radius, statics );
    GetIntersectManager()->FindDynaPhysElems( *position, s_radius, dynamics );
    GetIntersectManager()->FindAnimPhysElems( *position, s_radius, animatics );

    //
    // Populate the entity array.  Simplifies the code slightly
    // to do it this way.  This is debug stuff, doesn't need to be
    // pretty.
    //
    for( i = 0; i < statics.mUseSize; i++ )
    {
        if( arrayIndex >= NUM_ENTITIES )
        {
            break;
        }
        collEntityArray[arrayIndex] = statics[i];
        ++arrayIndex;
    }
    for( i = 0; i < dynamics.mUseSize; i++ )
    {
        if( arrayIndex >= NUM_ENTITIES )
        {
            break;
        }
        collEntityArray[arrayIndex] = dynamics[i];
        ++arrayIndex;
    }
    for( i = 0; i < animatics.mUseSize; i++ )
    {
        if( arrayIndex >= NUM_ENTITIES )
        {
            break;
        }
        collEntityArray[arrayIndex] = animatics[i];
        ++arrayIndex;
    }

    //
    // Fill the rest of the array with NULLs
    //
    for( i = arrayIndex; i < NUM_ENTITIES; i++ )
    {
        collEntityArray[i] = NULL;
    }

    int startIndex = (s_page - 1) * NUM_VISIBLE_LINES;
    int endIndex = startIndex + NUM_VISIBLE_LINES;

    for( i = startIndex; i < endIndex; ++i )
    {

        if( collEntityArray[i] == NULL )
        {
            break;
        }

        if( collEntityArray[i]->GetCollisionAttributes() == NULL )
        {
            sprintf( buffy, "%-32s\t%-32s", collEntityArray[i]->GetName(), noSound );
        }
        else
        {
            sprintf( buffy, "%-32s\t%-32s", collEntityArray[i]->GetName(), 
                                            collEntityArray[i]->GetCollisionAttributes()->GetSound() );
        }

        renderTextLine( buffy, LEFT, TOP + ((i % NUM_VISIBLE_LINES)*20), stringColour );
    }

    s_dumpToWindow = false;
#endif
}

void SoundDebugDisplay::renderTextLine( const char* text, int leftPosn, 
                                        int topPosn, tColour& colour )
{
    p3d::pddi->DrawString( text, leftPosn + s_leftOffset, topPosn + s_topOffset, colour );

    if ( s_dumpToWindow )
    {
        rDebugString( text );
    }
}
