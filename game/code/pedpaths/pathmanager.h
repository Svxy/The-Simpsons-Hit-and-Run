//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathmanager.h
//
// Description: Defines PathManager class
//
// History:     09/20/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================




#ifndef PATHMANAGER_H
#define PATHMANAGER_H

class Path;


class PathManager
{
//METHODS
public:
    //STATICS
    static PathManager* GetInstance();
    static void Destroy();

    //NONSTATICS
    Path* GetFreePath();

//MEMBERS
public:
    enum
    {
        MAX_PATHS = 125
    };




//METHODS
private:

    PathManager();
    virtual ~PathManager();

    void AllocatePaths( int nPaths );

    // These copy constructors are wasteful
    PathManager( const PathManager& pathmanager );
    PathManager& operator=( const PathManager& pathmanager );


//MEMBERS
private:

    //STATICS
    static PathManager* mInstance;

    //NONSTATICS
    Path* mPaths;             // static array of Path objects
    int mnPaths;              // total number of Path objects
    int mNextFreeIndex;       // next free path (initially 0)
};



// ***************************** INLINES *************************************


#endif // PATHMANAGER_H