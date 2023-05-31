#include "precompiled/PCH.h"

#include "export.h"
#include "main/constants.h"
#include "nodes/walllocator.h"
#include "nodes/fenceline.h"
#include "nodes/intersection.h"
#include "nodes/road.h"
#include "nodes/pedpath.h"
#include "utility/mui.h"
#include "utility/mext.h"

#include <toollib.hpp>

const char*  ExportCommand::stringId = "TE_Export";
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

    //Go through all the chunks looking for the types we want and exporting them.
    //Alternatively, we could go thtough all the chunks and attempt to access the
    //IExportable interface and then export if the interface exists...

    //Args are all, or selected.

    const unsigned char FILE_NAME_SIZE = 255;
    char filePath[FILE_NAME_SIZE];
    filePath[0] = '\0';

    if ( MUI::FileDialog( filePath,  
                          FILE_NAME_SIZE, 
                          "Track Editor Export", 
                          "Pure3D(*.p3d)|*.p3d|All Files(*.*)|*.*||", 
                          "p3d", 
                          MUI::SAVE ) )
    {
        //TODO: add selected.
        MItDag dagIt( MItDag::kBreadthFirst, MFn::kLocator );

        tlDataChunk* outChunk = new tlDataChunk;

        //Put in a history chunk.
        tlHistory history;
        
//        char hist[256];
//        sprintf(hist, "Track Editor version: 2.0, toollib version: %s", tlversion);
//
//        history.AddLine( hist );

//        outChunk->AppendSubChunk( history.Chunk(), 0 );

        bool deleteLast = false;
        MFnDependencyNode fnNode;
        MObject lastObj;
        MTypeId id;

        while ( !dagIt.isDone() )
        { 
            fnNode.setObject( dagIt.item() );
            id = fnNode.typeId();

            if ( id == FenceLineNode::id )
            {
                //Export a wall locator;
                tlDataChunk* newChunk = FenceLineNode::Export( dagIt.item(), history );

                if ( newChunk )
                {
                    //Append this to the output file.
                    outChunk->AppendSubChunk( newChunk );
                }
                else
                {
                    //Time to go away.
                    deleteLast = true;
                }
            }
            else if ( id == IntersectionLocatorNode::id )
            {
                tlDataChunk* newChunk = IntersectionLocatorNode::Export( dagIt.item(), history );

                if ( newChunk )
                {
                    //Append this to the output file.
                    outChunk->AppendSubChunk( newChunk );
                }
                else
                {
                    //Time to go away.
                    deleteLast = true;
                }
            }
            else if ( id == RoadNode::id )
            {
                tlDataChunk* newChunk = RoadNode::Export( dagIt.item(), history, outChunk );

                if ( newChunk )
                {
                    //Append this to the output file.
                    outChunk->AppendSubChunk( newChunk );
                }
                else
                {
                    //Time to go away.
                    deleteLast = true;
                }
            }
            else if ( id == PedPathNode::id )
            {
                tlDataChunk* newChunk = PedPathNode::Export( dagIt.item(), history );

                if ( newChunk )
                {
                    //Append this to the output file.
                    outChunk->AppendSubChunk( newChunk );
                }
                else
                {
                    //Time to go away.
                    deleteLast = true;
                }
            }

            if ( deleteLast )
            {
                lastObj = dagIt.item();
            }
            
            dagIt.next();

            if ( deleteLast )
            {
                MExt::DisplayWarning( "Deleting useless node: %s", fnNode.name().asChar() );
                MExt::DeleteNode( lastObj, true );
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
    }

    return MS::kSuccess;
}
