#include "precompiled/PCH.h"

#include "intersectioncommands.h"
#include "utility/mext.h"
#include "nodes/road.h"
#include "main/trackeditor.h"
#include "nodes/intersection.h"


const char* CreateRoadCmd::stringId = "TE_CreateRoad";
const char* AddIntersectionToRoadCmd::stringId = "TE_AddIntersectionToRoad";


//==============================================================================
// CreateRoadCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* CreateRoadCmd::creator()
{
    return new CreateRoadCmd();
}

//==============================================================================
// CreateRoadCmd::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus CreateRoadCmd::doIt( const MArgList& args )
{
    //Take all the selected road segments and create a road from them.
    //If there is a segment that has not been roadified, highlight that one and return an error message.

    MSelectionList selectList;
    MGlobal::getActiveSelectionList( selectList );
    MItSelectionList itSelect( selectList, MFn::kMesh );

    if ( selectList.length() <= 0 || itSelect.isDone() )
    {
        MExt::DisplayWarning( "Nothing to do, please select road segments!" );
        return MStatus::kSuccess;
    }

    MObjectArray segArray;
    MObject obj;
    MFnMesh fnMesh;
    MPlug whichRoadPlug;
    MStatus status;

    while ( !itSelect.isDone() )
    {
        //Gather all the road segments and add them to the new road.
        itSelect.getDependNode( obj );

        fnMesh.setObject( obj );

        whichRoadPlug = fnMesh.findPlug( MString( "teWhichRoad" ), &status );
        
        if ( status == MStatus::kSuccess )
        {
            //This is one of them.
            segArray.append( obj );
        }

        itSelect.next();
    }

    if ( segArray.length() <= 0 )
    {
        //There were no appropriate segs in the selection.
        MExt::DisplayWarning( "Nothing to do, please select road segments!" );
        return MStatus::kSuccess;
    }

    MObject newRoad;
    MObject newRoadT;

    MExt::CreateNode( newRoad, newRoadT, MString( RoadNode::stringId ) );

    assert( !newRoad.isNull() );
   
    unsigned int i;
    for ( i = 0; i < segArray.length(); ++i )
    {
        //Test to see if this road seg is already connected.
        if ( MExt::IsConnected( segArray[ i ], "teWhichRoad" ) )
        {
            MExt::DisconnectAll( segArray[ i ], "teWhichRoad" );
        }

        MExt::Connect( segArray[ i ], "teWhichRoad", newRoad, RoadNode::ROAD_SEG_NAME_LONG );
    }

    TrackEditor::AddChild( newRoad );

    return MStatus::kSuccess;
}


//==============================================================================
// AddIntersectionToRoadCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* AddIntersectionToRoadCmd::creator()
{
    return new AddIntersectionToRoadCmd();
}

//==============================================================================
// AddIntersectionToRoadCmd::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus AddIntersectionToRoadCmd::doIt( const MArgList& args )
{
    MStatus status;

    //Arg 0 is the name of the intersection (the road is selected)
    //Arg 1 is whether it is a start or end point on the road.

    assert( args.length() == 2 );

    MObjectArray roadArray;

    if ( GetRoadsFromSelectionList( roadArray ) )
    {
        MString intersectionName;
        args.get( 0, intersectionName );

        if ( intersectionName == MString( "" ) )
        {
            MExt::DisplayWarning( "Must have an intersection selected in the editor." );
            return MStatus::kSuccess;
        }

        bool isEnd;
        args.get( 1, isEnd );

        MDagPath dagPath;
        if ( !MExt::FindDagNodeByName( &dagPath, intersectionName ) )
        {
            MExt::DisplayWarning( "The Intersection: %s does not exist!", intersectionName.asChar() );
            return MStatus::kSuccess;
        }

        MFnDagNode fnIntersectionDagNode( dagPath );

        unsigned int i;
        for ( i = 0; i < roadArray.length(); ++i )
        {
            if ( isEnd )
            {
                MExt::DisconnectAll( roadArray[i], RoadNode::INTERSECTION_END_LONG );
                MExt::Connect( roadArray[i], RoadNode::INTERSECTION_END_LONG, fnIntersectionDagNode.object(), IntersectionLocatorNode::ROAD_LONG );
            }
            else
            {
                MExt::DisconnectAll( roadArray[i], RoadNode::INTERSECTION_START_LONG );
                MExt::Connect( roadArray[i], RoadNode::INTERSECTION_START_LONG, fnIntersectionDagNode.object(), IntersectionLocatorNode::ROAD_LONG );
            }
        }
    }

    return MStatus::kSuccess;
}


const char* ShowRoadCmd::stringId = "TE_ShowRoad";

//==============================================================================
// ShowRoadCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* ShowRoadCmd::creator()
{
    return new ShowRoadCmd();
}

//==============================================================================
// ShowRoadCmd::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus ShowRoadCmd::doIt( const MArgList& args )
{

    MObject road;
    
    if ( GetRoadFromSelectionList( road ) )
    {
        MString cmd;

        MFnDependencyNode fnNode( road );
        MPlug roadPlug = fnNode.findPlug( MString( RoadNode::ROAD_SEG_NAME_LONG ) );
        assert( roadPlug.isArray() );

        MGlobal::clearSelectionList();
       
        MPlugArray source, dest;
        MExt::ResolveConnections( &source, &dest, roadPlug, AS_DEST );

        assert( source.length() );

        unsigned int i;
        for ( i = 0; i < source.length(); ++i )
        {
            fnNode.setObject( source[i].node() );
            cmd = MString( "select -add " ) + fnNode.name();

            MGlobal::executeCommand( cmd );
        }

        fnNode.setObject( road );
        cmd = MString("select -add ") + fnNode.name();
        MGlobal::executeCommand( cmd );
    }

    return MStatus::kSuccess;
}


const char* DestroyRoadCmd::stringId = "TE_DestroyRoad";

//==============================================================================
// DestroyRoadCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* DestroyRoadCmd::creator()
{
    return new DestroyRoadCmd();
}

//==============================================================================
// DestroyRoadCmd::doIt
//==============================================================================
// Description: Comment 
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus DestroyRoadCmd::doIt( const MArgList& args )
{

    MObject road;
    
    if ( GetRoadFromSelectionList( road ) )
    {
        MExt::DeleteNode( road, true );
    }

    return MStatus::kSuccess;
}



//==============================================================================
// GetRoadFromSelectionList
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& road )
//
// Return:      bool
//
//==============================================================================
bool GetRoadFromSelectionList( MObject& road )
{
    MStatus status;

    MSelectionList selectList;
    MGlobal::getActiveSelectionList( selectList );

    if ( selectList.length() <= 0 ) 
    {
        MExt::DisplayWarning( "A road segment must be selected!" );
        return false;
    }
    else
    {
        MObject segment;
        selectList.getDependNode( 0, segment );
        MFnDagNode fnNode( segment );

        if ( fnNode.typeName() == MString( RoadNode::stringId ) )
        {
            //this is a road segment
            road = fnNode.object();
        }
        else
        {
            //Test to make sure the selected item is a road segment.
            MFn::Type type = fnNode.type();

            if ( fnNode.typeName() == MString( "transform" ) )
            {
                //We want the child of this, not the transform.
                fnNode.setObject( fnNode.child( 0 ) );
            }

            MPlug whichRoadPlug = fnNode.findPlug( MString( "teWhichRoad" ), &status );

            if ( status )
            {
                //Get the intersection connected to this road and select all the road segs
                //attached to it.
                if ( whichRoadPlug.isConnected() ) 
                {
                    //Get the road Locator;
                    MPlugArray plugs;
                    whichRoadPlug.connectedTo( plugs, false, true );

                    assert( plugs.length() > 0 );

                    //Get to road attached to the segment.
                    road = plugs[ 0 ].node();
                }
                else
                {
                    MExt::DisplayWarning( "This road segment is not part of a road!" );
                    return false;
                }
            }
            else
            {
                MExt::DisplayWarning( "A road segment must be selected!" );
                return false;
            }
        }
    }

    return true;
}

//==============================================================================
// GetRoadsFromSelectionList
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& road )
//
// Return:      bool
//
//==============================================================================
bool GetRoadsFromSelectionList( MObjectArray& roadArray )
{
    MStatus status;

    MSelectionList selectList;
    MGlobal::getActiveSelectionList( selectList );

    if ( selectList.length() <= 0 ) 
    {
        MExt::DisplayWarning( "At least one road segment must be selected!" );
        return false;
    }
    else
    {
        unsigned int i;
        for ( i = 0; i < selectList.length(); ++i )
        {
            MObject node;
            selectList.getDependNode( i, node );
            MFnDagNode fnNode( node );

            if ( fnNode.typeName() == MString( RoadNode::stringId ) )
            {
                //this is a road node
                roadArray.append( fnNode.object() );
            }
            else
            {
                //Test to make sure the selected item is a road segment.
                MFn::Type type = fnNode.type();

                if ( fnNode.typeName() == MString( "transform" ) )
                {
                    //We want the child of this, not the transform.
                    fnNode.setObject( fnNode.child( 0 ) );
                }

                MPlug whichRoadPlug = fnNode.findPlug( MString( "teWhichRoad" ), &status );

                if ( status )
                {
                    //Get the intersection connected to this road and select all the road segs
                    //attached to it.
                    if ( whichRoadPlug.isConnected() ) 
                    {
                        //Get the road Locator;
                        MPlugArray plugs;
                        whichRoadPlug.connectedTo( plugs, false, true );

                        assert( plugs.length() > 0 );

                        //Get to road attached to the segment.
                        roadArray.append( plugs[ 0 ].node() );
                    }
                    else
                    {
                        MExt::DisplayWarning( "This road segment: %s is not part of a road!", fnNode.name().asChar() );
                        return false;
                    }
                }
            }
        }
    }

    return true;
}