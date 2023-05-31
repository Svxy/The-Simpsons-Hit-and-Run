//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        mouthflapper.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Created -- NAME
//
//=============================================================================

#ifndef MOUTHFLAPPER_H
#define MOUTHFLAPPER_H

//========================================
// Nested Includes
//========================================

#include <poser/joint.hpp>
#include <poser/posedriver.hpp>

//========================================
// Forward References
//========================================

class poser::Joint;
class poser::Pose;
class Character;


//=============================================================================
//
// Synopsis:    This class contains all the internal settings for mouth 
//              flapping
//
//=============================================================================
class MouthFlapperDefaultSetting
{
public:
    MouthFlapperDefaultSetting( const tName& name, const float minOpen, const float maxOpen, const float maxDeviation, const float minSpeed, const float maxSpeed );
    MouthFlapperDefaultSetting( const MouthFlapperDefaultSetting& right );
    MouthFlapperDefaultSetting& operator=( const MouthFlapperDefaultSetting& right );
    void         AddToWatcher();
    const float  GetMaxDeviation()   const;
    const float  GetMaxOpen()        const;
    const float  GetMaxSpeed()       const;
    const float  GetMinOpen()        const;
    const float  GetMinSpeed()       const;
    const tName& GetName()           const;
    void         RemoveFromWatcher();
protected:
private:
    tName name;
    float minOpen;
    float maxOpen;
    float maxDeviation;
    float minSpeed;
    float maxSpeed;
};

//=============================================================================
//
// Synopsis:    class for mouth flapping
//
//=============================================================================
class MouthFlapper : public poser::PoseDriver
{
public:
    MouthFlapper();
    virtual ~MouthFlapper();

#ifdef DEBUGWATCH
    static  void AddVariablesToWatcher();
#endif DEBUGWATCH
    virtual void Advance( float deltaTime );
    void         GetDefaultSettings( const tName& name );
    void         SetCharacter( Character* pCharacter );
    virtual void Update( poser::Pose* pose );

protected:
    static unsigned int GetNumberOfDefaultSettings();

private:

    //Prevent wasteful constructor creation.
    MouthFlapper( const MouthFlapper& mouthflapper );
    MouthFlapper& operator=( const MouthFlapper& mouthflapper );

    void NeuSpeed();

    int mJointIndex;
    poser::Joint* mJoint;
    Character*    mCharacter;

    float mCurrentdt;
    float mDirection;
    float mAngle;
    float mSpeed;
    float mMaxOpen;
    float mMinOpen;

    MouthFlapperDefaultSetting mSetting;
    bool  mGotDefaultSettings;
};


#endif //MOUTHFLAPPER_H
