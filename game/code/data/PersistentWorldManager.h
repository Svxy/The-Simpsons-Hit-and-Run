//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PersistentWorldManager
//
// Description: Manage the state of the world we want to keep during gameplay
//              and record to memory card.
//
// Authors:     James Harrison
//
// Revisions		Date			Author          Revision
//                  24 Feb          James Harrison  Created for SRR2
//
//===========================================================================

#ifndef PERSISTENT_WORLD_MANAGER_H
#define PERSISTENT_WORLD_MANAGER_H

class PersistentWorldManager
{
public:
    // Static Methods for accessing this singleton.
    static PersistentWorldManager* CreateInstance();
    static void DestroyInstance();
    static PersistentWorldManager* GetInstance();

    void OnSectorLoad( tUID Sector );
    void OnLevelLoad( int LevelNum );
    short GetPersistentObjectID( tUID Sector, tUID ObjectName = 0 );
    short GetPersistentObjectID( void ); // This is a level based object not associated with a particular sector.
    void OnObjectBreak( short ObjectID );

protected:
    PersistentWorldManager();
    ~PersistentWorldManager();
    PersistentWorldManager( const PersistentWorldManager& );
    PersistentWorldManager& operator= ( const PersistentWorldManager& );

    bool CheckObject( unsigned char SectorIndex, unsigned char ObjectIndex ) const;

    // Pointer to the one and only instance of this singleton.
    static PersistentWorldManager* spInstance;

    unsigned char mCurSectorLoad;    // Just to speed up the searches.
};

// Standard syntactic sugar for getting at this singleton.
inline PersistentWorldManager* GetPersistentWorldManager() { return PersistentWorldManager::GetInstance(); }

#endif //#ifndef PERSISTENT_WORLD_MANAGER_H