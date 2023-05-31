#include "precompiled/PCH.h"

#include "trackeditorcommands.h"
#include "main/trackeditor.h"
#include "utility/mext.h"
#include "main/constants.h"

//TEStateChange
const char* TEStateChangeCommand::stringId = "TE_StateChange";

//==============================================================================
// TEStateChangeCommand::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* TEStateChangeCommand::creator()
{
    return new TEStateChangeCommand();
}

//==============================================================================
// TEStateChangeCommand::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus TEStateChangeCommand::doIt( const MArgList& args )
{
    assert( args.length() == 1 );

    int arg;
    args.get( 0, arg );

    TrackEditor::SetEditMode( (TrackEditor::EditMode) arg );

    return MStatus::kSuccess;
}

//TEGetSelectedVertexPosition

const char* TEGetSelectedVertexPosition::stringId = "TE_GetSelectedVertexPosition";

//==============================================================================
// TEGetSelectedVertexPosition::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* TEGetSelectedVertexPosition::creator()
{
    return new TEGetSelectedVertexPosition(); 
}

//==============================================================================
// TEGetSelectedVertexPosition::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus TEGetSelectedVertexPosition::doIt( const MArgList& args )
{
    MStatus status;
    MDoubleArray returnVal( 3, 0 );

    assert( args.length() == 1 );  //Only one arg.

    MString argString;
    args.get( 0, argString );

    MSelectionList activeList;
    MGlobal::getActiveSelectionList(activeList);

    MItSelectionList iter( activeList,  MFn::kMeshVertComponent, &status);

    //We're only going to deal with the first selected item.  Don't select more
    //Than one please.
    
    if ( !iter.isDone() )
    {
        MDagPath item;
        MObject component;
        iter.getDagPath( item, component );
        // do something with it

        MStatus isMeshIT;
        MItMeshVertex mITVert( item , component, &isMeshIT );

        if(isMeshIT == MS::kSuccess)
        {
            MPoint vertPos;
            double x, y, z;
            
            if ( argString == MString( "local" ) )
            {
                vertPos = mITVert.position( MSpace::kObject, &status );
                x = vertPos[0];
                y = vertPos[1];
                z = vertPos[2];
            }
            else  //"world"
            {
                vertPos = mITVert.position( MSpace::kWorld, &status );
                x = vertPos[0];
                y = vertPos[1];
                z = vertPos[2];
            }
            
            returnVal[0] = x / TEConstants::Scale;
            returnVal[1] = y / TEConstants::Scale;
            returnVal[2] = z / TEConstants::Scale;

        }

        iter.next(); //This is to test if there are more verts than needed.
    }
    else
    {
        MExt::DisplayWarning("No vertices selected!");
    }

    if ( !iter.isDone() )
    {
        MExt::DisplayWarning("Too many vertices selected!");
    }


    MPxCommand::setResult( returnVal );

    return MStatus::kSuccess;
}


//TEGetSelectedVertexIndex

const char* TEGetSelectedVertexIndex::stringId = "TE_GetSelectedVertexIndex";

//==============================================================================
// TEGetSelectedVertexIndex::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* TEGetSelectedVertexIndex::creator()
{
    return new TEGetSelectedVertexIndex();
}

//==============================================================================
// TEGetSelectedVertexIndex::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus TEGetSelectedVertexIndex::doIt( const MArgList& args )
{
    int returnVal = -1;

    MStatus status;
    MSelectionList activeList;
    MGlobal::getActiveSelectionList(activeList);

    MItSelectionList iter( activeList,  MFn::kMeshVertComponent, &status);

    //We're only going to deal with the first selected item.  Don't select more
    //Than one please.
    
    if ( !iter.isDone() )
    {
        MDagPath item;
        MObject component;
        iter.getDagPath( item, component );
        // do something with it

        MStatus isMeshIT;
        MItMeshVertex mITVert( item , component, &isMeshIT );

        if(isMeshIT == MS::kSuccess)
        {
            returnVal = mITVert.index();
        }

        iter.next(); //This is to test if there are more verts than needed.
    }
    else
    {
        MExt::DisplayWarning("No vertices selected!");
    }

    if ( !iter.isDone() )
    {
        MExt::DisplayWarning("Too many vertices selected!");
    }

    MPxCommand::setResult( returnVal );

    return MStatus::kSuccess;
}
