//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintdrawable.h
//
// Description: Blahblahblah
//
// History:     2/8/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERSPRINTDRAWABLE_H
#define SUPERSPRINTDRAWABLE_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
#include <p3d/debugdraw.hpp>
#include <p3d/drawable.hpp>
#include <p3d/sprite.hpp>
#include <p3d/utility.hpp>
#include <p3d/texture.hpp>
#include <p3d/file.hpp>
#include <p3d/shader.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/memory.hpp>
#include <p3d/bmp.hpp>
#include <p3d/png.hpp>
#include <p3d/targa.hpp>
#include <p3d/font.hpp>
#include <p3d/texturefont.hpp>
#include <p3d/unicode.hpp>
#include <radfile.hpp>

#include <string.h>

#include <supersprint/supersprintdata.h>

#include <mission/gameplaymanager.h>

#include <camera/supercammanager.h>

#include <debug/debuginfo.h>

//========================================
// Forward References
//========================================

extern unsigned char gFont[];

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperSprintDrawable : public tDrawable
{
public:
        SuperSprintDrawable();
        virtual ~SuperSprintDrawable();

        void SetCarData( const SuperSprintData::CarData* carData );
        void SetPlayerData( const SuperSprintData::PlayerData* playerData );
        void SetCountDownMSG( const char* text );
        void SetTextScale( float scale );
        void DoCountDownToo( bool enable );

        void Display();

        enum RenderState
        {
            CAR_DATA,
            PLAYER_DATA,
            COUNT_DOWN,
            HIGH_SCORES,
            NONE
        };

        void SetRenderState( RenderState state );

private:

    const SuperSprintData::CarData* mCarData;
    const SuperSprintData::PlayerData* mPlayerData;

    RenderState mRenderState;

    tTextureFont* mFont;

    const char* mCountDownText;

    float mTextScale;

    bool mCountDownToo;

    bool mILoadedThefont;

    inline void DisplayCarData();
    inline void DisplayPlayerData();
    inline void DisplayCountDown();
    inline void DisplayHighScores();

    //Prevent wasteful constructor creation.
    SuperSprintDrawable( const SuperSprintDrawable& supersprintdrawable );
    SuperSprintDrawable& operator=( const SuperSprintDrawable& supersprintdrawable );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperSprintDrawable::SuperSprintDrawable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      none
//
//=============================================================================
inline SuperSprintDrawable::SuperSprintDrawable() :
    mCarData( NULL ),
    mPlayerData( NULL ),
    mRenderState( CAR_DATA ),
    mFont( NULL ),
    mCountDownText( NULL ),
    mTextScale( 1.0f ),
    mCountDownToo( false ),
    mILoadedThefont( false )
{
    p3d::inventory->PushSection();

    p3d::inventory->SelectSection( "Default" );
    mFont = p3d::find<tTextureFont>("adlibn_20");

    if ( mFont == NULL )
    {
       tFileMem* file = new tFileMem( gFont , 61075 );  //HACK
        file->AddRef();
        file->SetFilename("memfile.p3d");
        p3d::loadManager->GetP3DHandler()->Load( file, p3d::inventory );
        file->Release();

        mILoadedThefont = true;
        mFont = p3d::find<tTextureFont>("adlibn_20");
    }

    rAssert( mFont );

    mFont->AddRef();
    tShader* fontShader = mFont->GetShader();
    fontShader->SetInt(PDDI_SP_BLENDMODE,PDDI_BLEND_ALPHA);
    fontShader->SetInt(PDDI_SP_FILTER,PDDI_FILTER_BILINEAR);

    // Make the missing letter into somthing I can see
    //
    mFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));

    p3d::inventory->PopSection();    
}

//=============================================================================
// ::~SuperSprintDrawable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      none
//
//=============================================================================
inline SuperSprintDrawable::~SuperSprintDrawable()
{
    if ( mILoadedThefont )
    {
#ifndef DEBUGINFO_ENABLED
        p3d::inventory->Remove( mFont );  //Debuginfo uses this too.
#endif
        mILoadedThefont = false;
    }
    mFont->Release();
    mFont = NULL;
}

//=============================================================================
// SuperSprintDrawable::SetCarData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const SuperSprintData::CarData* carData )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::SetCarData( const SuperSprintData::CarData* carData )
{
    mCarData = carData;
}

//=============================================================================
// SuperSprintDrawable::SetPlayerData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const SuperSprintData::PlayerData* playerData )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::SetPlayerData( const SuperSprintData::PlayerData* playerData )
{
    mPlayerData = playerData;
}

//=============================================================================
// SuperSprintDrawable::SetCountDownMSG
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* text )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::SetCountDownMSG( const char* text )
{
    mCountDownText = text;
}

//=============================================================================
// SuperSprintDrawable::SetTextScale
//=============================================================================
// Description: Comment
//
// Parameters:  ( float scale )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::SetTextScale( float scale )
{
    mTextScale = scale;
}

//=============================================================================
// SuperSprintDrawable::DoCountDownToo
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::DoCountDownToo( bool enable )
{
    mCountDownToo = enable;
}

//=============================================================================
// SuperSprintDrawable::DisplayCarData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::DisplayCarData()
{
    char displayText[1024];

    const char* seperator;
    unsigned int position;
    if ( GetGameplayManager()->GetNumPlayers() == 2 )
    {
        seperator = "\n\n";
        position = 2;
    }
    else
    {
        seperator = "\n";
        position = 1;
    }

    sprintf( displayText, seperator);

    int i;
    for ( i = 0; i < GetGameplayManager()->GetNumPlayers(); ++ i )
    {      
        char text[256];
        if ( mCarData[i].mState == SuperSprintData::CarData::WAITING )
        {
            sprintf( text, "Player %d\n%s%s", i + 1, "PRESS START", seperator );
        }
        else if ( mCarData[i].mState == SuperSprintData::CarData::SELECTING )
        {
            sprintf( text, "Player %d\n%s%s", i + 1, mCarData[ i ].mCarName, seperator );
        }
        else if ( mCarData[i].mState == SuperSprintData::CarData::SELECTED )
        {
            sprintf( text, "Player %d\n%s%s", i + 1, "READY", seperator );
        }

        strncpy( &displayText[position], text, strlen( text ) );
        position += strlen( text );
    }

    P3D_UNICODE unicodeText[1024];
    p3d::AsciiToUnicode( displayText, unicodeText, position + 1 );

    mFont->DisplayText( unicodeText, 3 );
}

//=============================================================================
// SuperSprintDrawable::DisplayCountDown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::DisplayCountDown()
{
    P3D_UNICODE unicodeText[256];

    p3d::AsciiToUnicode( mCountDownText, unicodeText, 256 );

    mFont->DisplayText( unicodeText, 3 );
}

//=============================================================================
// SuperSprintDrawable::DisplayPlayerData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::DisplayPlayerData()
{
    char displayText[1024];
    const char* seperator = "\n";
    sprintf( displayText, seperator);

    unsigned int position = 1;

    int i;
    for ( i = 0; i < GetGameplayManager()->GetNumPlayers(); ++ i )
    {
        char text[256];

        if( !mPlayerData[ i ].mRacing )
        {
            sprintf( text, "Player %d%s", i + 1, seperator );
        }
        else
        {
            if ( mPlayerData[ i ].mPosition == 0 )
            {
                sprintf( text, "Player %d %s%sDNF%sLOSER!%s", i + 1, "PRESS START", seperator, seperator, seperator );
            }
            else
            {
                sprintf( text, "Player %d %s%sRace Time: %.2f%sBest Lap: %.2f%s", i + 1, "PRESS START", seperator, rmt::LtoF(mPlayerData[ i ].mRaceTime) / 1000000.0f, seperator, rmt::LtoF(mPlayerData[ i ].mBestLap) / 1000000.0f, seperator );
            }
        }

        strncpy( &displayText[position], text, strlen( text ) );
        position += strlen( text );
    }

    P3D_UNICODE unicodeText[1024];
    p3d::AsciiToUnicode( displayText, unicodeText, position + 1 );

    mFont->DisplayText( unicodeText, 3 );
}

//=============================================================================
// SuperSprintDrawable::DisplayHighScores
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
/*
inline void SuperSprintDrawable::DisplayHighScores()
{
    char displayText[1024];
    displayText[0] = '\0';

    const char* seperator = "\n";

    sprintf( displayText, seperator);

    unsigned int position = 1;

    char text[256];

    sprintf( text, "BEST TIME           BEST LAP%s=================++==========%s", seperator, seperator );
    strncpy( &displayText[position], text, strlen( text ) );
    position += strlen( text );

    int j;
    for ( j = 0; j < SuperSprintData::HighScore::NUM_HIGH_SCORE; ++j )
    {
        sprintf( text, "%8s %5s %.2f  %8s %5s %.2f%s", SuperSprintData::BEST_TIME[j].name, 
                                                       SuperSprintData::VEHICLE_NAMES[SuperSprintData::BEST_TIME[j].carNum].name,
                                                       SuperSprintData::BEST_TIME[j].score / 1000000.0f, 
                                                       SuperSprintData::BEST_LAP[j].name, 
                                                       SuperSprintData::VEHICLE_NAMES[SuperSprintData::BEST_LAP[j].carNum].name,
                                                       SuperSprintData::BEST_LAP[j].score / 1000000.0f,
                                                       seperator );

        strncpy( &displayText[position], text, strlen( text ) );
        position += strlen( text );
    }

    P3D_UNICODE unicodeText[1024];
    p3d::AsciiToUnicode( displayText, unicodeText, position + 1 );

    mFont->DisplayText( unicodeText, 3 );
}
*/

//=============================================================================
// SuperSprintDrawable::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::Display()
{
    tColour colour( 240, 240, 0 );
    colour.SetAlpha( 255 );

    mFont->SetColour( colour );

    p3d::pddi->PushState( PDDI_STATE_VIEW );
    p3d::pddi->SetProjectionMode( PDDI_PROJECTION_ORTHOGRAPHIC );

    p3d::stack->Push();
    p3d::stack->LoadIdentity();

    float textPosX, textPosY;
    textPosX = textPosY = 0.0f;

    tPointCamera* pPtCam = (tPointCamera*)GetSuperCamManager()->GetSCC(0)->GetCamera();

    p3d::stack->Translate( textPosX, textPosY, pPtCam->GetNearPlane()+0.5f);
    float scaleSize = 1.0f / 480.0f;  //This is likely good for 528 also.

    float fontScale = 0.5f * mTextScale;
    p3d::stack->Scale(scaleSize * fontScale, scaleSize * fontScale , 1.0f);

    switch ( mRenderState )
    {
    case CAR_DATA:
        {
            DisplayCarData();

            if ( mCountDownToo )
            {
                DisplayCountDown();
            }
            break;
        }
    case PLAYER_DATA:
        {
//            DisplayPlayerData();
            break;
        }
    case COUNT_DOWN:
        {
            DisplayCountDown();
            break;
        }
    case HIGH_SCORES:
        {
//            DisplayHighScores();
            break;
        }
    default:
        {
            break;
        }
    }

    p3d::stack->Pop();
    p3d::pddi->PopState( PDDI_STATE_VIEW );
}

//=============================================================================
// SuperSprintDrawable::SetRenderState
//=============================================================================
// Description: Comment
//
// Parameters:  ( RenderState state )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintDrawable::SetRenderState( RenderState state )
{
    mRenderState = state;
}
#endif //SUPERSPRINTDRAWABLE_H
