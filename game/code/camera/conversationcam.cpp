//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ConversationCam.cpp
//
// Description: Implement ConversationCam
//
// History:     24/04/2002 + Created -- Cary Brisebois, based from the Bumper Camera
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <algorithm>
#include <meta/locator.h>
#include <p3d/utility.hpp>
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <radmath/trig.hpp>
#include <stdlib.h>

//========================================
// Project Includes
//========================================
#include <camera/ConversationCam.h>
#include <camera/isupercamtarget.h>
#include <meta/triggervolumetracker.h>
#include <presentation/presentation.h>
#include <worldsim/character/character.h>

//========================================
// Definitions
//========================================
static unsigned int FindConversationCam( const tName& name );
static void InitializeCameraNames();
static void CleanupCameraNames();
int ConversationCam::sInstCount = 0;


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
static float       sDistance  = 5.0f;
static float       sAngle     = 0.0f;
static float       sElevation = 2.0f;
static float       sFov       = rmt::PI / 180.0f * 50.00f;
static float       sXOffset   = 0.0f;
static float       sYOffset   = 1.3f;
static float       sHeightAdjustment = 0.0f;
static float       sXOffsetPC        = 0.0f;
static float       sXOffsetNPC       = 0.0f;
const float        childHeightAdjustment = -0.4f;
static rmt::Vector  sPcOffset( 0.0f, 0.0f, 0.0f );
static rmt::Vector  sNpcOffset( 0.0f, 0.0f, 0.0f );

static const unsigned int sMaxCameras = 8;
static float sDistances [ sMaxCameras ] = { 5.00f, 5.00f, 6.20f,  6.20f };
static float sElevations[ sMaxCameras ] = { 2.11f, 2.11f, 2.20f,  2.20f };
static float sXOffsets  [ sMaxCameras ] = { 0.00f, 0.00f, 1.06f, -1.06f };
static float sYOffsets  [ sMaxCameras ] = { 1.03f, 1.03f, 1.25f,  1.25f };
static bool  sPcFocus   [ sMaxCameras ] = { false, false, false, false };
static float sFovs      [ sMaxCameras ] = 
{ 
    1.00f,
    1.00f,
    0.49f,
    0.49f,
};
static float sAngles    [ sMaxCameras ] = 
{ 
    0.55f,
   -0.55f,
    1.40f,
   -1.40f,
};
static rmt::Vector sPcOffsets[ sMaxCameras ] = 
{
    rmt::Vector( 0.00f, 0.00f,  0.00f ),
    rmt::Vector( 0.00f, 0.00f,  0.00f ),
    rmt::Vector( 0.00f, 0.00f,  0.00f ),
    rmt::Vector( 0.00f, 0.00f,  0.00f )
};
static rmt::Vector sNpcOffsets[ sMaxCameras ] = 
{
    rmt::Vector(  0.00f, 0.00f,  0.00f ),
    rmt::Vector(  0.00f, 0.00f,  0.00f ),
    rmt::Vector( -1.80f, 0.00f, -0.21f ),
    rmt::Vector(  0.00f, 0.00f, -0.00f )
};
static tName* sCamNames;
static unsigned int sCamPc  = 999;      //these numbers are overridden by the mission
static unsigned int sCamNpc = 999;
static bool sNpcIsChild;
static bool sPcIsChild;
static tName bestSideLocatorName;
static Locator* bestSideLocator = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ConversationCam::ConversationCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ConversationCam::ConversationCam() :
    mPosition( 0.0f, 0.0f, 0.0f ),
    mTarget  ( 0.0f, 0.0f, 0.0f ),
    mTargetHeight( 1.0f ),
    mPositionHeight( 1.5 )
{
    if( sCamNames == NULL )
    {
        InitializeCameraNames();
    }

    //Only have 2 targets.
    mTargets[ 0 ] = NULL;
    mTargets[ 1 ] = NULL;

    sInstCount++;
}

//==============================================================================
// ConversationCam::~ConversationCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ConversationCam::~ConversationCam()
{
    sInstCount--;
    if (sInstCount < 1)
    {
        CleanupCameraNames ();
    }
    
    bestSideLocatorName.SetText( NULL );
    bestSideLocator = NULL;
}

//=============================================================================
// ConversationCam::Update
//=============================================================================
// Description: called to animate the camera
//
// Parameters:  ( unsigned int milliseconds
//
// Return:      void 
//
//=============================================================================
void ConversationCam::Update( unsigned int milliseconds)
{
    rAssertMsg( mTargets[ 0 ], "The ConversationCam needs a target!" );
    rAssertMsg( mTargets[ 1 ], "The ConversationCam needs a secondary target!" );

    {
        rmt::Vector pos;
        rmt::Vector candidates[ 2 ];
        rmt::Vector targPos[ 2 ];

        targPos[ 0 ] = mPositionOriginal;
        targPos[ 1 ] = mTargetOriginal;

        //
        // Make characters face one another
        //
        GetPresentationManager()->MakeCharactersFaceEachOther( mCharacters[ 0 ], mCharacters[ 1 ] );

        //
        // Update the characters' position 
        //
        rmt::Vector averagePosition = ( mPositionOriginal + mTargetOriginal ) / 2;
        rmt::Vector offset = mPositionOriginal - mTargetOriginal;
        rmt::Vector pos1 = mPositionOriginal;
        rmt::Vector pos2 = mTargetOriginal;
        float distanceApart = offset.Magnitude();
        offset.Normalize();
        const float minimumSeparation = 1.65f;
        if( distanceApart < minimumSeparation )
        {
            float amountToCorrectDistance = minimumSeparation - distanceApart;
            pos1 += offset * amountToCorrectDistance / 2;
            pos2 -= offset * amountToCorrectDistance / 2;
        }
        rmt::Vector rightvector = offset;
        const rmt::Vector y = rmt::Vector( 0.0f, 1.0f, 0.0f );
        rightvector.CrossProduct( y );

        //
        // figure out if we should invert the left and right to showcase a 
        // particular side of the conversation
        //
        bool swapSides = false;
        if( bestSideLocator != NULL )
        {
            rmt::Vector bestSide;
            bestSideLocator->GetPosition( &bestSide );
            rmt::Vector leftSide = pos1 - rightvector;
            rmt::Vector rightSide = pos1 + rightvector;
            float leftDistance = ( bestSide - leftSide ).MagnitudeSqr();
            float rightDistance = ( bestSide - rightSide ).MagnitudeSqr();
            if( leftDistance > rightDistance )
            {
                swapSides = true;
                rightvector = -rightvector;
            }
        }

        pos1 += sPcOffset.x * offset + sPcOffset.y * y + sPcOffset.z * rightvector;
        pos2 += sNpcOffset.x * offset + sNpcOffset.y * y + sNpcOffset.z * rightvector;
        float rotation = choreo::GetWorldAngle( offset.x, offset.z );
        mCharacters[ 1 ]->RelocateAndReset( pos2, rotation, false );
        offset.Scale( -1.0f );
        rotation = choreo::GetWorldAngle( offset.x, offset.z );
        mCharacters[ 0 ]->RelocateAndReset( pos1, rotation, false );


        

        rmt::Vector towardsNPC = targPos[ 1 ] - targPos[ 0 ];
        rmt::Vector t0Tot1 = towardsNPC;

        rmt::Vector normals[2];

        normals[ 0 ] = t0Tot1;
        normals[ 1 ] = t0Tot1;

        float normalScale = 5.0f;

        normals[ 0 ].CrossProduct( rmt::Vector( 0.0f, 1.0f, 0.0f ) );
        normals[ 0 ].Normalize();
        normals[ 0 ].Scale( normalScale );
        normals[ 1 ].CrossProduct( rmt::Vector( 0.0f, -1.0f, 0.0f ) );
        normals[ 1 ].Normalize();
        normals[ 1 ].Scale( normalScale );

        rmt::Vector& right = normals[ 0 ];
        if( swapSides )
        {
            right *= -1.0f;
        }

        t0Tot1.Scale( 0.5f );

        candidates[ 0 ].Add( targPos[ 0 ], t0Tot1 );
        candidates[ 1 ].Add( targPos[ 0 ], t0Tot1 );

        candidates[ 0 ].Add( normals[ 0 ] );
        candidates[ 1 ].Add( normals[ 1 ] );

        //Should test to make sure that one of the candidates is not passing into a wall, 
        //find the closest for now.
        rmt::Vector camPos;
        GetPosition( &camPos );

        rmt::Vector test[2];
        test[ 0 ].Sub( candidates[ 0 ], camPos );
        test[ 1 ].Sub( candidates[ 1 ], camPos );

        if ( test[ 0 ].MagnitudeSqr() < test[ 1 ].MagnitudeSqr() )
        {
            mPosition = candidates[ 0 ];
        }
        else
        {
            mPosition = candidates[ 1 ];
        }

        float theta = sAngle;
        towardsNPC.Normalize();
        right.Normalize();
        mPosition = towardsNPC * rmt::Sin( theta ) + right * rmt::Cos( theta );
        mPosition *= sDistance;
        mPosition += ( targPos[ 0 ] + targPos[ 1 ] ) / 2.0f;
        mPosition.y += sElevation + sHeightAdjustment; //adjust height
        mTarget   =  ( targPos[ 0 ] + targPos[ 1 ] ) / 2.0f;
        mTarget.y += sYOffset + sHeightAdjustment;
        mTarget   += towardsNPC * sXOffset;     //adjust the x position for the target
        SetFlag( (Flag)FIRST_TIME, false );

        //Reset the FOV.
        SetFOV( sFov );
    }

    //---------  Buid a rod for the camera

    SetCameraValues( milliseconds, mPosition, mTarget);
}

//=============================================================================
// ConversationCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void ConversationCam::LoadSettings( unsigned char* settings )
{
}

//=============================================================================
// ConversationCam::LockCharacterPositions
//=============================================================================
// Description: Grabs the positionss of the characters and records them for 
//              later use
//
// Parameters:  NONE
//
// Return:      NONE 
//
//=============================================================================
void ConversationCam::LockCharacterPositions()
{
    //
    // First reset the characters to their minimum separating distance
    //
    mCharacters[ 0 ]->GetPosition( &mPositionOriginal );
    mCharacters[ 1 ]->GetPosition( &mTargetOriginal );
}

//=============================================================================
// ConversationCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetTarget( ISuperCamTarget* target )
{
    mTargets[ 0 ] = target;
}

//=============================================================================
// ConversationCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void ConversationCam::AddTarget( ISuperCamTarget* target )
{
    mTargets[ 1 ] = target;
}

//=============================================================================
// ConversationCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
unsigned int ConversationCam::GetNumTargets() const
{
    unsigned int count = 0;

    if ( mTargets[ 0 ] )
    {
        count++;
    }

    if ( mTargets[ 1 ] )
    {
        count++;
    }

    return count;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// ConversationCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ConversationCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Conversation", GetPlayerID() );

    radDbgWatchAddFloat( &sDistance,  "Distance from center", nameSpace, NULL, NULL, 0.0f, 20.0f );
    radDbgWatchAddFloat( &sAngle,     "Angle", nameSpace, NULL, NULL, -rmt::PI_2, rmt::PI_2 );
    radDbgWatchAddFloat( &sElevation, "Elevation", nameSpace, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &sFov,       "FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &sXOffset,   "XOffset", nameSpace, NULL, NULL, -5.0f, 5.0f );
    radDbgWatchAddFloat( &sYOffset,   "YOffset", nameSpace, NULL, NULL, 0.0f, 5.0f );  
    radDbgWatchAddVector( reinterpret_cast< float* >( &sPcOffset ), "PcOffset", nameSpace, NULL, NULL, -5.0f, 5.0f );
    radDbgWatchAddVector( reinterpret_cast< float* >( &sNpcOffset ), "NpcOffset", nameSpace, NULL, NULL, -5.0f, 5.0f );
#endif
}

//=============================================================================
// ConversationCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ConversationCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &sDistance );
    radDbgWatchDelete( &sAngle );
    radDbgWatchDelete( &sFov );
    radDbgWatchDelete( &sElevation );
    radDbgWatchDelete( &sXOffset );
    radDbgWatchDelete( &sYOffset );
    radDbgWatchDelete( &sPcOffset );
    radDbgWatchDelete( &sNpcOffset );
#endif
}

//=============================================================================
// InitializeCameraNames
//=============================================================================
// Description: sets up the names of the conversation cameras for scripting
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
static void InitializeCameraNames()
{
    sCamNames = new tName[ sMaxCameras ];
    int count = -1;
    sCamNames[ ++count ] = "pc_far";
    sCamNames[ ++count ] = "npc_far";
    sCamNames[ ++count ] = "pc_near";
    sCamNames[ ++count ] = "npc_near";
}

//=============================================================================
// CleanupCameraNames
//=============================================================================
// Description: Cleans up the allocations made in InitializeCameraNames
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
static void CleanupCameraNames()
{
    if (sCamNames)
    {
        delete[] sCamNames;
        sCamNames = 0;
    }
}

//=============================================================================
// FindConversationCam
//=============================================================================
// Description: finds the index of a specific conversation camera by name
//
// Parameters:  name - the name of the camera that we're looking for
//
// Return:      void 
//
//=============================================================================
static unsigned int FindConversationCam( const tName& name )
{
    if( sCamNames == NULL )
    {
        InitializeCameraNames();
    }
    const tName* foundname = std::find( &sCamNames[ 0 ], &sCamNames[ sMaxCameras ], name );
    const unsigned int offset = reinterpret_cast< unsigned int >( foundname ) - reinterpret_cast< unsigned int >( &sCamNames[ 0 ] );
    const unsigned int index = offset / sizeof( tName );
#ifdef RAD_DEBUG
    if( index >= sMaxCameras )
    {
        rDebugPrintf( "Could Not Find conversation camera %s\n", name.GetText() );
    }
#endif
    rAssertMsg( index < sMaxCameras, "Conversation camera could not be found" );
    return index;
}

//=============================================================================
// SetCameraByIndex
//=============================================================================
// Description: finds the index of a specific conversation camera by name
//
// Parameters:  name - the name of the camera that we're looking for
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCameraByIndex( const unsigned int index )
{
    sDistance  = sDistances  [ index ];
    sAngle     = sAngles     [ index ];
    sElevation = sElevations [ index ];
    sFov       = sFovs       [ index ];
    sXOffset   = sXOffsets   [ index ];
    sYOffset   = sYOffsets   [ index ];
    sPcOffset  = sPcOffsets  [ index ];
    sNpcOffset = sNpcOffsets [ index ];
    if( sPcFocus[ index ] == true )
    {
        if( sPcIsChild )
        {
            sHeightAdjustment = childHeightAdjustment;
        }
        else
        {
            sHeightAdjustment = 0.0f;
        }
    }
    if( sPcFocus[ index ] == false )
    {
        if( sNpcIsChild )
        {
            sHeightAdjustment = childHeightAdjustment;
        }
        else
        {
            sHeightAdjustment = 0.0f;
        }
    }
}

//=============================================================================
// SetCameraByName
//=============================================================================
// Description: finds the index of a specific conversation camera by name
//
// Parameters:  name - the name of the camera that we're looking for
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCameraByName( const tName& name )
{
    unsigned int index = FindConversationCam( name );
    SetCameraByIndex( index );
}
//=============================================================================
// SetPcCameraByName
//=============================================================================
// Description: sets the camera used when the PC is talking
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCameraDistanceByName( const tName& name, const float distance )
{
    const unsigned int index = FindConversationCam( name );
    SetCameraDistanceByIndex( index, distance );
}

//=============================================================================
// SetCamBestSide
//=============================================================================
// Description: gets the locator from the name, and uses its location to 
//
// Parameters:  index - the index of the camera we're using
//              distance - the distance to the focus point
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCamBestSide( const tName& name )
{
    bestSideLocatorName = name;
    bestSideLocator = p3d::find< Locator >( bestSideLocatorName.GetUID() );
}

//=============================================================================
// SetCameraDistanceByIndex
//=============================================================================
// Description: sets the distance from the camera to the focus point by camera
//              index
//
// Parameters:  index - the index of the camera we're using
//              distance - the distance to the focus point
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCameraDistanceByIndex( const unsigned int index, const float distance )
{
    rAssert( index < sMaxCameras );
    sDistances[ index ] = distance;
}

//=============================================================================
// ConversationCam::SetCharacter
//=============================================================================
// Description: sets the actual characters we're going to look at
//
// Parameters:  index - 0,1 is this the player or the NPC
//              character - pointer to the character
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetCharacter( const int index, Character* character )
{
    rAssert( index < CONV_CAM_MAX_CHARACTERS );
    mCharacters[ index ] = character;
}

//=============================================================================
// SetPcCameraByName
//=============================================================================
// Description: sets the camera used when the PC is talking
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetPcCameraByName( const tName& name )
{
    unsigned int index = FindConversationCam( name );
    sCamPc = index;
}
//=============================================================================
// SetNpcCamerByName
//=============================================================================
// Description: sets the camera used when the NPC is talking
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetNpcCameraByName( const tName& name )
{
    unsigned int index = FindConversationCam( name );
    sCamNpc = index;
}
//=============================================================================
// SetNpcIsChild
//=============================================================================
// Description: tells the converstaion camera that its target is a child
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetNpcIsChild( const bool isChild )
{
        sNpcIsChild = isChild;
}
//=============================================================================
// ConversationCam::SetPcIsChild
//=============================================================================
// Description: tells the converstaion camera that its target is a child
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::SetPcIsChild( const bool isChild )
{
    sPcIsChild = isChild;
}
//=============================================================================
// UsePcCam
//=============================================================================
// Description: sets the camera used when the NPC is talking
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::UsePcCam()
{
    SetCameraByIndex( sCamPc );
}
//=============================================================================
// SetNpcCamerByName
//=============================================================================
// Description: sets the camera used when the NPC is talking
//
// Parameters:  name - the name of the camera that we're using
//
// Return:      void 
//
//=============================================================================
void ConversationCam::UseNpcCam()
{
    SetCameraByIndex( sCamNpc );
}

//=============================================================================
// OnInit
//=============================================================================
// Description: gets called when we're transitioning from another camera type
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void ConversationCam::OnInit()
{
    TriggerVolumeTracker::GetInstance()->IgnoreTriggers( true );
}

//=============================================================================
// OnShutdown
//=============================================================================
// Description: gets called when we're transitioning to another camera type
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void ConversationCam::OnShutdown()
{
    TriggerVolumeTracker::GetInstance()->IgnoreTriggers( false );
    bestSideLocatorName = "";
}
