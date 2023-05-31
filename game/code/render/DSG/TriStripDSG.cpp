#include <p3d/utility.hpp>
#include <render/DSG/tristripdsg.h>
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


//===========================================================================
// TriStripDSG::TriStripDSG
//===========================================================================
// Description:
//		TriStripDSG constructor
//
// Constraints:
//		Cannot change the maximum size of the vertex buffer, ever (prevents fragmentation)
//		Tristrippool has functionality to end a TriStripDSG and start a new one automatically
//		when it runs out of space
//
// Parameters:
//		Number of vertices to preallocate. Vertices are 24 bytes each + padding		
//
// Return:
//		None.
//
//===========================================================================


TriStripDSG::TriStripDSG( int maxVertices )
:mpShader( NULL )
{
	IEntityDSG::mTranslucent = true;
    mMaxVertices = maxVertices;
    mpVertices = new TriStripDSG::Vertex[ mMaxVertices ];
    mNumVertices = 0;
}

//===========================================================================
// TriStripDSG::~TriStripDSG
//===========================================================================
// Description:
//		TriStripDSG dtor. Frees all memory allocated in the ctor.
//
// Constraints:
//
// Parameters:
//		None.	
//
// Return:
//		None.
//
//===========================================================================


TriStripDSG::~TriStripDSG()
{
BEGIN_PROFILE( "TriStripDSG Destroy" );
    delete [] mpVertices;
    mpVertices = NULL;       
	if( mpShader != NULL )
	{
		mpShader->Release();
	}	
END_PROFILE( "TriStripDSG Destroy" );
}
//===========================================================================
// TriStripDSG::GetVertex
//===========================================================================
// Description:
//		Returns the information for a vertex in the array.
//
// Constraints:
//		Assertion failure if index isn't valid or not filled out.
//
// Parameters:
//		const reference (careful) to the internal vertex structure
//
// Return:
//		None.
//
//===========================================================================

const TriStripDSG::Vertex& TriStripDSG::GetVertex( int index ) const
{
	rAssert( index >=0 && index < mNumVertices );
	return mpVertices[ index ];	
}

//===========================================================================
// TriStripDSG::Display
//===========================================================================
// Description:
//		Overloaded Display() function from DSG parent
//
// Constraints:
//	
//	
// Parameters:
//		None
//
// Return:
//		None.
//
//===========================================================================
void TriStripDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  TriStripDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
	
	if ( mNumVertices > 0 )
	{

		// We will hack this stuff in for use with the skid marks
		// subtractive blending
       // rAssert( mpShader != NULL );

        if( mpShader != NULL )
        {
		    pddiPrimStream* pStream = p3d::pddi->BeginPrims( mpShader->GetShader(), PDDI_PRIM_TRISTRIP, PDDI_V_CT, mNumVertices );

		    for (int i = 0 ; i < mNumVertices ; ++i)
		    {
			    pStream->Vertex( &mpVertices[ i ].vertex, mpVertices[ i ].colour, &mpVertices[ i ].uv );
		    }
		    p3d::pddi->EndPrims( pStream ); 
        }
	}
    DSG_END_PROFILE(profileName)
}
//===========================================================================
// TriStripDSG::Clear
//===========================================================================
// Description:
//		Resets number of vertices to zero.
//
// Constraints:
//		Doesnt free vertex buffer
//	
// Parameters:
//		None
//
// Return:
//		None.
//
//===========================================================================
void TriStripDSG::Clear()
{
	mNumVertices = 0;
	mBoundingBox = rmt::Box3D();
}
//===========================================================================
// TriStripDSG::SetShader
//===========================================================================
// Description:
//		Sets the shader to use with the strip
//
// Constraints:
//		
//	
// Parameters:
//		None
//
// Return:
//		None.
//
//===========================================================================

void TriStripDSG::SetShader( tShader* pShader) 
{
    if ( pShader != mpShader )
    {
	    if( pShader != NULL)
	    {
		    pShader->AddRef();
	    }
	    if( mpShader != NULL)
	    {
		    mpShader->Release();
	    }
	    mpShader = pShader;
    }
}
//===========================================================================
// TriStripDSG::AddVertex
//===========================================================================
// Description:
//		Adds a vertex the strip. 
//
// Constraints:
//		Assertion if not enough space. Use IsSpaceLeft() to check whether or not to call
//		the function
//
// Parameters:
//		Position, colour, uv coordinates
//
// Return:
//		None.
//
//===========================================================================

void TriStripDSG::AddVertex( const pddiVector& vertex, pddiColour colour, const pddiVector2& uv )
{
	rAssert( IsSpaceLeft() == true );
	mpVertices[ mNumVertices ].vertex = vertex;
	mpVertices[ mNumVertices ].colour = colour;
	mpVertices[ mNumVertices ].uv = uv;

	++mNumVertices;

	// Adjust bounding box based upon the new vertex position.
	mBoundingBox.Expand( vertex );
	mPosition = mBoundingBox.Mid();
}
//===========================================================================
// TriStripDSG::AddVertex
//===========================================================================
// Description:
//		Adds a new vertex
//
// Constraints:
//		Assertion if the strip has run out of room
//
// Parameters:
//		TriStripDSG::Vertex structure.
//
// Return:
//		None.
//
//===========================================================================

void TriStripDSG::AddVertex( const Vertex& vertex )
{
    rAssert( IsSpaceLeft() == true );
	mpVertices[ mNumVertices ] = vertex;
	++mNumVertices;

    // Adjust bounding box based upon the new vertex position.
    mBoundingBox.Expand( vertex.vertex );
    mPosition = mBoundingBox.Mid();
}
//===========================================================================
// TriStripDSG::SetVertex
//===========================================================================
// Description:
//		Modifies an existing vertex
//
// Constraints:
//		Assertion if vertex does not exist
//
// Parameters:
//		position, colour, and texture coordinates
//
// Return:
//		None.
//
//===========================================================================

void TriStripDSG::SetVertex( int index, const pddiVector& vertex, pddiColour colour, const pddiVector2& uv )
{
	rAssert( index >=0 && index < mNumVertices );
	mpVertices[ index ].vertex = vertex;
	mpVertices[ index ].colour = colour;
	mpVertices[ index ].uv = uv;

    // Adjust bounding box based upon the new vertex position.
    mBoundingBox.Expand( vertex );
    mPosition = mBoundingBox.Mid();
}

//===========================================================================
// TriStripDSG::DisplayBoundingBox
//===========================================================================
// Description:
//		Draws the bounding box associated with the object
//
// Constraints:
//		Debugging only
//
// Parameters:
//		Bounding box colour
//
// Return:
//		None.
//
//===========================================================================

void TriStripDSG::DisplayBoundingBox( tColour colour )
{
#ifndef RAD_RELEASE
   pddiPrimStream* stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.low.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.low.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.high.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.low.y, mBoundingBox.low.z);
   p3d::pddi->EndPrims(stream);

   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.high.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.low.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.low.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.high.z);
   p3d::pddi->EndPrims(stream);

   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.low.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.low.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.high.x, mBoundingBox.high.y, mBoundingBox.high.z);
   p3d::pddi->EndPrims(stream);

   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.high.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.low.y, mBoundingBox.high.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.low.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.high.y, mBoundingBox.low.z);
   stream->Colour(colour);
   stream->Coord(mBoundingBox.low.x, mBoundingBox.high.y, mBoundingBox.high.z);
   p3d::pddi->EndPrims(stream);
#endif
}