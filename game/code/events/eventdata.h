//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventdata.h
//
// Description: Define any custom data structures that are passed
//              with event triggers here.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef EVENTDATA_H
#define EVENTDATA_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>
#include <p3d/anim/pose.hpp>

//========================================
// Forward References
//========================================
class Character;
class AnimCollisionEntityDSG;

namespace ActionButton
{
    class AnimSwitch;
};

namespace sim
{
    class Collision;
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_INTERIOR_LOADED event.
//
//==============================================================================
struct InteriorLoadedEventData
{
    tName interiorName;
    tName sectionName;
    bool first;
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_CONVERSATION_INIT event.
//
//==============================================================================
struct DialogEventData
{
    DialogEventData() : char1( NULL ), char2( NULL ), charUID1( 0 ), charUID2( 0 ), dialogName( 0 ) {};

    Character* char1;
    Character* char2;
    tUID charUID1;
    tUID charUID2;
    radKey32 dialogName;
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_START_ANIMATION_SOUND event.
//
//==============================================================================
struct AnimSoundData
{
    AnimSoundData( const char* sound, const char* posnSettings ) :
        soundName( sound ), animJoint( NULL ), soundObject( NULL ), positionalSettingName( posnSettings ) {};

    const char* soundName;
    tPose::Joint* animJoint;
    ActionButton::AnimSwitch* soundObject;
    const char* positionalSettingName;

    private:
        AnimSoundData();
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_START_ANIM_ENTITY_DSG_SOUND event.
//
//==============================================================================
struct AnimSoundDSGData
{
    AnimSoundDSGData( const char* sound, AnimCollisionEntityDSG* object, tPose::Joint* joint, const char* settingName ) : 
        soundName( sound ), animJoint( joint ), soundObject( object ), positionalSettingName( settingName ) {};

    const char* soundName;
    tPose::Joint* animJoint;
    AnimCollisionEntityDSG* soundObject;
    const char* positionalSettingName;

private:
    AnimSoundDSGData();
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_CHANGE_MUSIC_STATE event.
//
//==============================================================================
struct MusicStateData
{
    MusicStateData() : stateKey( 0 ), stateEventKey( 0 ) {};
    MusicStateData( radKey32 state, radKey32 event ) : stateKey( state ), stateEventKey( event ) {};

    radKey32 stateKey;
    radKey32 stateEventKey;
};

//==============================================================================
//
// Synopsis: Associated data for EVENT_CAMERA_SHAKE events.
//
//==============================================================================
struct ShakeEventData
{
    ShakeEventData() : playerID( -1 ), force( 0.0f ), looping( false ) { direction.Set(0.0f, 0.0f, 0.0f ); };

    int playerID;
    rmt::Vector direction;
    float force;
    bool looping;
};

class Vehicle;
struct CarOnCarCollisionEventData
{
    CarOnCarCollisionEventData() : vehicle( NULL ), force ( 0.0f ), collision( NULL ) {};
    
    Vehicle* vehicle;
    float force;
    sim::Collision* collision;
};

struct RumbleCollision
{
    RumbleCollision() : vehicle( NULL ), normalizedForce( 0.0f ) { point.Set( 0.0f, 0.0f, 0.0f ); };
    Vehicle* vehicle;
    float normalizedForce;
    rmt::Vector point;
};

#endif // EVENTDATA_H