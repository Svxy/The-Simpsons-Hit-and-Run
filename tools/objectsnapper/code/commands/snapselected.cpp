//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement SnapSelectedCmd
//
// History:     18/03/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech

//========================================
// Project Includes
//========================================
#include "snapselected.h"
#include "utility/MUI.h"
#include "utility/mext.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

const char* SnapSelectedCmd::stringId = "OS_SnapSelected";
const double OSScale = 100.0;

enum SnapType
{
    ALL,
    SINGLE,
    TREELINE
};

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SnapSelectedCmd::SnapSelectedCmd
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapSelectedCmd::SnapSelectedCmd()
{
    mObjDagPaths.clear();
    mNewPositions.clear();
    mOldPositions.clear();
}

//==============================================================================
// SnapSelectedCmd::~SnapSelectedCmd
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapSelectedCmd::~SnapSelectedCmd()
{
}

//==============================================================================
// SnapSelectedCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* SnapSelectedCmd::creator()
{
    return new SnapSelectedCmd();
}

//==============================================================================
// SnapSelectedCmd::doit
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus SnapSelectedCmd::doIt( const MArgList& args )
{
    if ( !MUI::ConfirmDialog( "Are you sure you want to snap these objects?\nYou can not UNDO this command." ) == MUI::YES )
    {
        //quit.
        return MStatus::kSuccess;
    }

    assert( args.length() == 2 );

    double OFFSET = 0;
    int snapType = ALL;

    args.get( 0, OFFSET );
    args.get( 1, snapType );

    OFFSET *= OSScale;

    MSelectionList list;
    MGlobal::getActiveSelectionList( list );
    
    MItSelectionList i( list );

    MFnDagNode fnDagNode;
    MDagPath dagPath;
    MItDag itDag;
    MObject obj;
    MDagPath objDagPath;
    MObject childObj;

    //For all selected objects.
    for ( ; !i.isDone(); i.next() )
    {
        i.getDagPath( dagPath );

        itDag.reset( dagPath, MItDag::kBreadthFirst, MFn::kTransform );

        for ( ; !itDag.isDone() && itDag.depth() < 2; itDag.next() )
        {
            obj = itDag.item();
            fnDagNode.setObject( obj );

            const char* objName = fnDagNode.name().asChar();
            const char* objTypeName = fnDagNode.typeName().asChar();
    
            int childCount = fnDagNode.childCount();

            int whichChild;

            for ( whichChild = 0; whichChild < childCount; ++whichChild )
            {
                childObj = fnDagNode.child( whichChild );
                fnDagNode.setObject( childObj );

                const char* childObjName = fnDagNode.name().asChar();
                const char* childObjTypeName = fnDagNode.typeName().asChar();

                //Find a mesh below me and move my pivot to the intersection.
                itDag.getPath( objDagPath );
                MFnTransform fnTrans( objDagPath );

                //Get all the child meshes of this obj node to prevent snapping to 
                //something that is part of me.

                MStringArray meshNames;

                GetChildMeshNames( objDagPath, meshNames );


                MVector pos = fnTrans.translation( MSpace::kWorld );
                MPoint rotate = fnTrans.rotatePivot( MSpace::kWorld );
                MVector rayDir( 0, -1.0, 0 );

                MItDag meshIt( MItDag::kDepthFirst, MFn::kMesh );
                MDagPath meshDagPath;
                MPointArray intersectPoints;

                bool found = false;

                for ( ; !meshIt.isDone(); meshIt.next() )
                {
                    meshIt.getPath( meshDagPath );
                    MFnMesh mesh( meshDagPath );

                    const char* meshName = mesh.name().asChar();                        

                    unsigned int i;
                    bool nameFound = false;
                    for ( i = 0; i < meshNames.length(); ++i )
                    {
                        if ( meshNames[i] == mesh.name() )
                        {
                            nameFound = true;
                            break;
                        }
                    }

                    if ( nameFound )
                    {
                        continue;
                    }

                    if ( snapType == TREELINE )
                    {

                    }
                    else
                    {
                        mesh.intersect( rotate, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

                        if ( intersectPoints.length() > 0 )
                        {
                            MVector diff( intersectPoints[ 0 ] - rotate );
                            diff.y += OFFSET;

                            //Prepare the command for the redo it ( which does all the work )
                            mObjDagPaths.append( fnTrans.dagPath() );
                            mNewPositions.append( diff );

                            //Save the old position.
                            MVector vector = fnTrans.translation( MSpace::kObject );
                            mOldPositions.append( vector );

//                            //Move the transform.
//                            fnTrans.translateBy( diff, MSpace::kWorld );

                            found = true;

                            break;
                        }
                    }
                }

                if ( !found )
                {
                    //Look up
                    MPoint rotate = fnTrans.rotatePivot( MSpace::kWorld );
                    MVector rayDir( 0, 1.0, 0 );

                    MItDag meshIt( MItDag::kDepthFirst, MFn::kMesh );
                    MDagPath meshDagPath;
                    MPointArray intersectPoints;

                    for ( ; !meshIt.isDone(); meshIt.next() )
                    {
                        meshIt.getPath( meshDagPath );
                        MFnMesh mesh( meshDagPath );

                        const char* meshName = mesh.name().asChar();                        

                        unsigned int i;
                        bool nameFound = false;
                        for ( i = 0; i < meshNames.length(); ++i )
                        {
                            if ( meshNames[i] == mesh.name() )
                            {
                                nameFound = true;
                                break;
                            }
                        }

                        if ( nameFound )
                        {
                            continue;
                        }

                        if ( snapType == TREELINE )
                        {

                        }
                        else
                        {
                            mesh.intersect( rotate, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

                            if ( intersectPoints.length() > 0 )
                            {
                                MVector diff( intersectPoints[ 0 ] - rotate );
                                diff.y -= OFFSET;

                                //Prepare the command for the redo it ( which does all the work )
                                mObjDagPaths.append( fnTrans.dagPath() );
                                mNewPositions.append( diff );

                                //Save the old position.
                                MVector vector = fnTrans.translation( MSpace::kObject );
                                mOldPositions.append( vector );

//                                //Move the transform.
//                                fnTrans.translateBy( diff, MSpace::kWorld );

                                found = true;

                                break;
                            }
                        }
                    }
                }

                if ( !found )
                {
                    MString errorMsg( "The object: " );
                    errorMsg += fnTrans.name();
                    errorMsg += MString( " has no mesh below it.\nNo snapping done on it." );

                    MUI::InfoDialog( errorMsg.asChar() );
                }
            }

            if ( snapType == SINGLE )
            {
                //No more iterations.
                break;
            }
        }
    }

    redoIt();

    return MStatus::kSuccess;
}

//=============================================================================
// SnapSelectedCmd::undoIt
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus SnapSelectedCmd::undoIt()
{
    unsigned int i;
    for ( i = 0; i < mObjDagPaths.length(); ++i )
    {
        if ( mObjDagPaths[i].isValid() )
        {
            //Move this guy to the new position.
            MFnTransform fnTransform( mObjDagPaths[i] );

            fnTransform.setTranslation( mOldPositions[i], MSpace::kObject );
        }
        else
        {
            MExt::DisplayError( "Error performing snap due to invalid object or change in heirarchy" );
        }
    }

    return MStatus::kSuccess;
}

//=============================================================================
// SnapSelectedCmd::redoIt
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus SnapSelectedCmd::redoIt()
{
    unsigned int i;
    for ( i = 0; i < mObjDagPaths.length(); ++i )
    {
        if ( mObjDagPaths[i].isValid() )
        {
            //Move this guy to the new position.
            MFnTransform fnTransform( mObjDagPaths[i] );

            fnTransform.translateBy( mNewPositions[i], MSpace::kWorld );
        }
        else
        {
            MExt::DisplayError( "Error performing snap due to invalid object or change in heirarchy" );
        }
    }

    return MStatus::kSuccess;
}

//=============================================================================
// SnapSelectedCmd::isUndoable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool SnapSelectedCmd::isUndoable() const
{
    return true;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// SnapSelectedCmd::GetChildMeshNames
//==============================================================================
// Description: Comment
//
// Parameters:  ( MDagPath objDagPath, MStringArray& names )
//
// Return:      void 
//
//==============================================================================
void SnapSelectedCmd::GetChildMeshNames( MDagPath objDagPath, MStringArray& names )
{
    names.clear();

    MItDag itDag;
    itDag.reset( objDagPath, MItDag::kDepthFirst, MFn::kMesh );

    for ( ; !itDag.isDone(); itDag.next() )
    {
            MDagPath dagPath;
            itDag.getPath( dagPath );
            MFnMesh fnMesh( dagPath );

            names.append( fnMesh.name() );

            const char* meshName = fnMesh.name().asChar();
    }

}
