//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        MouthFlapper.cpp
//
// Description: Implement MouthFlapper
//
// History:     09/09/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/trig.hpp>
#include <radtime.hpp>
#include <stdlib.h>
#include <p3d/anim/skeleton.hpp>

//========================================
// Project Includes
//========================================

#include <presentation/mouthflapper.h>

#include <worldsim/character/character.h>

#include <choreo/puppet.hpp>

#include <poser/pose.hpp>
#include <poser/joint.hpp>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static float MIN_OPEN         = -0.10f * rmt::PI;
static float MAX_OPEN         =  0.15f * rmt::PI;
static float MAX_DEVIATION    =  0.04f * rmt::PI;
static float MIN_SPEED        =  1.00f;
static float MAX_SPEED        =  4.00f;

//default settings
MouthFlapperDefaultSetting gDefaultSetting( "default", -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f );
MouthFlapperDefaultSetting gDefaultSettings[] = 
{
    MouthFlapperDefaultSetting( "apu",      -0.314159f, 0.490973f, 0.13f, 2.20f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "b_cbg",    -0.314159f, 0.559204f, 0.13f, 2.44f, 3.88f ),     //done  
    MouthFlapperDefaultSetting( "b_cletus", -0.314159f, 0.420973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "b_frink",  -0.314159f, 0.420973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "b_grandpa",-0.314159f, 0.420973f, 0.136640f, 2.20f, 3.56f ), //done
    MouthFlapperDefaultSetting("b_milhouse",-0.314159f, 0.521504f, 0.125664f, 2.22f, 4.00f ), //done
    MouthFlapperDefaultSetting( "b_nelson", -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "b_ralph",  -0.314159f, 0.471504f, 0.125664f, 2.22f, 4.00f ), //done
    MouthFlapperDefaultSetting( "b_skinner",-0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "b_smithers", -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "b_snake",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "b_zfem1",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "b_zmale1", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "b_zmale2", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "b_zmale3", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "brn_unf",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "barney",   -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "bart",     -0.314159f, 0.521504f, 0.125664f, 2.50f, 4.00f ), //done
    MouthFlapperDefaultSetting( "burns",    -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "captain",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "carl",     -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "cbg",      -0.314159f, 0.559204f, 0.13f, 2.44f, 3.88f ),     //done  
    MouthFlapperDefaultSetting( "cletus",   -0.314159f, 0.420973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "frink",    -0.314159f, 0.420973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "gil",      -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "grandpa",  -0.314159f, 0.420973f, 0.136640f, 2.20f, 3.56f ), //done
    MouthFlapperDefaultSetting( "hibbert",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "homer",    -0.314159f, 0.420973f, 0.136640f, 2.00f, 3.44f ), //done
    MouthFlapperDefaultSetting( "jimbo",    -0.314159f, 0.470973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "joger1",   -0.314159f, 0.470973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "kearney",  -0.314159f, 0.521504f, 0.125664f, 2.34f, 3.78f ), //done
    MouthFlapperDefaultSetting( "krusty",   -0.314159f, 0.420973f, 0.136640f, 2.20f, 3.56f ), //done
    MouthFlapperDefaultSetting( "lenny",    -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "lisa",     -0.314159f, 0.471239f, 0.125664f, 2.5f, 4.00f ),  //done
    MouthFlapperDefaultSetting( "louie",    -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "marge",    -0.314159f, 0.420973f, 0.13f, 1.66f, 3.44f ),     //done
    MouthFlapperDefaultSetting( "milhouse", -0.314159f, 0.521504f, 0.125664f, 2.22f, 4.00f ), //done
    MouthFlapperDefaultSetting( "moe",      -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "moleman",  -0.314159f, 0.452389f, 0.125664f, 1.56f, 3.54f ), //done
    MouthFlapperDefaultSetting( "ned",      -0.314159f, 0.490088f, 0.13f, 1.78f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "nelson",   -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "nriviera", -0.314159f, 0.470973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "otto",     -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "patty",    -0.314159f, 0.420973f, 0.136640f, 2.20f, 3.56f ), //done
    MouthFlapperDefaultSetting( "ped0",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped1",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped2",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped3",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped4",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped5",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ped6",     -0.314159f, 0.452389f, 0.13f, 1.66f, 3.44f ),
    MouthFlapperDefaultSetting( "ralph",    -0.314159f, 0.471504f, 0.125664f, 2.22f, 4.00f ), //done
    MouthFlapperDefaultSetting( "reward_barney", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "reward_homer",  -0.314159f, 0.420973f, 0.136640f, 2.00f, 3.44f ), //done
    MouthFlapperDefaultSetting( "reward_kearney",-0.314159f, 0.521504f, 0.125664f, 2.34f, 3.78f ), //done
    MouthFlapperDefaultSetting( "reward_otto",   -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),
    MouthFlapperDefaultSetting( "reward_willie", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "selma",    -0.314159f, 0.420973f, 0.136640f, 2.20f, 3.56f ), //done
    MouthFlapperDefaultSetting( "skinner",  -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "smithers", -0.314159f, 0.490088f, 0.13f, 2.00f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "snake",    -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "teen",     -0.314159f, 0.470973f, 0.13f, 1.88f, 3.78f ),     //done
    MouthFlapperDefaultSetting( "wiggum",   -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),     //done
    MouthFlapperDefaultSetting( "willie",   -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "zfem1",  -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "zmale1", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "zmale2", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
    MouthFlapperDefaultSetting( "zmale3", -0.314159f, 0.559204f, 0.13f, 2.56f, 4.12f ),
};

char gWatcherNMnameSpace[ 256 ] = "Presentation\\MouthFlapping\\";    

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MouthFlapper::MouthFlapper
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MouthFlapper::MouthFlapper() :
    mJointIndex( -1 ),
    mJoint( NULL ),
    mCharacter( NULL ),
    mCurrentdt( 0.0f ),
    mDirection( -1.0f ),
    mAngle( 0.0f ),
    mSpeed( 0.0f ),
    mMaxOpen( MAX_OPEN ),
    mMinOpen( MIN_OPEN ),
    mSetting( gDefaultSetting ),
    mGotDefaultSettings( false )
{
}

//==============================================================================
// MouthFlapper::~MouthFlapper
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MouthFlapper::~MouthFlapper()
{
    mCharacter = NULL;
}

//=============================================================================
// MouthFlapper::AddVariablesToWatcher
//=============================================================================
// Description: Adds all the global variables controlling mouth flapping to the
//              watcher so that they can be tuned
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
#ifdef DEBUGWATCH
void MouthFlapper::AddVariablesToWatcher()
{
    int size = GetNumberOfDefaultSettings();
    int i;
    for( i = 0; i < size; ++i )
    {
        gDefaultSettings[ i ].AddToWatcher();
    }
}
#endif //DEBUGWATCH

//=============================================================================
// MouthFlapper::GetDefaultSettings
//=============================================================================
// Description: Comment
//
// Parameters:  name - the name of the character who's mouth is supposed to 
//              flap
//
// Return:      void 
//
//=============================================================================
void MouthFlapper::GetDefaultSettings( const tName& name )
{
    int size = GetNumberOfDefaultSettings();
    int i;
    for( i = 0; i < size; ++i )
    {
        MouthFlapperDefaultSetting& setting = gDefaultSettings[ i ];
        tName settingName = setting.GetName();
        if( settingName == name )
        {
            //
            // Assign the settings
            //
            mSetting = setting;
            return;
        }
    }
    rAssertMsg( false, "A character for whom we have no mouth flapping parameters is talking - please tell Ian which level and mission this was" );
}

//=============================================================================
// MouthFlapper::GetNumberOfDefaultSettings
//=============================================================================
// Description: returns the nubmer of characters for which default parameters
//              have been hardcoded
//
// Parameters:  none
//
// Return:      unsigned int - how many parameters are there 
//
//=============================================================================
unsigned int MouthFlapper::GetNumberOfDefaultSettings()
{
    size_t totalSize =  sizeof( gDefaultSettings );
    size_t sizeOfEach = sizeof( MouthFlapperDefaultSetting );
    return totalSize / sizeOfEach;
}

//=============================================================================
// MouthFlapper::SetCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* guy )
//
// Return:      void 
//
//=============================================================================
void MouthFlapper::SetCharacter( Character* pCharacter )
{
    mCharacter = pCharacter;
    if( pCharacter != NULL )
    {
        choreo::Puppet* pPuppet = pCharacter->GetPuppet( );

        poser::Pose* pPose = pPuppet->GetPose();
        tSkeleton* skeleton = pPose->GetSkeleton();
        mJointIndex = skeleton->FindJointIndex( "Jaw" );

        mJoint = pPose->GetJoint( mJointIndex );
    }
    else
    {
        mJointIndex = -1;
        mJoint = NULL;
    }

    NeuSpeed();
}

//=============================================================================
// MouthFlapper::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedtime )
//
// Return:      void 
//
//=============================================================================
void MouthFlapper::Advance( float deltaTime )
{
    mCurrentdt += deltaTime;
}

//=============================================================================
// MouthFlapper::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( Pose* pose )
//
// Return:      void 
//
//=============================================================================
void MouthFlapper::Update( poser::Pose* pose )
{
    //
    // make sure that the default settings are properly applied
    //
    rAssert( mCharacter != NULL );
    const tName& name = mCharacter->GetNameObject();
    GetDefaultSettings( name );

    if( mCurrentdt <= 0.0f )
    {
        return;
    }

    rAssert( mJointIndex >= 0 );
    poser::Joint* joint = pose->GetJoint(mJointIndex);
    poser::Transform pt = joint->GetObjectTransform();
    rmt::Matrix m = pt.GetMatrix();
    rmt::Matrix r;
    r.Identity();
    float dAngle = mSpeed * mCurrentdt * mDirection;
    mAngle += dAngle;
    if( mAngle > mMaxOpen )
    {
        mDirection = -mDirection;
        mAngle = mMaxOpen;
        NeuSpeed();
    }

    if( mAngle < mMinOpen )
    {
        mDirection = -mDirection;
        mAngle = mMinOpen;
        NeuSpeed();
    }

    r.FillRotateZ( mAngle );
    m.Mult( r );    
    pt.SetMatrix(m);
    joint->SetObjectTransform(pt);
    mCurrentdt = 0.0f;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MouthFlapper::NeuSpeed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MouthFlapper::NeuSpeed()
{
    float defaultMaxDeviation = mSetting.GetMaxDeviation();
    float defaultMaxSpeed     = mSetting.GetMaxSpeed();
    float defaultMinSpeed     = mSetting.GetMinSpeed();
    float defaultMaxOpen      = mSetting.GetMaxOpen();
    float defaultMinOpen      = mSetting.GetMinOpen();
    mSpeed   = defaultMinSpeed + ( defaultMaxSpeed - defaultMinSpeed ) * rand() / RAND_MAX;
    mMaxOpen = defaultMaxOpen - defaultMaxDeviation * rand() / RAND_MAX;
    mMinOpen = defaultMinOpen + defaultMaxDeviation * rand() / RAND_MAX;
}

//=============================================================================
// MouthFlapperDefaultSetting::MouthFlapperDefaultSetting
//=============================================================================
// Description: constructor
//
// Parameters:  N/A
//
// Return:      N/A
//
//=============================================================================
MouthFlapperDefaultSetting::MouthFlapperDefaultSetting( 
    const tName& name, 
    const float minOpen, 
    const float maxOpen, 
    const float maxDeviation,
    const float minSpeed,
    const float maxSpeed ) :
    name( name ),
    minOpen( minOpen ),
    maxOpen( maxOpen ),
    maxDeviation( maxDeviation ),
    minSpeed( minSpeed ),
    maxSpeed( maxSpeed )
{
}

//=============================================================================
// MouthFlapperDefaultSetting::MouthFlapperDefaultSetting
//=============================================================================
// Description: copy constructor
//
// Parameters:  N/A
//
// Return:      N/A
//
//=============================================================================
MouthFlapperDefaultSetting::MouthFlapperDefaultSetting( const MouthFlapperDefaultSetting& right )
{
    *this = right;
}

//=============================================================================
// MouthFlapperDefaultSetting::AddToWatcher
//=============================================================================
// Description: adds this set of mouth flapper parameters to the watcher
//
// Parameters:  NONE
//
// Return:      NONE
//
//=============================================================================
void MouthFlapperDefaultSetting::AddToWatcher()
{
    char nameSpace[ 256 ] = "";
    const char* nameString = name.GetText();
    sprintf( nameSpace , "%s%s", gWatcherNMnameSpace, nameString );
    radDbgWatchAddFloat( &minOpen,      "Min Open",      nameSpace, NULL, NULL, -rmt::PI, rmt::PI );
    radDbgWatchAddFloat( &maxOpen,      "Max Open",      nameSpace, NULL, NULL, -rmt::PI, rmt::PI );
    radDbgWatchAddFloat( &maxDeviation, "Max Deviation", nameSpace, NULL, NULL, 0.0f, 20.0f );
    radDbgWatchAddFloat( &minSpeed,     "Min Speed",     nameSpace, NULL, NULL, 0.0f, 20.0f );
    radDbgWatchAddFloat( &maxSpeed,     "Max Speed",     nameSpace, NULL, NULL, 0.0f, 20.0f );
}

//=============================================================================
// MouthFlapperDefaultSetting::GetMaxDeviation
//=============================================================================
// Description: allows access to the maximum deviation
//
// Parameters:  NONE
//
// Return:      the maximum deviation parameter
//
//=============================================================================
const float MouthFlapperDefaultSetting::GetMaxDeviation() const
{
    return maxDeviation;
}

//=============================================================================
// MouthFlapperDefaultSetting::GetMaxOpen
//=============================================================================
// Description: allows access to the maxopen parameters
//
// Parameters:  NONE
//
// Return:      the max open parameter
//
//=============================================================================
const float MouthFlapperDefaultSetting::GetMaxOpen() const
{
    return maxOpen;
}

//=============================================================================
// MouthFlapperDefaultSetting::GetMaxSpeed
//=============================================================================
// Description: GetMaxSpeed
//
// Parameters:  NONE
//
// Return:      the max speed parameter
//
//=============================================================================
const float MouthFlapperDefaultSetting::GetMaxSpeed() const
{
    return maxSpeed;
}

//=============================================================================
// MouthFlapperDefaultSetting::GetMinOpen
//=============================================================================
// Description: GetMinOpen
//
// Parameters:  NONE
//
// Return:      the min open parameter
//
//=============================================================================
const float MouthFlapperDefaultSetting::GetMinOpen() const
{
    return minOpen;
}

//=============================================================================
// MouthFlapperDefaultSetting::GetMinSpeed
//=============================================================================
// Description: GetMinSpeed
//
// Parameters:  NONE
//
// Return:      the minSpeed parameter
//
//=============================================================================
const float MouthFlapperDefaultSetting::GetMinSpeed() const
{
    return minSpeed;
}

//=============================================================================
// MouthFlapperDefaultSetting::GetName
//=============================================================================
// Description: GetName
//
// Parameters:  NONE
//
// Return:      the name of the object
//
//=============================================================================
const tName& MouthFlapperDefaultSetting::GetName() const
{
    return name;
}

//=============================================================================
// MouthFlapperDefaultSetting::operator=
//=============================================================================
// Description: assignment operator
//
// Parameters:  N/A
//
// Return:      N/A
//
//=============================================================================
MouthFlapperDefaultSetting& MouthFlapperDefaultSetting::operator=( const MouthFlapperDefaultSetting& right )
{
    if( this == &right )
    {
        return *this;
    }

    name         = right.name;
    minOpen      = right.minOpen;
    maxOpen      = right.maxOpen;
    maxDeviation = right.maxDeviation;
    minSpeed     = right.minSpeed;
    maxSpeed     = right.maxSpeed;
    return *this;
}

//=============================================================================
// MouthFlapperDefaultSetting::RemoveFromWatcher
//=============================================================================
// Description: removes this set of mouth flapper parameters to the watcher
//
// Parameters:  NONE
//
// Return:      NONE
//
//=============================================================================
void MouthFlapperDefaultSetting::RemoveFromWatcher()
{
    radDbgWatchDelete( &minOpen      );
    radDbgWatchDelete( &maxOpen      );
    radDbgWatchDelete( &maxDeviation );
    radDbgWatchDelete( &minSpeed     );
    radDbgWatchDelete( &maxSpeed     );
}
