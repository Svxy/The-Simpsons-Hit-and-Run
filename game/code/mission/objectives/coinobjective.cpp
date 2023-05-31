//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:       CoinObjective.cpp
//
// Description: ImplementCoinObjective
//
// History:     03/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================
#include <mission/objectives/coinobjective.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <events/eventmanager.h>


//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
//CoinObjective::CoinObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
CoinObjective::CoinObjective()
    
{
    mbFeeCollected = false;
    mCoinFee = 0;    
   
}

//=============================================================================
//CoinObjective::~CoinObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
CoinObjective::~CoinObjective()
{

}

//=============================================================================
//CoinObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void CoinObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch (id)
    {
    case EVENT_ATTEMPT_TO_ENTER_GAMBLERACE:
        {
            //check if player has enough tokens to enter race
            if (GetCoinManager()->GetBankValue() >= mCoinFee)
            {
                mbFeeCollected = true;
                GetEventManager()->TriggerEvent(EVENT_ENTER_GAMBLERACE_SUCCESS);
                int subtract_coins = 0;
                subtract_coins -= mCoinFee;
                GetCoinManager()->AdjustBankValue(subtract_coins);
                SetFinished(true);
            }
            else
            {
                GetEventManager()->TriggerEvent(EVENT_ENTER_GAMBLERACE_FAILURE);
                GetEventManager()->TriggerEvent(EVENT_TREE_OF_WOE_NEGATIVE_FEEDBACK);
                mbFeeCollected = false;
            }
            break;
        }
    default :
        {
            //Should never enter here
            rAssert(0);
        }
    }

    
    
}


//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
//CoinObjective::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CoinObjective::OnInitialize()
{
    GetEventManager()->AddListener(this,EVENT_ATTEMPT_TO_ENTER_GAMBLERACE);    
}

//=============================================================================
//CoinObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CoinObjective::OnFinalize()
{
    GetEventManager()->RemoveAll(this);
}


void CoinObjective::Update(unsigned int elaspedTime)
{
    OnUpdate(elaspedTime);
}


void CoinObjective::OnUpdate(unsigned int elaspedTime)
{
    if (mbFeeCollected == true)
    {
        SetFinished(true);
    }
    else
    {
        GetGameplayManager()->AbortCurrentMission();
    }
}


bool CoinObjective::PayCoinFee()
{  
   if ( GetCharacterSheetManager()->GetNumberOfTokens( GetGameplayManager()->GetCurrentLevelIndex()) > mCoinFee)
   {
       //subtract token from character sheet and set mbFinished to true
       return true;
   }
   else
   {
        return false;
   }
}


void CoinObjective::SetCoinFee(int coins)
{
    mCoinFee= coins;
}

int CoinObjective::GetCoinAmount()
{
    return mCoinFee;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
