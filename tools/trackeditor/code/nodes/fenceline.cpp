#include "fenceline.h"
#include "walllocator.h"
#include "utility/mext.h"

#include <toollib.hpp>

MTypeId FenceLineNode::id(  TEConstants::TypeIDPrefix, TEConstants::NodeIDs::FenceLine );
const char*  FenceLineNode::stringId = "FenceLine";

//==============================================================================
// FenceLineNode::FenceLineNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      FenceLineNode
//
//==============================================================================
FenceLineNode::FenceLineNode() {}

//==============================================================================
// FenceLineNode::~FenceLineNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      FenceLineNode
//
//==============================================================================
FenceLineNode::~FenceLineNode() {}

//==============================================================================
// FenceLineNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* FenceLineNode::creator()
{
    return new FenceLineNode();
}

//==============================================================================
// FenceLineNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus FenceLineNode::initialize()
{
    return MS::kSuccess;
}

//==============================================================================
// FenceLineNode::postConstructor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void FenceLineNode::postConstructor()
{
    //No moving the fenceline.
    MPlug lPlug( thisMObject(), localPosition );
    lPlug.setLocked( true );

    MPlug wPlug( thisMObject(), worldPosition );
    wPlug.setLocked( true );
}

//This is how you export one of these.
//==============================================================================
// FenceLineNode::Export
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& fenceNode, tlHistory& history )
//
// Return:      tlDataChunk
//
//==============================================================================
tlDataChunk* FenceLineNode::Export( MObject& fenceNode, tlHistory& history )
{
    //This fenceline assumes that there are fences below it.
    MFnDagNode fnNode( fenceNode );

    if ( fnNode.typeId() == FenceLineNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlFenceLineChunk* fenceLine = new tlFenceLineChunk;

        //Go through all it's children and add them to the chunk incrementing the 
        //count.

        unsigned int childCount = 0;
        MItDag dagIt( MItDag::kDepthFirst, MFn::kLocator );

        MFnDagNode fnDag( fenceNode );
        MObject fenceT = fnDag.parent( 0 );

        dagIt.reset( fenceT );

        while ( !dagIt.isDone() )
        { 
            MFnDependencyNode fnNode( dagIt.item() );
            MTypeId id = fnNode.typeId();

            if ( id == WallLocatorNode::id )
            {
                //Export a wall locator;
                tlDataChunk* newChunk = WallLocatorNode::Export( dagIt.item(), history );

                //Append this to the fence line
                fenceLine->AppendSubChunk( newChunk );

                ++childCount;
            }

            dagIt.next();
        }

        if ( childCount )
        {
            fenceLine->SetNumWalls( childCount );
        }
        else
        {
            delete fenceLine;
            return NULL;
        }

        return fenceLine;
    }

    assert( false );
    return NULL;
}

//==============================================================================
// FenceLineNode::AddWall
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& fenceLine, MObject& wall )
//
// Return:      void 
//
//==============================================================================
void FenceLineNode::AddWall( MObject& fenceLine, MObject& wall )
{
    //Test to make sure the fenceLine passed in is an obj, not a transform.
    MFnDagNode fnDag( fenceLine );
    if ( fnDag.typeId() == FenceLineNode::id )
    {
        MExt::AddChild( fenceLine, wall );
    }
    else
    {
        if ( fenceLine.apiType() == MFn::kTransform )
        {
            //We need to find the FenceLine node that is the child of this transform.
            unsigned int childCount = fnDag.childCount();

            unsigned int i;

            MObject child;
            MFnDagNode fnDagChild;

            for ( i = 0; i < childCount; ++i )
            {
                child = fnDag.child( i );

                fnDagChild.setObject( child );

                if ( fnDagChild.typeId() == FenceLineNode::id )
                {
                    //This is the child.
                    MExt::AddChild( child, wall );
                    return;
                }
            }

            MExt::DisplayError( "Tried to parent something strange to fenceLine" );
            assert(false);
        }
    }
}

