//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        collectibleobjective.h
//
// Description: Blahblahblah
//
// History:     10/8/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef COLLECTIBLEOBJECTIVE_H
#define COLLECTIBLEOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <mission/objectives/missionobjective.h>

//========================================
// Forward References
//========================================
class Locator;
class AnimatedIcon;
class Vehicle;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CollectibleObjective : public MissionObjective
{
public:
    CollectibleObjective();
	virtual ~CollectibleObjective();

    enum { MAX_COLLECTIBLES = 32 };

    unsigned int GetNumCollectibles() const;
    unsigned int GetNumCollected() const;

    const Locator* GetCollectibleLocator( unsigned int num );
    void MoveCollectible( unsigned int num, const rmt::Vector& newPos );

    void AddCollectibleLocatorName( char* locatorname, char* p3dname, radKey32 dialogName, tUID speakerName, float scale );
    void SetCollectEffectName( char* name );

    void AllowUserDump();

    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    void OnUpdate( unsigned int elapsedTime );
    virtual void OnInitialize();
    void OnFinalize();
    
    virtual void OnInitCollectibles() = 0;
    virtual void OnInitCollectibleObjective() = 0;
    virtual void OnFinalizeCollectibleObjective() = 0;
    virtual bool OnCollection( unsigned int collectibleNum, bool &shouldReset ) = 0;
    virtual void OnUpdateCollectibleObjective( unsigned int elapsedTimeMilliseconds ) {};

    virtual bool CheckCollectibleLocators( Locator* locator );
    void Activate( unsigned int index, bool bIsActive, bool primary, HudMapIcon::eIconType icon = HudMapIcon::ICON_COLLECTIBLE, bool render = true );
    void Collect( unsigned int index, bool shouldReset );
    bool IsCollected( unsigned int index );
    void SetFocus( unsigned int index );
    void ChangeIcon( unsigned int index, HudMapIcon::eIconType type );

    void ResetCollectibles();

    void DumpCollectible( int collectibleNum, Vehicle* dumper, Vehicle* hitter = NULL, bool useIntersectionList = true, bool terminal = false );
    
    bool IsUserDumpAllowed() const;
    int GetAnyCollectedID() const;

    void Uncollect( int collectibleNum );

    void GetCollectiblePathInfo( unsigned int index, RoadManager::PathElement& elem, float& roadT );

    unsigned int mNumCollectibles;
    unsigned int mNumCollected;

	struct CollectibleLocatorData
	{
        CollectibleLocatorData() : 
            pLocator( NULL ), 
            mAnimatedIcon( NULL ), 
            iHUDIndex( -1 ), 
            bTriggered( false ),
            fScaleFactor( 1.0f ),
            seg( NULL ),
            segT( 0.0f ),
            roadT( 0.0f ),
            mDialogName( 0 )
        { 
            locatorName[0] = '\0'; 
            p3dname[0]='\0'; 
            mArrowPath.mPathRoute.Allocate( RoadManager::GetInstance()->GetNumRoads() ); 
            elem.elem = NULL;
        };

		char locatorName[ 32 ];
		char p3dname[ 32 ];

		Locator* pLocator;
		AnimatedIcon* mAnimatedIcon;

		int iHUDIndex;

		bool bTriggered;
        float fScaleFactor;

        PathStruct mArrowPath;

        // the current path info upon which my locator is located
        RoadManager::PathElement elem; 
        RoadSegment* seg;
        float segT;
        float roadT;

        //
        // Key for the name of the line of dialog we're going to play
        // when this thing is collected, or 0 if unused.
        //
        radKey32 mDialogName;
        tUID mSpeakerName;
	};

    CollectibleLocatorData mCollectibles[ MAX_COLLECTIBLES ];

private:
    
    char mEffectName[32];
    AnimatedIcon* mCollectEffect;

    bool mAllowUserDump;
    
    unsigned int mCurrentFocus;


    //Prevent wasteful constructor creation.
	CollectibleObjective( const CollectibleObjective& collectibleobjective );
	CollectibleObjective& operator=( const CollectibleObjective& collectibleobjective );
};

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// CollectibleObjective::GetNumCollectibles
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned int
//
//=============================================================================
inline unsigned int CollectibleObjective::GetNumCollectibles() const
{ 
    return mNumCollectibles;
}

//=============================================================================
// CollectibleObjective::GetNumCollected
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int CollectibleObjective::GetNumCollected() const
{
    return mNumCollected;
}

//=============================================================================
// CollectibleObjective::GetCollectibleLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num )
//
// Return:      inline 
//
//=============================================================================
inline const Locator* CollectibleObjective::GetCollectibleLocator( unsigned int num )
{
    rAssert( num < MAX_COLLECTIBLES );
    return mCollectibles[ num ].pLocator;
}

//=============================================================================
// CollectibleObjective::AllowUserDump
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void CollectibleObjective::AllowUserDump()
{
    mAllowUserDump = true;
}


//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// CollectibleObjective::IsCollected
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      bool 
//
//=============================================================================
inline bool CollectibleObjective::IsCollected( unsigned int index )
{
    rAssert( index < mNumCollectibles );

    return mCollectibles[ index ].bTriggered;
}

//=============================================================================
// CollectibleObjective::IsUserDumpAllowed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool CollectibleObjective::IsUserDumpAllowed() const
{
    return mAllowUserDump;
}

#endif //COLLECTIBLEOBJECTIVE_H
