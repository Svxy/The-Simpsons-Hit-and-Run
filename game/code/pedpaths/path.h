//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        path.h
//
// Description: Defines Path class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================


#ifndef PATH_H
#define PATH_H

//#include <pedpaths/pathsegment.h>
#include <roads/geometry.h>

class PathSegment;
class Pedestrian;


class Path
{
//MEMBERS
public:

    enum
    {
        MAX_PEDESTRIANS = 3 // number of peds to allow on a single path
    };


//METHODS
public:
    Path();
    Path( bool isClosed, int nSegments );
    ~Path();


    PathSegment* GetPathSegmentByIndex( int index );
    void AllocateSegments( int nSegments );


    
    //ACCESSORS
    bool IsFull() const;
    bool IsClosed() const;
    void SetIsClosed( bool isClosed );
    int GetNumPathSegments() const;

    bool AddPedestrian();
    bool RemovePedestrian();

//MEMBERS
private:
    bool mIsClosed;
    int mNumPathSegments;
    int mNumPeds;

    // pointer to dynamically allocated PathSegment array
    PathSegment** mPathSegments;


//METHODS:
private:


};


// ********************************* INLINES *******************************


inline bool Path::IsClosed() const
{
    return mIsClosed;
}

inline void Path::SetIsClosed( bool isClosed )
{
    mIsClosed = isClosed;
}

inline int Path::GetNumPathSegments() const
{
    return mNumPathSegments;
}


#endif //PATH_H