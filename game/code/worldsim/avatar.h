//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        avatar.h
//
// Description: Avatar Class declaration.
//
// History:     4/3/2002 + Created -- TBJ
//
//=============================================================================

#ifndef AVATAR_H
#define AVATAR_H

//========================================
// Nested Includes
//========================================

#include <radmath/radmath.hpp>
#include <cheats/cheatinputsystem.h>
#include <roads/roadmanager.h>

//========================================
// Forward References
//========================================
class Character;
class Vehicle;
class VehicleController;
struct AvatarInputManager;
class tCamera;

//=============================================================================
//
// Synopsis:    The Avatar
//
//=============================================================================

class Avatar : public ICheatEnteredCallback
{
// METHODS:
public:
    Avatar();
    ~Avatar();

    void Destroy( void );

    int GetControllerId( void ) const;
    void SetControllerId ( int id );

    void SetPlayerId( int id ) { mPlayerId = id; }
    int GetPlayerId() const { return mPlayerId; }

    Character* GetCharacter( void ) const;
    void SetCharacter( Character* pCharacter );

    Vehicle* GetVehicle( void ) const;
    void SetVehicle( Vehicle* pVehicle );

    void GetIntoVehicleStart( Vehicle* pVehicle );
    void GetIntoVehicleEnd( Vehicle* pVehicle );
    void GetOutOfVehicleStart( Vehicle* pVehicle );
    void GetOutOfVehicleEnd( Vehicle* pVehicle );
    
    void SetInCarController( void );
    void SetOutOfCarController( void );
    void SetCameraTargetToCharacter( bool cut = false );
    void SetCameraTargetToVehicle( bool cut = false );
    
    void SetCamera( tCamera* pCamera );

    const void GetPosition( rmt::Vector& pos ) const;
    const void GetHeading( rmt::Vector& irHeading ) const;

    // *** //
    const void GetNormalizedHeadingSafe( rmt::Vector& heading ) const;
    const void GetVelocity( rmt::Vector& vel ) const;
    const float GetSpeedMps() const;
    // *** //

    bool IsInCar( void ) const;

    void PreSubstepUpdate();
    void Update(float dt);
    void PreCollisionPrep();

    void OnCheatEntered( eCheatID cheatID, bool isEnabled );

    void GetLastPathInfo( 
        RoadManager::PathElement& elem, 
        RoadSegment*& seg,
        float& segT,
        float& roadT );

    void GetRaceInfo(
        float& distToCurrCollectible,
        int& currCollectible,
        int& numLapsCompleted );

    void SetRaceInfo(
        float distToCurrCollectible,
        int currCollectible,
        int numLapsCompleted );

//MEMBERS
public:


private:

    ///////////////// PATH INFO /////////////////////
    bool mHasBeenUpdatedThisFrame;
    RoadManager::PathElement mLastPathElement;
    RoadSegment* mLastRoadSegment;
    float mLastRoadSegmentT;
    float mLastRoadT;

    ///////////////// RACE DATA ///////////////////
    // we keep pieces of the race data in this class
    // because catch-up logic will need to use them
    float mDistToCurrentCollectible;
    int mCurrentCollectible;
    int mNumLapsCompleted;


    //Prevent wasteful constructor creation.
    //
	Avatar( const Avatar& avatar );
    Avatar& operator=( const Avatar& avatar );

    // Data.
    //
    static const int INVALID_CONTROLLER = -1;
    int mControllerId;

    int mPlayerId;

    Character* mpCharacter;

    // NULL if not in any vehicle, when IsInCar, this is set to that car
    Vehicle* mpVehicle; 

    AvatarInputManager* mpAvatarInputManager;

    static bool s_displayCoordinates;

    /*
    // TRUE if neither directly on a NON-shortcut road segment nor 
    // in an intersection
    bool mOffRoad; 

    // number of seconds mOffRoad has been true or false
    float mSecondsOffOrOnRoad;
    */
};



#endif //AVATAR_H
