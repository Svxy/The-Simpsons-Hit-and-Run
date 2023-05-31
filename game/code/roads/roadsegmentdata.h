#ifndef ROADSEGMENTDATA_H_
#define ROADSEGMENTDATA_H_

#include <radmath/radmath.hpp>
#include <p3d/entity.hpp>

#define LERP( fUnit, fV0, fV1 ) ( (1.0f-(fUnit))*fV0 + (fUnit)*fV1 )

class RoadSegmentData
{
public:
    RoadSegmentData();

    RoadSegmentData( const rmt::Vector& v0, 
        const rmt::Vector& v1, 
        const rmt::Vector& v2,
        unsigned int nLanes );

    ~RoadSegmentData();
    void SetData( const rmt::Vector& v0, 
        const rmt::Vector& v1, 
        const rmt::Vector& v2,
        unsigned int nLanes );

    const rmt::Vector& GetCorner( int index ) const;
    const rmt::Vector& GetEdgeNormal( int index ) const;
    const rmt::Vector& GetSegmentNormal( void ) const;
    unsigned int GetNumLanes( void ) const;

    void SetName( const char* name );
    tUID GetNameUID() const;

private:
    // The rectangle with extra vertices at the leading and trailing
    // edge midpoints.
    //
    rmt::Vector mCorners[ 4 ];
    
    // The inward pointing edge normals.
    //
    rmt::Vector mEdgeNormals[ 4 ];
    
    // The surface normal.
    //
    rmt::Vector mNormal;

    unsigned int mnLanes;

    tUID nameUID;
};

inline void RoadSegmentData::SetName( const char* name )
{
    nameUID = tEntity::MakeUID( name );
}
inline tUID RoadSegmentData::GetNameUID() const
{
    return nameUID;
}

#endif