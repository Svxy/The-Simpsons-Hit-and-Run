#include "pedpath.h"
#include "walllocator.h"
#include "utility/mext.h"

#include <toollib.hpp>

MTypeId PedPathNode::id(  TEConstants::TypeIDPrefix, TEConstants::NodeIDs::PedPath );
const char*  PedPathNode::stringId = "PedPath";

//==============================================================================
// PedPathNode::PedPathNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PedPathNode
//
//==============================================================================
PedPathNode::PedPathNode() {}

//==============================================================================
// PedPathNode::~PedPathNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PedPathNode
//
//==============================================================================
PedPathNode::~PedPathNode() {}

//==============================================================================
// PedPathNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* PedPathNode::creator()
{
    return new PedPathNode();
}

//==============================================================================
// PedPathNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus PedPathNode::initialize()
{
    return MS::kSuccess;
}

//==============================================================================
// PedPathNode::postConstructor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void PedPathNode::postConstructor()
{
    //No moving the pedpath.
    MPlug lPlug( thisMObject(), localPosition );
    lPlug.setLocked( true );

    MPlug wPlug( thisMObject(), worldPosition );
    wPlug.setLocked( true );
}

//This is how you export one of these.
//==============================================================================
// PedPathNode::Export
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& pedNode, tlHistory& history )
//
// Return:      tlDataChunk
//
//==============================================================================
tlDataChunk* PedPathNode::Export( MObject& pedNode, tlHistory& history )
{
    //This fenceline assumes that there are fences below it.
    MFnDagNode fnNode( pedNode );

    if ( fnNode.typeId() == PedPathNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlPedpathChunk* pedPath = new tlPedpathChunk;

        //Go through all it's children and add them to the chunk incrementing the 
        //count.

        unsigned int childCount = 0;
        MItDag dagIt( MItDag::kDepthFirst, MFn::kLocator );

        MFnDagNode fnDag( pedNode );
        MObject fenceT = fnDag.parent( 0 );

        dagIt.reset( fenceT );

        tlDataChunk tempChunk;

        while ( !dagIt.isDone() )
        { 
            MFnDependencyNode fnNode( dagIt.item() );
            MTypeId id = fnNode.typeId();

            if ( id == WallLocatorNode::id )
            {
                //Export a wall locator;
                tlWallChunk* newChunk = reinterpret_cast<tlWallChunk*>(WallLocatorNode::Export( dagIt.item(), history ));

                //Append this to the fence line
                tempChunk.AppendSubChunk( newChunk );

                ++childCount;
            }

            dagIt.next();
        }

        if ( childCount )
        {
            tlPoint* points = new tlPoint[childCount + 1];

            unsigned int i;
//            for ( i = 0; i < tempChunk.SubChunkCount(); i++ )
//            {
//                points[i] = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( i ))->GetStart();
//
//                if ( i == tempChunk.SubChunkCount() - 1 )
//                {
//                    points[childCount] = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( i ))->GetEnd();
//                }
//            }

            //Okay, we need to order the points...  If there was a split, the points will
            //be badly ordered.
            points[0] = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( 0 ))->GetStart();  //First point is always good.

            //This is the testing point for the loop below.
            tlPoint testPoint = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( 0 ))->GetEnd();
            tempChunk.RemoveSubChunk( 0 );
            
            unsigned int remainingChunks = childCount - 1;
            unsigned int foundCount = 1;


            while ( foundCount < childCount )
            {
                bool found = false;
                unsigned int chunkIndex = 0;
                for ( i = 0; i < remainingChunks; ++i )
                {
                    if ( !found && reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( i ))->GetStart() == testPoint )
                    {
                        points[foundCount] = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( i ))->GetStart();
                        found = true;
                        foundCount++;

                        //heheh
                        if ( foundCount == childCount )
                        {
                            points[foundCount] = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( i ))->GetEnd();
                            foundCount++;                        
                        }

                        chunkIndex = i;
                    }
                }

                if ( found )
                {
                    testPoint = reinterpret_cast<tlWallChunk*>(tempChunk.GetSubChunk( chunkIndex ))->GetEnd();

                    tempChunk.RemoveSubChunk( chunkIndex );
                    remainingChunks--;
                }
                else
                {
                    MExt::DisplayError("WHOA!  Big error, get Cary!!!!  Looks like ped paths are screwy..");
                    break;
                }
            }



            pedPath->SetPoints( points, childCount + 1 );
            pedPath->SetNumPoints( childCount + 1 );

            delete[] points;
            points = NULL;
        }
        else
        {
            delete pedPath;
            return NULL;
        }

        return pedPath;
    }

    assert( false );
    return NULL;
}

//==============================================================================
// PedPathNode::AddWall
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& pedPath, MObject& wall )
//
// Return:      void 
//
//==============================================================================
void PedPathNode::AddWall( MObject& pedPath, MObject& wall )
{
    //Test to make sure the fenceLine passed in is an obj, not a transform.
    MFnDagNode fnDag( pedPath );
    if ( fnDag.typeId() == PedPathNode::id )
    {
        MExt::AddChild( pedPath, wall );
    }
    else
    {
        if ( pedPath.apiType() == MFn::kTransform )
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

                if ( fnDagChild.typeId() == PedPathNode::id )
                {
                    //This is the child.
                    MExt::AddChild( child, wall );
                    return;
                }
            }

            MExt::DisplayError( "Tried to parent something strange to pedPath" );
            assert(false);
        }
    }
}

