//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        getoutofcarcondition.h
//
// Description: Blahblahblah
//
// History:     4/7/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef GETOUTOFCARCONDITION_H
#define GETOUTOFCARCONDITION_H

//========================================
// Nested Includes
//========================================
#include <mission/conditions/missioncondition.h>

//========================================
// Forward References
//========================================
class Vehicle;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GetOutOfCarCondition : public MissionCondition
{
public:
    GetOutOfCarCondition();
    virtual ~GetOutOfCarCondition();

    // the Mission Stage should call this every frame
    virtual void Update( unsigned int elapsedTime );

    virtual bool IsClose();
    virtual void HandleEvent( EventEnum id, void* pEventData );
    virtual int   GetTimeRemainingTilFailuremilliseconds ();


    bool IsConditionActive( ) { return mbIsConditionActive; };
    void SetTime( unsigned int time ) { mTimeAmount = time + 1000; };
    void SetConditionActive();

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:

    int mTimeAmount;
    Vehicle* mMyVehicle;

    int mOldTime;


    bool mbIsConditionActive; //state of the condition false if user is in a car , true if they have left the car
    int mTimeRemainingmilliseconds; //amount of time remaining till user fails due to being out of the car  


    //Prevent wasteful constructor creation.
    GetOutOfCarCondition( const GetOutOfCarCondition& getoutofcarcondition );
    GetOutOfCarCondition& operator=( const GetOutOfCarCondition& getoutofcarcondition );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //GETOUTOFCARCONDITION_H
