//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedestrian.h
//
// Description: Defines Pedestrian class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef PEDESTRIAN_H                                                           // 80 character mark
#define PEDESTRIAN_H

#include <worldsim/character/charactercontroller.h>
#include <roads/geometry.h>

// because we're only storing pointers to these 
// objects (all pointers are same size), we can 
// use forward declarations here.
class Character;
class Path;
class PathSegment;


class Pedestrian 
: public NPCController
{
// METHODS
public:
    Pedestrian();
    virtual ~Pedestrian();

    void GetBoundingSphere( rmt::Sphere& s );

    /////////////////////////////////////////////////////////////////////////
    // CharacterController
    /////////////////////////////////////////////////////////////////////////
	virtual void Update( float seconds );
    virtual float GetValue( int buttonId ) const;
    virtual bool IsButtonDown( int buttonId ) const;
    /////////////////////////////////////////////////////////////////////////

    // a magic all-in-one call to place ped in the world & make it active
    // This only marks it for activation while waiting for CharaterManager
    // to load our data... won't put in world till model is done loading...
    void Activate(  
        Path* path, 
        PathSegment* pathSegment, 
        rmt::Vector spawnPos,
        const char* modelName );

    // This immediately removes model from the world and makes it inactive
    void Deactivate();

    // ACCESSORS
    bool GetIsActive() const;
    void SetIsActive( bool isActive );

    Path* GetPath();
    void SetPath( Path* path );

    void InitZero();

// MEMBERS
public:
    unsigned int mMillisecondsOutOfSight;

protected:
    float GetFollowPathSpeedMps() const;
    void OnReachedWaypoint();

// METHODS
private: 
    void DetermineFollowPathSpeed();
    void ActivateSelf();

    //Prevent wasteful constructor creation.
    Pedestrian( const Pedestrian& ped );
    Pedestrian& operator=( const Pedestrian& ped );

// MEMBERS
private:

    /////////////////////////////////////////////////////////////////////////
    // CharacterController
    /////////////////////////////////////////////////////////////////////////
    rmt::Vector mNormalizedDir;
    /////////////////////////////////////////////////////////////////////////

    bool mIsActive;

    Path* mPath;                    // path I'm on

    float mFollowPathSpeedMps;
    bool mMarkedForActivation;

    enum {
        MAX_STRING_LEN = 64
    };

    char mModelName [MAX_STRING_LEN+1];
    rmt::Vector mSpawnPos;
    float mSecondsTillActivateSelf;

};


// **************************** INLINES ******************************

inline bool Pedestrian::GetIsActive() const
{
    return mIsActive;
}
inline void Pedestrian::SetIsActive( bool isActive )
{
    mIsActive = isActive;
}
inline Path* Pedestrian::GetPath()
{
    return mPath;
}
inline void Pedestrian::SetPath( Path* path ) 
{
    mPath = path;
}

#endif // PEDESTRIAN_H