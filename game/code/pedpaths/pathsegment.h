//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathsegment.h
//
// Description: Defines PathSegment class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef PATHSEGMENT_H
#define PATHSEGMENT_H


#ifdef WORLD_BUILDER
#include "../render/DSG/IEntityDSG.h"
#else
#include <render/DSG/IEntityDSG.h>
#endif //WORLD_BUILDER

#include <radmath/radmath.hpp>
#include <raddebug.hpp>

#include <pedpaths/path.h>
//class Path;

// Hey, I'm just a glorified line segment!
class PathSegment : 
    public IEntityDSG
{

//MEMBERS
public:

//METHODS
public:

    PathSegment();
    PathSegment( Path* parent, int index, rmt::Vector start, rmt::Vector end );
    ~PathSegment();

    void Initialize( Path* parent, int index, rmt::Vector start, rmt::Vector end );

    void GetStartPos( rmt::Vector& pos );

    void GetEndPos( rmt::Vector& pos );

    Path* GetParentPath();
    void SetParentPath( Path* path );

    int GetIndexToParentPath() const;
    void SetIndexToParentPath( int index );

    /////////////////////////////////////////////////////////////////////////
    // IEntityDSG Interface
    /////////////////////////////////////////////////////////////////////////
    void DisplayBoundingBox(tColour colour = tColour(0,255,0));
    void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

    virtual void GetBoundingBox(rmt::Box3D* box);
    virtual void GetBoundingSphere(rmt::Sphere* sphere);
    void Display();

    rmt::Vector* pPosition();
    const rmt::Vector& rPosition();
    void GetPosition( rmt::Vector* ipPosn );
    /////////////////////////////////////////////////////////////////////////


//MEMBERS
private:

    rmt::Vector mStartPos;
    rmt::Vector mEndPos;

    Path* mParentPath;
    int mIndexToParentPath; 


    // Absolutely needed for IEntityDSG queries
    float mRadius;
    /*
    rmt::Vector mPosition; // center point of the path segment
    rmt::Sphere mBoundingSphere; // sphere around path segment
    rmt::Box3D mBoundingBox;
    */
    

//METHODS
private:
};




// ******************************* INLINES ******************************


inline void PathSegment::GetStartPos( rmt::Vector& pos )
{
    pos = mStartPos;
}

inline void PathSegment::GetEndPos( rmt::Vector& pos )
{
    pos = mEndPos;
}

inline Path* PathSegment::GetParentPath()
{
    return mParentPath;
}

inline void PathSegment::SetParentPath( Path* path )
{
    rAssert( path != NULL );
    mParentPath = path;
}

inline int PathSegment::GetIndexToParentPath() const
{
    return mIndexToParentPath;
}
inline void PathSegment::SetIndexToParentPath( int index )
{
    rAssert( mParentPath != NULL );
    rAssert( 0 <= index && index < mParentPath->GetNumPathSegments() );
    mIndexToParentPath = index;
}





#endif //PATHSEGMENT_H