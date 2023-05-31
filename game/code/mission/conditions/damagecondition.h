//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        damagecondition.h
//
// Description: Blahblahblah
//
// History:     24/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef DAMAGECONDITION_H
#define DAMAGECONDITION_H

//========================================
// Nested Includes
//========================================

#include <mission/conditions/vehiclecondition.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DamageCondition : public VehicleCondition
{
public:
    DamageCondition();
    virtual ~DamageCondition();

    void SetMinValue( float value ) { mMinValue = value; }
    float GetMinValue() { return mMinValue; }

    virtual void HandleEvent( EventEnum id, void* pEventData );

    bool IsClose();

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:

    //Prevent wasteful constructor creation.
    DamageCondition( const DamageCondition& damagecondition );
    DamageCondition& operator=( const DamageCondition& damagecondition );

    float mMinValue;

    float mLastVehicleDamage;
    bool mbCarDestroyed;
};


#endif //DAMAGECONDITION_H
