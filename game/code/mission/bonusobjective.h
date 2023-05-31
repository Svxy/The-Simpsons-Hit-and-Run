//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bonusobjective.h
//
// Description: Blahblahblah
//
// History:     12/5/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BONUSOBJECTIVE_H
#define BONUSOBJECTIVE_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BonusObjective
{
public:
    enum Type { NO_DAMAGE, NO_CHASE_COLLISIONS, TIME, POSITION, HIT_N };

    BonusObjective() : mType( NO_DAMAGE ) {};
    virtual ~BonusObjective() {};

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual unsigned int GetNumericData() = 0;  //All bonus objective have numeric data in common...

    void Reset();
    void Start();
    void Update( unsigned int milliseconds );

    Type GetType() const;
    bool GetSuccessful() const;
        
protected:
    virtual void OnReset() {};
    virtual void OnStart() {};
    virtual void OnUpdate( unsigned int milliseconds ) {};

    void SetType( Type type );
    void SetSuccessful( bool successful );
    bool GetStarted() const;

private:
    Type mType;
    bool mSuccessful;
    bool mStarted;

        //Prevent wasteful constructor creation.
    BonusObjective( const BonusObjective& bonusobjective );
    BonusObjective& operator=( const BonusObjective& bonusobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// BonusObjective::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void BonusObjective::Reset()
{
    mStarted = false;
    OnReset();
}

//=============================================================================
// BonusObjective::Start
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void BonusObjective::Start()
{
    mStarted = true;
    OnStart();
}

//=============================================================================
// BonusObjective::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
inline void BonusObjective::Update( unsigned int milliseconds )
{
    OnUpdate( milliseconds );
}

//=============================================================================
// BonusObjective::Type GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BonusObjective
//
//=============================================================================
inline BonusObjective::Type BonusObjective::GetType() const
{
    return mType;
}

//=============================================================================
// BonusObjective::GetSuccessful
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool BonusObjective::GetSuccessful() const
{
    return mSuccessful;
}


//*****************************************************************************
//
//Inline Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// BonusObjective::SetType
//=============================================================================
// Description: Comment
//
// Parameters:  ( Type type )
//
// Return:      void 
//
//=============================================================================
inline void BonusObjective::SetType( Type type )
{
    mType = type;
}

//=============================================================================
// BonusObjective::SetSuccessful
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool successful )
//
// Return:      void 
//
//=============================================================================
inline void BonusObjective::SetSuccessful( bool successful )
{
    mSuccessful = successful;
}

//=============================================================================
// BonusObjective::GetStarted
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool BonusObjective::GetStarted() const
{
    return mStarted;
}

#endif //BONUSOBJECTIVE_H
