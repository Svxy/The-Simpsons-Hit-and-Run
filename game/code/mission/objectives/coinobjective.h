//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        CoinObjective.h
//
// Description: Used for Race Missions that require coins to enter
//
// History:     Mar. 27. 2003 + Created -- Chuck C.
//
//=============================================================================

#ifndef COINOBJECTIVE_H
#define COINOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>
#include <events/eventdata.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Coin Objective
//
//=============================================================================

class CoinObjective : public MissionObjective
{
public:
   

    CoinObjective();
    virtual ~CoinObjective();

    virtual void HandleEvent( EventEnum id, void* pEventData );
    void SetCoinFee(int coins);
    void SetFeeCollected();
    int GetCoinAmount();
    bool PayCoinFee();
    //these two functions are soo hacky they are a work around for missionstages resetting for Coin Objectives
    bool Proceed();
    void SetProceed();
    
   
protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate(unsigned int elapsedTime);
    virtual void Update(unsigned int elaspedTime);
private:
    bool mbFeeCollected;
    int mCoinFee;
    bool mHack;
    //Prevent wasteful constructor creation.
    CoinObjective( const CoinObjective& CoinObjective );
    CoinObjective& operator=( const CoinObjective& CoinObjective );
};


#endif //COINOBJECTIVE_H
