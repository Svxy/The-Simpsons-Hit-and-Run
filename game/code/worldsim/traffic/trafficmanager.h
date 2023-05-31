//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficmanager.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Spawning/Removing vehicles -- Dusit Eakkachaichanvet
//              04/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRAFFICMANAGER_H
#define TRAFFICMANAGER_H

//========================================
// Nested Includes
//========================================
#include <worldsim/traffic/trafficvehicle.h>
#include <worldsim/traffic/trafficmodelgroup.h>
#include <ai/vehicle/trafficai.h>
#include <events/eventlistener.h>
#include <roads/intersection.h>

#include <render/culling/swaparray.h>

//========================================
// Forward References
//========================================
class Vehicle;
class Lane;
class Character;


//********************************************
// USED TO CONTROL WHETHER OR NOT TRAFFIC CARS GET ADDED TO THE WORLD
// FOR THE PURPOSES OF TESTING.
//
const bool DISABLETRAFFIC = false;
//
//********************************************


struct ITrafficSpawnController 
{
    virtual void SetMaxTraffic( int n ) = 0;
    virtual void EnableTraffic() = 0;
    virtual void DisableTraffic() = 0;
    virtual void ClearTrafficInSphere( const rmt::Sphere& s ) = 0;
};

/*
These are the constraints Traffic lives by.

Road Segment
============
- Try not to make angles of the edge normals deviate too greatly from 
  the segment's "direction". i.e Avoid this:
           
            |
          / |
        /   |
      /     |
    /       |
  /         |
 |          |
 |__________|

- a single road segment is a flat polygon (of coplanar vertices).
- road world builder data must conform to the art/physics.
- road segments should end BEFORE they cross over the (art-side) 
  crosswalks. Otherwise traffic cars will run over people crossing the street.
- no road segment is overlapping another
- the road segments that belong to the same road must lie within half a meter 
  apart from one another to be considered attached to that road.


Intersections
=========
- plane of the intersection (formed by in & out road segments) must be 
  FLAT FLAT FLAT... This means completely X-Z horizontal.

- For each intersection "x", for every IN road "y" belonging to "x", if "y" 
  has more than 1 OUT road ("z"?) other than a U-turn OUT road going back 
  the way it came ("w"?), then it needs to be type N_WAY (type value of 1 
  for the world builders). Otherwise its type is NO_STOP (type value 0). This 
  goes for all intersections, including the "fake" ones we use to join 
  Zones/Rails.

- The road data for IN & OUT segments of "fake" intersections (used to 
  join zones/rails) need to leave a small (1 meter or less) "gap" between 
  one another. Otherwise the traffic cars will appear to "flip". More 
  generally: no 2 roads going in or out of any intersection are 
  touching/overlapping one another.

- the centre of any intersection (hand-placed) is more or less at the 
  physical centre of the intersection. 

- There is an appreciable/reasonable distance between one intersection and 
  another. This translates to something like 10 meters or more.

General
======
- The world is not round or overlapping. In other words, if I were to extend 
  an infinite line, parallel to y-axis down through the world, it will only 
  cut through only one single road segment or intersection plane. This is a 
  precautionary step only. Not sure what drastic effects failing to conform 
  to this constraint will produce, but I imagine that most of the code
  assumes we are in 2.5D, not 3D.



Might have missed some.. but there you are.
*/


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TrafficManager :
    ITrafficSpawnController,
    public EventListener    
{
public:

    static TrafficManager* mInstance;

    static const float FADE_RADIUS;
    static const float ADD_RADIUS;
    static const float CENTER_OFFSET;
    static const float INITIAL_ADD_RADIUS;
    static const float REMOVE_RADIUS;
    static const unsigned int MILLISECONDS_BETWEEN_REMOVE;
    static const unsigned int MILLISECONDS_BETWEEN_ADD;
    static const unsigned int MILLISECONDS_POPULATE_WORLD;

    enum { 
        MAX_CHARS_TO_STOP_FOR = 10 // Max # characters traffic will brake for
    };

    static ITrafficSpawnController* GetSpawnController();
    static TrafficManager* GetInstance();
    static void DestroyInstance();

    ///////////////////////////////////
    // EVENTLISTENTER STUFF
    virtual void HandleEvent( EventEnum id, void* pEventData );
    ///////////////////////////////////

    void Init();
    void InitDefaultModelGroups();
    void Update( unsigned int milliseconds ); 

    void Cleanup();

    void AddCharacterToStopFor( Character* character );
    void RemoveCharacterToStopFor( Character* character );
    int GetNumCharsToStopFor();
    Character* GetCharacterToStopFor( int i ) const;

    // Given its pointer, find it in our static list of TrafficVehicles and
    // take it out of lane traffic and out of active list. Also invalidate 
    // TrafficVehicle-specific fields.
    void RemoveTraffic( Vehicle* vehicle ); //Take this out of traffic.
    void Deactivate( Vehicle* v );

    void SetMaxTraffic( int n );
    int GetMaxTraffic();

    void EnableTraffic();
    void DisableTraffic();
    void ClearTrafficInSphere( const rmt::Sphere& s );
    void ClearTrafficOutsideSphere( const rmt::Sphere& s );

    void ClearOutOfSightTraffic();

    TrafficModelGroup* GetTrafficModelGroup( int i );
    void SetCurrTrafficModelGroup( int i );

    float GetDesiredTrafficSpeed();
    void GenerateRandomColour( pddiColour& colour );
    void SwapInTrafficHusk( Vehicle* vehicle ); // Remove traffic vehicle, swap in a husk...

    bool IsVehicleTrafficVehicle( Vehicle* vehicle );

//MEMBERS
private:
    float mDesiredTrafficSpeedKph;

    enum {
        MAX_TRAFFIC_MODEL_GROUPS = 10, // Max # different groups of traffic models 
        MAX_INTERSECTIONS = 5, // Max # intersections we'll be keeping track of (calling Update to)
        NUM_SWATCH_COLOURS = 25, // # artist-defined swatch colours
        MAX_QUEUED_TRAFFIC_HORNS = 3
    };
    int mMaxTraffic; // value is betw 0 and MAX_TRAFFIC
    int mNumTraffic; // value changes between 0 and MAX_TRAFFIC as vehicles are added/removed

    // keep repository of all the vehicles we'll ever need.
    TrafficVehicle* mVehicles;

    Character* mCharactersToStopFor[ MAX_CHARS_TO_STOP_FOR ];
    int mNumCharsToStopFor;

    // keep list of intersections that will need to be updated in this loop
    Intersection* mpIntersections[ MAX_INTERSECTIONS ]; 

    unsigned int mMillisecondsBetweenRemove;
    unsigned int mMillisecondsBetweenAdd;
    unsigned int mMillisecondsPopulateWorld;

    bool mTrafficEnabled;

    TrafficModelGroup mTrafficModelGroups[ MAX_TRAFFIC_MODEL_GROUPS ];
    int mCurrTrafficModelGroup;

    struct SwatchColour
    {
        int red;
        int green;
        int blue;
    };
    static SwatchColour sSwatchColours[ NUM_SWATCH_COLOURS ];

    struct TrafficHornQueue
    {
        Vehicle* vehicle;
        unsigned int delayInMilliseconds;
    };
    SwapArray<TrafficHornQueue> mQueuedTrafficHorns;
    int mNumQueuedTrafficHorns;

//METHODS
private:

    float DetermineDesiredSpeedKph();

    // Add a traffic vehicle to the lane and to active list and Initialize
    // some TrafficVehicle-specific fields
    bool AddTraffic( Lane* lane, TrafficVehicle* tv );

    // Given its index in our static list of TrafficVehicles, take vehicle out 
    // of lane traffic and out of active list. Also invalidate TrafficVehicle-
    // specific fields.
    void RemoveTraffic( int vIndex );  
    
    void RemoveTrafficVehicle( TrafficVehicle* tv );

    TrafficVehicle* FindTrafficVehicle( Vehicle* vehicle );
    TrafficVehicle* GetFreeTrafficVehicle();
    Vehicle* InitRandomVehicle();
    Vehicle* InitRandomHusk( Vehicle* v );


    // update the intersections to let cars go in turn (if NWAY)
    void UpdateIntersection( 
        unsigned int milliseconds, 
        Vehicle* v, 
        int& nIntersectionsUpdated ); 

    void SetVehicleFadeAlpha( Vehicle* v, const rmt::Vector& pPos );

    bool AttemptResurrection( TrafficVehicle* tv );

    void UpdateQueuedTrafficHorns( unsigned int milliseconds );

    // constructors/destructors we wish to hide so we can implement singleton
    TrafficManager();
    virtual ~TrafficManager();

    //Prevent wasteful constructor creation.
    TrafficManager( const TrafficManager& trafficmanager );
    TrafficManager& operator=( const TrafficManager& trafficmanager );
};




// ************************************ INLINES *******************************************


inline int TrafficManager::GetNumCharsToStopFor()
{
    return mNumCharsToStopFor;
}
inline Character* TrafficManager::GetCharacterToStopFor( int i ) const
{
    rAssert( 0 <= i && i < MAX_CHARS_TO_STOP_FOR );
    return mCharactersToStopFor[i];
}
inline TrafficModelGroup* TrafficManager::GetTrafficModelGroup( int i )
{
    rTuneAssert( 0 <= i && i < MAX_TRAFFIC_MODEL_GROUPS );
    return &mTrafficModelGroups[i];
}
inline void TrafficManager::SetCurrTrafficModelGroup( int i )
{
    rTuneAssert( 0 <= i && i < MAX_TRAFFIC_MODEL_GROUPS );
    mCurrTrafficModelGroup = i;
}
inline float TrafficManager::GetDesiredTrafficSpeed()
{
    return mDesiredTrafficSpeedKph * KPH_2_MPS;
}
#endif //TRAFFICMANAGER_H




