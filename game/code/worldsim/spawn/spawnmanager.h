//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        spawnmanager.h
//
// Description: SpawnManager Class Declaration
//
// History:     11/5/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef SPAWNMANAGER_H
#define SPAWNMANAGER_H

#include <radmath/radmath.hpp>
#include <raddebug.hpp>

class SpawnManager
{
public:

    SpawnManager();
    virtual ~SpawnManager();
    void Update( float seconds ); // don't allow overwriting of Update, which enforces structure

    // PURE VIRTUALS
    virtual void Init();
    virtual void ClearAllObjects() = 0;
    virtual void ClearObjectsInsideRadius( rmt::Vector center, float radius ) = 0;
    virtual void ClearObjectsOutsideRadius( rmt::Vector center, float radius ) = 0;
    virtual int GetAbsoluteMaxObjects() const = 0;
    virtual int GetMaxObjects() const = 0;
    virtual void SetMaxObjects( int maxObjects ) = 0;
    virtual int GetMaxModels() const = 0;
    virtual int GetNumRegisteredModels() const = 0;
    virtual bool RegisterModel( const char* name, int spawnFreq ) = 0;
    virtual bool UnregisterModel( const char* name ) = 0;

    // ACCESSORS
    bool IsActive() const;

    // turning on will make use of current enable flags (all default to true)
    // turning off will not doing anything (no adding, removing, updating whatsoever)
    void SetActive( bool activeFlag ); 

    void EnableAdd( bool enable );
    void EnableRemove( bool enable );
    void EnableUpdate( bool enable );

    float GetSpawnRadius() const;
    void SetSpawnRadius( float radius );
    float GetRemoveRadius() const;
    void SetRemoveRadius( float radius );

    int GetNumObjects() const;


protected:

    // PURE VIRTUALS
    // return # objects added/removed
    virtual void AddObjects( float seconds ) = 0;
    virtual void RemoveObjects( float seconds ) = 0;
    virtual void UpdateObjects( float seconds ) = 0;

    // subclass accessors
    virtual float GetSecondsBetwAdds() const = 0;
    virtual float GetSecondsBetwRemoves() const = 0;
    virtual float GetSecondsBetwUpdates() const = 0;


    bool mIsActive;
    float mSpawnRadius; // in meters
    float mRemoveRadius; // in meters
    // Betw 0 and AbsoluteMax (defined by subclass)... 
    // NOTE: Not betw 0 and mMaxObjects because mMaxObjects can be changed
    //       to a lower number while there are mNumObjects > mMaxObjects 
    //       still within radius (they don't get removed till an explicit 
    //       call to ClearObjectsOutsideRadius takes place)
    int mNumObjects; 

    float mSecondsSinceLastAdd;
    float mSecondsSinceLastRemove;
    float mSecondsSinceLastUpdate;

    bool mAddEnabled;
    bool mRemoveEnabled;
    bool mUpdateEnabled;
private:

};

inline SpawnManager::SpawnManager() :
mIsActive( true ),
mSpawnRadius( -1.0f ),
mRemoveRadius( -1.0f ),
mNumObjects( 0 ),
mSecondsSinceLastAdd( 0.0f ),
mSecondsSinceLastRemove( 0.0f ),
mSecondsSinceLastUpdate( 0.0f ),
mAddEnabled( true ),
mRemoveEnabled( true ),
mUpdateEnabled( true )
{
}

inline SpawnManager::~SpawnManager()
{
}

inline bool SpawnManager::IsActive() const
{
    return mIsActive;
}
inline void SpawnManager::SetActive( bool activeFlag )
{
    mIsActive = activeFlag;
}
inline float SpawnManager::GetSpawnRadius() const
{
    return mSpawnRadius;
}
inline void SpawnManager::SetSpawnRadius( float radius )
{
    rAssert( radius >= 0.0f );
    mSpawnRadius = radius;
}
inline float SpawnManager::GetRemoveRadius() const
{
    return mRemoveRadius;
}
inline void SpawnManager::SetRemoveRadius( float radius )
{
    rAssert( radius >= 0.0f );
    mRemoveRadius = radius;
}
inline int SpawnManager::GetNumObjects() const
{
    return mNumObjects;
}
inline void SpawnManager::EnableAdd( bool enable )
{
    mAddEnabled = enable;
}
inline void SpawnManager::EnableRemove( bool enable )
{
    mRemoveEnabled = enable;
}
inline void SpawnManager::EnableUpdate( bool enable )
{
    mUpdateEnabled = enable;
}

#endif 
