//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        raceobjective.h
//
// Description: Blahblahblah
//
// History:     23/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef RACEOBJECTIVE_H
#define RACEOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/collectibleobjective.h>

//========================================
// Forward References
//========================================
class Locator;
class Vehicle;
class WaypointAI;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RaceObjective : public CollectibleObjective
{
public:
    RaceObjective();
    virtual ~RaceObjective();

    void SetNumLaps( int numLaps );
    void SetGambleRace(bool boolean);
    bool QueryIsGambleRace();
    void SetParTime(int seconds);
    int GetParTime();

protected:
    virtual void OnInitialize();
    virtual void OnInitCollectibles();
    virtual void OnInitCollectibleObjective();
    virtual void OnFinalizeCollectibleObjective();
    virtual bool OnCollection( unsigned int collectibleNum, bool &shouldReset );
    virtual void OnUpdateCollectibleObjective( unsigned int elapsedTimeMilliseconds );
    
private:
    enum { MAX_RACECARS = 4 };

    unsigned int mNextCollectible;
    unsigned int mNumAIVehicles;

    struct RaceVehicle
    {
        RaceVehicle() : raceCar( NULL ), raceCarAI( NULL ), mAnimatedIcon( NULL ) {};
        Vehicle* raceCar;
        WaypointAI* raceCarAI;
        AnimatedIcon* mAnimatedIcon;
    };
    RaceVehicle mAIRaceCars[ MAX_RACECARS ];

    unsigned int mMyPosition;
    
    int mNumLaps;
    int mNumLapsCompleted;
   

    //Chuck: for Gambling Races
    bool mIsGambleRace;
    int mParTime;               //used for GamlbeRaces as the time to beat
    
    
    bool mIsTimeTrial;

    RoadManager::PathElement mPlayerElem;
    RoadSegment* mPlayerSeg;
    float mPlayerSegT;
    float mPlayerRoadT;

    AnimatedIcon* mFinishLine;
    AnimatedIcon* mFinishLineEffect;
    bool mFinishActive;

    void CalculatePosition();

    //Prevent wasteful constructor creation.
    RaceObjective( const RaceObjective& raceobjective );
    RaceObjective& operator=( const RaceObjective& raceobjective );
};

//=============================================================================
// RaceObjective::SetNumLaps
//=============================================================================
// Description: Comment
//
// Parameters:  ( int numLaps )
//
// Return:      void 
//
//=============================================================================
inline void RaceObjective::SetNumLaps( int numLaps )
{
    mNumLaps = numLaps;
}

#endif //RACEOBJECTIVE_H
