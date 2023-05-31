#include "tiledisplay.h"
#include "main/constants.h"
#include "main/trackeditor.h"
#include "utility/glext.h"
#include "utility/mext.h"

MTypeId TileDisplayNode::id( TEConstants::TypeIDPrefix, TEConstants::NodeIDs::TileDisplay );
const char*  TileDisplayNode::stringId = "TileDisplayNode";

const int TileDisplayNode::ORIGIN_COLOUR = 10;
const int TileDisplayNode::ROAD_COLOUR = 11;
const int TileDisplayNode::TOP_COLOUR = 12;
const int TileDisplayNode::BOTTOM_COLOUR = 13;
const int TileDisplayNode::LANE_COLOUR = 4;
const float TileDisplayNode::SCALE = 1.0f * TEConstants::Scale;
const float TileDisplayNode::LINE_WIDTH = 6.0f;
const float TileDisplayNode::Y_OFFSET = 0.5f;

TileDisplayNode::TileDisplayNode() {};

TileDisplayNode::~TileDisplayNode() {};

//==============================================================================
// TileDisplayNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* TileDisplayNode::creator()
{
    return new TileDisplayNode();
}
//==============================================================================
// TileDisplayNode::draw
//==============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, 
//                const MDagPath& path, 
//                M3dView::DisplayStyle displayStyle, 
//                M3dView::DisplayStatus displayStatus )
//                            
//
// Return:      void 
//
//==============================================================================
void TileDisplayNode::draw( M3dView& view, 
                            const MDagPath& path, 
                            M3dView::DisplayStyle displayStyle, 
                            M3dView::DisplayStatus displayStatus 
                            )
{
    if ( TrackEditor::GetEditMode() != TrackEditor::OFF )
    {
        //Display the arrows for the selected mesh.
        MStatus status;
        MSelectionList activeList;
        MGlobal::getActiveSelectionList(activeList);

        MItSelectionList iter( activeList,  MFn::kMesh, &status);

        unsigned int count = activeList.length();

        //Draw
        view.beginGL();
        glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

        while ( !iter.isDone() )
        {
            MDagPath meshDagPath;
            iter.getDagPath( meshDagPath );

            MFnMesh fnMesh( meshDagPath );

            MPlug teOriginPlug = fnMesh.findPlug( MString("teOrigin"), &status );

            if ( status == MStatus::kSuccess )
            {
                //This is a mesh of the appropriate type
                //Get all the vertices that are set and display arrows for them.
                int originVert;
                teOriginPlug.getValue( originVert );

                int roadVert;
                MPlug teRoadPlug = fnMesh.findPlug( MString("teRoad"), &status );
                assert( status );
                teRoadPlug.getValue( roadVert );

                int topVert;
                MPlug teTopPlug = fnMesh.findPlug( MString("teTop"), &status );
                assert( status );
                teTopPlug.getValue( topVert );

                int bottomVert;
                MPlug teBottomPlug = fnMesh.findPlug( MString("teBottom"), &status );
                assert( status );
                teBottomPlug.getValue( bottomVert );

                int numLanes;
                MPlug teLanesPlug = fnMesh.findPlug( MString("teLanes"), &status );
                assert( status );
                teLanesPlug.getValue( numLanes );

                if ( numLanes < 1 )
                {
                    MExt::DisplayError( "The mesh %s has a road with no lanes!", fnMesh.name().asChar() );
                }

                MPoint teOriginPoint;
                fnMesh.getPoint( originVert, teOriginPoint, MSpace::kWorld );
                teOriginPoint[1] += Y_OFFSET;

                MPoint teRoadPoint;
                fnMesh.getPoint( roadVert, teRoadPoint, MSpace::kWorld );
                teRoadPoint[1] += Y_OFFSET;

                MPoint teTopPoint;
                fnMesh.getPoint( topVert, teTopPoint, MSpace::kWorld );
                teTopPoint[1] += Y_OFFSET;

                MPoint teBottomPoint;
                fnMesh.getPoint( bottomVert, teBottomPoint, MSpace::kWorld );
                teBottomPoint[1] += Y_OFFSET;

                //When we are in render mode, we draw the lines.
                //If this was in GL_SELECTION_MODE, we would not draw the lines, so they won't interfere
                //with selection.
                GLint value;
                glGetIntegerv( GL_RENDER_MODE, &value );

                if ( (value == GL_RENDER) )
                {
                    if ( originVert >= 0 )
                    {
                        view.setDrawColor( ORIGIN_COLOUR, M3dView::kActiveColors );
                        GLExt::drawSphere( 0.5f * SCALE, teOriginPoint );
                    }

                    if ( roadVert >= 0 )
                    {
                        view.setDrawColor( ROAD_COLOUR, M3dView::kActiveColors );
                        GLExt::drawSphere( 0.5f * SCALE, teRoadPoint );
                        GLExt::drawLine( teOriginPoint, teRoadPoint, LINE_WIDTH );
                    }

                    if ( topVert >= 0 )
                    {
                        view.setDrawColor( TOP_COLOUR, M3dView::kActiveColors );
                        GLExt::drawSphere( 0.5f * SCALE, teTopPoint );
                        GLExt::drawLine( teRoadPoint, teTopPoint, LINE_WIDTH );
                    }

                    if ( bottomVert >= 0 )
                    {
                        view.setDrawColor( BOTTOM_COLOUR, M3dView::kActiveColors );
                        GLExt::drawSphere( 0.5f * SCALE, teBottomPoint );
                        GLExt::drawLine( teOriginPoint, teBottomPoint, LINE_WIDTH );
                    }

                    if ( numLanes > 0 )
                    {
                        MVector scaledVector0, scaledVector1;
                        scaledVector0 = teBottomPoint - teOriginPoint;
                        scaledVector0 /= numLanes;
                        scaledVector0 /= 2;
                        
                        scaledVector1 = teTopPoint - teRoadPoint;
                        scaledVector1 /= numLanes;
                        scaledVector1 /= 2;

                        unsigned int i;
                        for( i = 0; i < numLanes; ++i )
                        {
                            MPoint p0, p1;

                            p0 = teOriginPoint + scaledVector0 + (scaledVector0 * i * 2);
                            p1 = teRoadPoint + scaledVector1 + (scaledVector1 * i * 2);
                          
                            view.setDrawColor( LANE_COLOUR, M3dView::kActiveColors );
                            GLExt::drawArrow( p0, p1, LINE_WIDTH );
                        }
                    }
                }
            }

            iter.next();
        }

        glPopAttrib();
        view.endGL();
    }
}

//==============================================================================
// TileDisplayNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus TileDisplayNode::initialize()
{
    return MStatus::kSuccess;
}

