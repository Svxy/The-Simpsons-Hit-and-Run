#ifndef INTERSECTION_LOADER_H
#define INTERSECTION_LOADER_H

// Pure 3D
#include <p3d/loadmanager.hpp>
#include <p3d/p3dtypes.hpp>

class IntersectionLoader : public tChunkHandler
{
public:
    IntersectionLoader();
    virtual ~IntersectionLoader();

    // P3D chunk loader.
    virtual tLoadStatus Load(tChunkFile* f, tEntityStore* store);

    // P3D chunk id.
    virtual bool CheckChunkID(unsigned id);
    virtual unsigned int GetChunkID();
    
private:

    // No copying or assignment. Declare but don't define.
    //
    IntersectionLoader( const IntersectionLoader& );
    IntersectionLoader& operator= ( const IntersectionLoader& );
};

#endif