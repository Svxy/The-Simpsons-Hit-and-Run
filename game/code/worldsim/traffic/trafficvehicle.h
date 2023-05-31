//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficvehicle.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Added members that aid Traffic management -- Dusit Eakkachaichanvet
//              04/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef TRAFFICVEHICLE_H
#define TRAFFICVEHICLE_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================
class Vehicle;
class Lane;


//=============================================================================
//
// TrafficVehicle
//
//=============================================================================

class TrafficVehicle
{
public:
    TrafficVehicle();
    virtual ~TrafficVehicle();

    Vehicle*    GetVehicle() const;
    void        SetVehicle( Vehicle* vehicle );

    Lane*       GetLane() const;
    void        SetLane( Lane* lane );

    bool        GetIsActive() const;
    void        SetIsActive( bool active );

    bool        HasHusk() const;
    void        SetHasHusk( bool yes );

    Vehicle*    GetHusk();
    void        SetHusk( Vehicle* husk );

    //int         mActiveListIndex;
    unsigned int mMillisecondsDeactivated;
    bool        mCanBeResurrected;

    unsigned int mMillisecondsOutOfSight;
    bool        mOutOfSight;

    

private:

    Vehicle* mVehicle;
    Vehicle* mHusk;

    bool mIsActive;      // Active = being used as traffic car
    Lane* mLane;         // Pointer to Lane to whose list of vehicles this
                         //  traffic vehicle has been added.
    bool mHasHusk;

private:
    //Prevent wasteful constructor creation.
    TrafficVehicle( const TrafficVehicle& trafficvehicle );
    TrafficVehicle& operator=( const TrafficVehicle& trafficvehicle );
};
inline TrafficVehicle::TrafficVehicle() :
    //mActiveListIndex( -1 ),
    mMillisecondsDeactivated( 0 ),
    mCanBeResurrected( true ),
    mMillisecondsOutOfSight( 0 ),
    mOutOfSight( true ),
    mVehicle( NULL ),
    mHusk( NULL ),
    mIsActive( false ),
    mLane( NULL ),
    mHasHusk( false )
{
}
inline TrafficVehicle::~TrafficVehicle()
{
}
inline Vehicle* TrafficVehicle::GetVehicle() const
{
    return mVehicle;
}
inline void TrafficVehicle::SetVehicle( Vehicle* vehicle )
{
    mVehicle = vehicle;
}
inline Lane* TrafficVehicle::GetLane() const
{
    return mLane;
}
inline void TrafficVehicle::SetLane( Lane* lane )
{
    mLane = lane;
}
inline bool TrafficVehicle::GetIsActive() const
{
    return mIsActive;
}
inline void TrafficVehicle::SetIsActive( bool active )
{
    mIsActive = active;
}
inline bool TrafficVehicle::HasHusk() const
{
    return mHasHusk;
}
inline void TrafficVehicle::SetHasHusk( bool yes )
{
    mHasHusk = yes;
}
inline Vehicle* TrafficVehicle::GetHusk()
{
    return mHusk;
}
inline void TrafficVehicle::SetHusk( Vehicle* husk )
{
    mHusk = husk;
}

#endif //TRAFFICVEHICLE_H
