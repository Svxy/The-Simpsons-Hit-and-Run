#ifndef ROADSEGMENT_H_
#define ROADSEGMENT_H_

#ifdef WORLD_BUILDER
#include "../render/DSG/IEntityDSG.h"
#elif !defined(TOOLS)
#include <render/DSG/IEntityDSG.h>
#else
#include <p3d/entity.hpp>
#define IEntityDSG tEntity
#endif

#include <string.h>
#include <radmath/radmath.hpp>

class Road;
class RoadSegmentData;


////////////////////////////////////////////////////////////////////////
// class RoadSegment adapted from GameGems article by Steven Ranck.  pp 412-420.
////////////////////////////////////////////////////////////////////////
// Concrete Base = 8 bytes.
//
class RoadSegment :
    public IEntityDSG
{
public:
	RoadSegment();
    virtual ~RoadSegment( void );

    void Init( RoadSegmentData* rsd, rmt::Matrix& hierarchy, float scaleAlongFacing );

    /////////////////////////////////////////////
    // ACCESSORS   
    /////////////////////////////////////////////
    void GetCorner( int index, rmt::Vector& out );
    void GetEdgeNormal( int index, rmt::Vector& out );
    void GetSegmentNormal( rmt::Vector& out );

    void SetSegmentIndex( unsigned int index );
    unsigned int GetSegmentIndex();

    Road* GetRoad();
    void SetRoad( Road* road );

    float GetSegmentLength();

    // for DEBUG
    const char* GetName();
    void SetName( const char* name );

    float GetLaneWidth();
    /////////////////////////////////////////////

    
  	float CalculateUnitDistIntoRoadSegment( float fPointX, float fPointZ );
    float CalculateUnitHeightInRoadSegment( float fPointX, float fPointZ );
    //float CalculateYHeight( float fPointX, float fPointZ );
    void GetLaneLocation( float t, int index, rmt::Vector& position, rmt::Vector& facing );



    unsigned int GetNumLanes( void ); // ask its parent road for the number of lanes
    float GetRoadWidth(); // number of lanes (obtained from parent Road) times lane-width

    //void GetJoinPoint( rmt::Vector& position, rmt::Vector& facing );
    void GetPosition( float t, float w, rmt::Vector* pos );
    float GetLaneLength( unsigned int lane );

    // Temp method to display.
    //
    void Render( const tColour& colour );
    void RenderAnywhere( const tColour& colour );


    //////////////////////////////////////////////////////////////////////////
    // IEntityDSG Interface
    //////////////////////////////////////////////////////////////////////////
    void DisplayBoundingBox(tColour colour = tColour(0,255,0));
    void DisplayBoundingSphere(tColour colour = tColour(0,255,0));
    void GetBoundingBox(rmt::Box3D* box);
    void GetBoundingSphere(rmt::Sphere* sphere);
    void Display();
    rmt::Vector*       pPosition();
    const rmt::Vector& rPosition();
    void GetPosition( rmt::Vector* ipPosn );
    ///////////////////////////////////////////////////////////////////////////


protected:
    Road* mRoad;
    unsigned int mSegmentIndex;

private:

#ifdef RAD_DEBUG
    enum { MAX_NAME_LEN = 40 };
    char mName[ MAX_NAME_LEN];
#endif

    rmt::Vector mCorners[ 4 ];      // The rectangle with extra vertices at the leading and trailing edge midpoints.
    rmt::Vector mEdgeNormals[ 4 ];  // The inward pointing edge normals.
    rmt::Vector mNormal;            // The surface normal.

    float mfSegmentLength;
    float mfLaneWidth;
    float mfRadius;
    float mfAngle;
    rmt::Sphere mSphere;

};

inline const char* RoadSegment::GetName()
{
#ifdef TOOLS
    return tEntity::GetName();
#endif

#ifdef RAD_DEBUG
    return mName;
#else
    return NULL;
#endif
}
inline void RoadSegment::SetName( const char* name )
{
#ifdef TOOLS
    tEntity::SetName( name );
#endif
#ifdef RAD_DEBUG
    strncpy( mName, name, MAX_NAME_LEN > strlen(name) ? strlen(name) : MAX_NAME_LEN );
    mName[strlen(name)] = '\0';
#endif
}

inline Road* RoadSegment::GetRoad()
{
    return mRoad;    
}
inline void RoadSegment::SetRoad( Road* road )
{
    mRoad = road;
}
inline void RoadSegment::SetSegmentIndex( unsigned int index )
{
    mSegmentIndex = index;
}
inline unsigned int RoadSegment::GetSegmentIndex()
{
    return mSegmentIndex;
}
inline void RoadSegment::DisplayBoundingBox(tColour colour)
{
    rAssert( false );
}
inline void RoadSegment::DisplayBoundingSphere(tColour colour)
{
    rAssert( false );
}
inline void RoadSegment::Display()
{
    rAssert( false ); // deprecated
}
inline rmt::Vector* RoadSegment::pPosition()
{
    rAssert( false ); // deprecated; result unreliable
    return &(mSphere.centre);
}
inline const rmt::Vector& RoadSegment::rPosition()
{
    rAssert( false ); // deprecated; result unreliable
    return mSphere.centre;
}
inline void RoadSegment::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = mSphere.centre;
}
inline float RoadSegment::GetSegmentLength()
{
    return mfSegmentLength;
}
inline float RoadSegment::GetLaneWidth()
{
    return mfLaneWidth;
}



/*

///////////////////////////////////////////////////////////////////////////
// class TransformRoadSegment
///////////////////////////////////////////////////////////////////////////
// Derived class = Base + 16*4 bytes
//
class TransformRoadSegment
:
public RoadSegment
{
public:
    TransformRoadSegment();

	TransformRoadSegment( const RoadSegmentData* pRoadSegmentData, 
        rmt::Matrix& transform 
        );

    TransformRoadSegment( const RoadSegmentData* pRoadSegmentData, 
        rmt::Matrix& transform, 
        float fScaleAlongFacing
        );

    TransformRoadSegment( const RoadSegmentData* pRoadSegmentData, 
        const rmt::Vector& facing, 
        const rmt::Vector& position
        );

    TransformRoadSegment( const RoadSegmentData* pRoadSegmentData, 
        const rmt::Vector& facing, 
        const rmt::Vector& position, 
        float fScaleAlongFacing
        );

    virtual void GetTransform( rmt::Matrix& out ) const;
    void SetTransform( rmt::Matrix& transform );
    void SetScaleAlongFacing( float scale );

    /////////////////////////////////////////////////
    // Implements RoadSegment.
    //
    void GetCorner( int index, rmt::Vector& out ) const;
    void GetEdgeNormal( int index, rmt::Vector& out ) const;
    void GetSegmentNormal( rmt::Vector& out ) const;
    //void GetJoinPoint( rmt::Vector& position, rmt::Vector& facing ) const;
    virtual void GetBoundingBox( rmt::Box3D& out ) const;
    virtual void GetBoundingSphere( rmt::Sphere& out ) const;

    //////////////////////////////////////////////////////////////////////////
    // IEntityDSG Interface
    //////////////////////////////////////////////////////////////////////////
    virtual void GetBoundingBox(rmt::Box3D* box);
    virtual void GetBoundingSphere(rmt::Sphere* sphere);
    void GetPosition( rmt::Vector* ipPosn );

private:
    rmt::Matrix mTransform;

    float mfScaleAlongFacing;
};

inline void TransformRoadSegment::SetTransform( rmt::Matrix& transform )
{
    mTransform = transform;
}
inline void TransformRoadSegment::SetScaleAlongFacing( float scale )
{
    mfScaleAlongFacing = scale;
}
inline void TransformRoadSegment::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = GetRoadSegmentData()->GetBoundingSphere().centre;
    ipPosn->Transform( mTransform );
}
*/


#endif