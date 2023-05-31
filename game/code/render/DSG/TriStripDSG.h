//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   tristripdsg
//
// Description: A triangle strip that can be inserted into the DSG, 
//				rendered using the (slow) pddiPrimStream, but
//				able to get modified after it is created and inserted
//				Intended for skid marks
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.

#ifndef TRISTRIPDSG_H
#define TRISTRIPDSG_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <render/dsg/staticentitydsg.h>
#include <p3d/shader.hpp>
#include <memory/srrmemory.h>
//===========================================================================
//
// Description: 
//		Encapsulates a vertex buffer (non-indexed, size allocated once on creation)
//		Derived from StaticEntityDSG and intended for insertion into the DSG
//
// Constraints:
//		tShader is a hack. Need to be able to set the shader in a function somewhere
//		pddi definitions may be inappropriate, use tColour and equivalents instead?
//
//===========================================================================

class TriStripDSG : public StaticEntityDSG
{
public:
    
	// Ctor, input the number of vertices to preallocate
    TriStripDSG( int maxVertices );
    virtual ~TriStripDSG();

    virtual void SetShader(tShader* pShader, int i)
    {
    }

    // Format of the vertices stored in the buffer, 24 bytes per vertex (assuming no padding)
    struct Vertex
    {
        pddiVector vertex;
        pddiColour colour;
        pddiVector2 uv;
    };

    virtual void GetBoundingBox( rmt::Box3D* box )
    {
        *box = mBoundingBox;                
    }
	virtual void GetBoundingSphere( rmt::Sphere* sphere )
	{
		*sphere = mBoundingBox.GetBoundingSphere();
	}
    virtual void GetPosition(rmt::Vector *ipPosn)
    {
        *ipPosn = mPosition;
    }
    virtual rmt::Vector* pPosition()
    {
        return &mPosition;
    }
    virtual const rmt::Vector& rPosition()
    {
        return mPosition;
    }
	// Obsolete function?
    virtual void RenderUpdate()
    {
       rAssert( false ); 
    }
	// Draw the strip using a pddiPrimStream
	virtual void Display();

	// Draw the strips bounding box
	virtual void DisplayBoundingBox( tColour colour = tColour( 255, 0, 0 ) );

	// Eliminate all vertices (well, really just sets the current vertex back to zero
	// no deallocations take place
	void Clear();

	// Add a new vertex, seperate component format
    void AddVertex( const pddiVector& vertex, pddiColour colour, const pddiVector2& uv );

	// Add a new vertex, TriStripDSG::Vertex format
	void AddVertex( const Vertex& vertex );

	void SetVertex( int index, const pddiVector& vertex, pddiColour colour, const pddiVector2& uv );

	void SetShader( tShader* pShader );
	tShader* GetShader() const { return mpShader; }
	void SetColour( int index, tColour colour )
	{
		rAssert( index >= 0 && index < mNumVertices );
		mpVertices[ index ].colour = colour;
	}
	
	// Retrieve the vertex at the specified index
	const Vertex& GetVertex( int index ) const;
	

	// Number of vertices in the vertex buffer
	inline int Size()const { return mNumVertices; }

	// Can we insert more vertices in the list
    inline bool IsSpaceLeft() const
    {
        return mNumVertices < mMaxVertices;
    }

private:
    
	// Disable default constructor
	TriStripDSG();

	tShader* mpShader;

	// Vertex buffer
    Vertex* mpVertices;

	// Encloses all vertices in the strip, extended when new vertices added
    rmt::Box3D mBoundingBox;
	// Center of the bounding box
    rmt::Vector mPosition;

	// Size of vertex buffer, all or none of these may actually hold valid vertices
    int mMaxVertices;
	// Number of vertices that are valid, number to render on Display()
    int mNumVertices;

};

#endif 

