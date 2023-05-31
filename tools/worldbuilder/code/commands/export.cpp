#include "main/toolhack.h"

#include "precompiled/PCH.h"

#include "export.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "nodes/eventlocatornode.h"
#include "nodes/scriptlocatornode.h"
#include "nodes/genericlocatornode.h"
#include "nodes/carstartlocatornode.h"
#include "nodes/triggervolumenode.h"
#include "nodes/splinelocatornode.h"
#include "nodes/zoneeventlocatornode.h"
#include "nodes/occlusionlocatornode.h"
#include "nodes/railcamlocatornode.h"
#include "nodes/interiorentrancelocatornode.h"
#include "nodes/directionallocatornode.h"
#include "nodes/actioneventlocatornode.h"
#include "nodes/fovlocatornode.h"
#include "nodes/BreakableCameraLocatorNode.h"
#include "nodes/StaticCameraLocatorNode.h"
#include "nodes/PedGroupLocator.h"
#include "nodes/wbspline.h"
#include "utility/mui.h"
#include "utility/mext.h"

#include <toollib.hpp>

#include "../../../game/code/meta/locatortypes.h"

const char*  ExportCommand::stringId = "WB_Export";
bool ExportCommand::sRegisteredChunks = false;

ExportCommand::ExportCommand() {};
ExportCommand::~ExportCommand() {};

//==============================================================================
// ExportCommand::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
// 
//==============================================================================
void* ExportCommand::creator()
{
    return new ExportCommand();
}

//==============================================================================
// ExportCommand::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus ExportCommand::doIt( const MArgList& args )
{
    if ( !sRegisteredChunks )
    {
        tlDataChunk::RegisterDefaultChunks();
        sRegisteredChunks = true;
    }
   
    MStatus status;

    //Go through all the chunks looking for the types we want and exporting them.
    //Alternatively, we could go thtough all the chunks and attempt to access the
    //IExportable interface and then export if the interface exists...

    //Args are all, or selected.

    const unsigned char FILE_NAME_SIZE = 255;
    char filePath[FILE_NAME_SIZE];
    filePath[0] = '\0';

    if ( MUI::FileDialog( filePath,  
                          FILE_NAME_SIZE, 
                          "World Builder Export", 
                          "Pure3D(*.p3d)|*.p3d|All Files(*.*)|*.*||", 
                          "p3d", 
                          MUI::SAVE ) )
    {
        MDagPath pathToWorldBuilder;
        if ( !MExt::FindDagNodeByName( &pathToWorldBuilder, MString( WorldBuilder::sName ) ) )
        {
            return MStatus::kSuccess;
        }        

        MItDag dagIt;
        dagIt.reset( pathToWorldBuilder, MItDag::kBreadthFirst );

        tlDataChunk* outChunk = new tlDataChunk;

        //Put in a history chunk.
        tlHistory history;
        
        char hist[256];
//        sprintf(hist, "World Builder version: 2.0, toollib version: %s", tlversion);

//        history.AddLine( hist );

        outChunk->AppendSubChunk( history.Chunk(), 0 );

        bool deleteLast = false;
        MFnDependencyNode fnNode;
        MObject lastObj;
        MTypeId id;

        while ( !dagIt.isDone() )
        { 
            fnNode.setObject( dagIt.item() );
            id = fnNode.typeId();

            tlDataChunk* newChunk = NULL;

            if ( id == EventLocatorNode::id )
            {
                //Export an event locator;
                newChunk = EventLocatorNode::Export( dagIt.item() );
            }
            else if ( id == ScriptLocatorNode::id )
            {
                //export a script locator
                newChunk = ScriptLocatorNode::Export( dagIt.item() );
            }
            else if ( id == GenericLocatorNode::id )
            {
                //export a script locator
                newChunk = GenericLocatorNode::Export( dagIt.item() );
            }
            else if ( id == CarStartLocatorNode::id )
            {
                //export a script locator
                newChunk = CarStartLocatorNode::Export( dagIt.item() );
            }
            else if ( id == SplineLocatorNode::id )
            {
                //Export a spline locator
                newChunk = SplineLocatorNode::Export( dagIt.item() );
            }
            else if ( id == ZoneEventLocatorNode::id )
            {
                //Export a zone event locator
                newChunk = ZoneEventLocatorNode::Export( dagIt.item() );
            }
            else if ( id == OcclusionLocatorNode::id )
            {
                //Export a zone event locator
                newChunk = OcclusionLocatorNode::Export( dagIt.item() );
            }
            else if ( id == InteriorEntranceLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = InteriorEntranceLocatorNode::Export( dagIt.item() );
            }
            else if ( id == DirectionalLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = DirectionalLocatorNode::Export( dagIt.item() );
            }
            else if ( id == ActionEventLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = ActionEventLocatorNode::Export( dagIt.item() );
            }
            else if ( id == FOVLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = FOVLocatorNode::Export( dagIt.item() );
            }
            else if ( id == BreakableCameraLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = BreakableCameraLocatorNode::Export( dagIt.item() );
            }
            else if ( id == StaticCameraLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = StaticCameraLocatorNode::Export( dagIt.item() );
            }
            else if ( id == PedGroupLocatorNode::id )
            {
                //Export an interior entrance
                newChunk = PedGroupLocatorNode::Export( dagIt.item() );
            }
            else if ( id == RailCamLocatorNode::id )
            {
                //Don't export, but don't delete either.
            }
            else
            {
                //Is it a nurbs curve or a trigger volume?
                MFnNurbsCurve( dagIt.item(), &status );

                if ( !status && id != TriggerVolumeNode::id )
                {
                    deleteLast = true;
                }
            }

            if ( newChunk )
            {
                tlWBLocatorChunk* newLoc = dynamic_cast<tlWBLocatorChunk*>(newChunk);

                if ( newLoc )
                {
                    if ( newLoc->Type() == LocatorType::GENERIC )
                    {
                        unsigned int i;
                        for ( i = 0; i < newChunk->SubChunkCount(); ++i )
                        {
                            outChunk->AppendSubChunk( newChunk->GetSubChunk( i ) );
                        
                        }

                        if ( i == 0 )
                        {
                            //Append to output file
                            outChunk->AppendSubChunk( newChunk );
                        }
                    }
                    else
                    {
                        outChunk->AppendSubChunk( newChunk );
                    }
                }
                else
                {

                    //Append to output file
                    outChunk->AppendSubChunk( newChunk );
                }
            }
            
            if ( deleteLast )
            {
                lastObj = dagIt.item();
            }
            
            dagIt.next();

            if ( deleteLast )
            {
                MFnTransform fnTransform( lastObj, &status );
                if ( !status )
                {
                    MExt::DisplayWarning( "Deleting useless node: %s", fnNode.name().asChar() );
                    MExt::DeleteNode( lastObj, true );
                }
                deleteLast = false;
            }
        }

        tlFile output(new tlFileByteStream(filePath, omWRITE), tlFile::CHUNK32);

        if(!output.IsOpen()) 
        {

            MGlobal::displayError("Unable to write file!");

            delete outChunk;
            return MS::kFailure;
        }

        //Sort it out..
        outChunk->SortSubChunks();
        outChunk->Write(&output);

        delete outChunk;

        MGlobal::executeCommand( MString("flushUndo") );
    }

    return MS::kSuccess;
}
