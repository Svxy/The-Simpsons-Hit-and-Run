#include <windows.h>
#include <GL/glu.h>

#include <math.h>

#include <radmath/radmath.hpp>
 
#include "nodes/treelineshapenode.h"
#include "utility/mext.h"
#include "utility/mui.h"
#include "main/shapeconstants.h"
#include "main/constants.h"
#include "main/trackeditor.h"

namespace TETreeLine
{

#define DORMANT_VERTEX_COLOR	8	// purple
#define ACTIVE_VERTEX_COLOR		16	// yellow

// Vertex point size
//
#define POINT_SIZE				2.0	

void SetQuadricDrawStyle (GLUquadricObj* qobj, int token)
{
    if ((token==SMOOTH_SHADED)||(token==FLAT_SHADED))
    {
        gluQuadricNormals( qobj, GLU_SMOOTH );
        gluQuadricTexture( qobj, true );
        gluQuadricDrawStyle( qobj, GLU_FILL );
    }
    else
    {
        gluQuadricDrawStyle( qobj, GLU_LINE );
    }
}

void p3dBaseShapeUI::getDrawRequestsWireframe( MDrawRequest& request, const MDrawInfo& info )
{
    request.setToken( WIREFRAME );

    M3dView::DisplayStatus displayStatus = info.displayStatus();
    M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
    M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
    switch ( displayStatus )
    {
        case M3dView::kLead :
            request.setColor( lead_color, activeColorTable );
            break;
        case M3dView::kActive :
            request.setColor( active_color, activeColorTable );
            break;
        case M3dView::kActiveAffected :
            request.setColor( active_affected_color, activeColorTable );
            break;
        case M3dView::kDormant :
            request.setColor( dormant_color, dormantColorTable );
            break;
        case M3dView::kHilite :
            request.setColor( hilite_color, activeColorTable );
            break;
    }
}

void p3dBaseShapeUI::getDrawRequestsShaded( MDrawRequest& request, const MDrawInfo& info,
                                                          MDrawRequestQueue& queue, MDrawData& data )
{
    // Need to get the material info
    //
    MDagPath path = info.multiPath();   // path to your dag object 
    M3dView view = info.view();;     // view to draw to
    MMaterial material = MPxSurfaceShapeUI::material( path );
    M3dView::DisplayStatus displayStatus = info.displayStatus();

    // Evaluate the material and if necessary, the texture.
    //
    if ( ! material.evaluateMaterial( view, path ) ) 
    {
        MExt::DisplayError( "Could not evaluate\n" );
    }

    if ( material.materialIsTextured() ) 
    {
        material.evaluateTexture( data );
    }

    request.setMaterial( material );

    bool materialTransparent = false;
    material.getHasTransparency( materialTransparent );
    if ( materialTransparent ) 
    {
        request.setIsTransparent( true );
    }
    
    // create a draw request for wireframe on shaded if
    // necessary.
    //
    if ( (displayStatus == M3dView::kActive) ||
         (displayStatus == M3dView::kLead) ||
         (displayStatus == M3dView::kHilite) )
    {
        MDrawRequest wireRequest = info.getPrototype( *this );
        wireRequest.setDrawData( data );
        getDrawRequestsWireframe( wireRequest, info );
        wireRequest.setToken( WIREFRAME_SHADED );
        wireRequest.setDisplayStyle( M3dView::kWireFrame );
        queue.add( wireRequest );
    }
}

/////////////////////////////////////////////////////////////////////
// SHAPE NODE IMPLEMENTATION
/////////////////////////////////////////////////////////////////////
MTypeId TreelineShapeNode::id(TEConstants::NodeIDs::TreeLine);
const char* TreelineShapeNode::stringId = "TreelineShapeNode";

const char* TreelineShapeNode::SHADER_NAME_LONG = "material";
const char* TreelineShapeNode::SHADER_NAME_SHORT = "mt";
MObject TreelineShapeNode::sShader;

const char* TreelineShapeNode::USCALE_NAME_LONG = "uscale";
const char* TreelineShapeNode::USCALE_NAME_SHORT = "us";
MObject TreelineShapeNode::sUScale;

const char* TreelineShapeNode::COLOUR_NAME_LONG = "colour";
const char* TreelineShapeNode::COLOUR_NAME_SHORT = "clr";
MObject TreelineShapeNode::sColour;

const char* TreelineShapeNode::RED_NAME_LONG = "red";
const char* TreelineShapeNode::RED_NAME_SHORT = "r";
MObject TreelineShapeNode::sRed;

const char* TreelineShapeNode::GREEN_NAME_LONG = "green";
const char* TreelineShapeNode::GREEN_NAME_SHORT = "g";
MObject TreelineShapeNode::sGreen;

const char* TreelineShapeNode::BLUE_NAME_LONG = "blue";
const char* TreelineShapeNode::BLUE_NAME_SHORT = "b";
MObject TreelineShapeNode::sBlue;

const char* TreelineShapeNode::ALPHA_NAME_LONG = "alpha";
const char* TreelineShapeNode::ALPHA_NAME_SHORT = "a";
MObject TreelineShapeNode::sAlpha;

const char* TreelineShapeNode::HEIGHT_NAME_LONG = "height";
const char* TreelineShapeNode::HEIGHT_NAME_SHORT = "h";
MObject TreelineShapeNode::sHeight;

TreelineShapeNode::TreelineShapeNode()
{
}

TreelineShapeNode::~TreelineShapeNode()
{
}

//********************************************************************************************
// Description
// 
//    When instances of this node are created internally, the MObject associated
//    with the instance is not created until after the constructor of this class
//    is called. This means that no member functions of MPxSurfaceShape can
//    be called in the constructor.
//    The postConstructor solves this problem. Maya will call this function
//    after the internal object has been created.
//    As a general rule do all of your initialization in the postConstructor.
//********************************************************************************************
void TreelineShapeNode::postConstructor()
{ 
    // This call allows the shape to have shading groups assigned
    setRenderable( true );
}

//********************************************************************************************
// Compute attribute values of the node
//********************************************************************************************
MStatus TreelineShapeNode::compute( const MPlug& plug, MDataBlock& datablock )
{
    return MS::kSuccess;
}

//********************************************************************************************
// Capture when connections are made to this shape
//********************************************************************************************
MStatus TreelineShapeNode::connectionMade ( const MPlug &plug, const MPlug &otherPlug, bool asSrc ) 
{
    MObject shaderObj = otherPlug.node();
    if ( asSrc && shaderObj.hasFn(MFn::kShadingEngine) )
    {
        MFnDependencyNode parentFn( plug.node() );
        MFnDependencyNode shaderFn( shaderObj );

        // connect this material with the chosen object
 
        MStatus status;
        MPlug parentMaterialPlug = parentFn.findPlug( SHADER_NAME_LONG, &status );

        if ( !status )
        {
            MExt::DisplayError( "Could not assign %s to %s", shaderFn.name(), parentFn.name() );
        }
        else
        {
            parentMaterialPlug.setLocked( false );
            parentMaterialPlug.setValue( shaderFn.name() );
            parentMaterialPlug.setLocked( true );
        }
    }

    // let Maya process the connection too
    return MS::kUnknownParameter;
}

//********************************************************************************************
// Create instance of shape
//********************************************************************************************
void* TreelineShapeNode::creator()
{
    return new TreelineShapeNode();
}

//********************************************************************************************
// Create and initialize all attributes in maya
//********************************************************************************************
MStatus TreelineShapeNode::initialize()
{ 
    MStatus              status;
    MFnTypedAttribute    strAttr;
    MFnNumericAttribute  numAttr;
    MFnCompoundAttribute compAttr;

    sShader = strAttr.create( SHADER_NAME_LONG, SHADER_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( strAttr.setInternal( false ) );
    RETURN_STATUS_ON_FAILURE( strAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( strAttr.setKeyable( false ) );
    status = addAttribute( sShader );
    RETURN_STATUS_ON_FAILURE( status );

    sUScale = numAttr.create( USCALE_NAME_LONG, USCALE_NAME_SHORT, MFnNumericData::kDouble, 1.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.01) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(100.0) );
    RETURN_STATUS_ON_FAILURE( strAttr.setInternal( false ) );
    RETURN_STATUS_ON_FAILURE( strAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( strAttr.setKeyable( true ) );
    status = addAttribute( sUScale );
    RETURN_STATUS_ON_FAILURE( status );

    // Compound colour attribute
    sRed = numAttr.create ( RED_NAME_LONG, RED_NAME_SHORT, MFnNumericData::kFloat, 1.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(1.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );
    
    sGreen = numAttr.create ( GREEN_NAME_LONG, GREEN_NAME_SHORT, MFnNumericData::kFloat, 1.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(1.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );

    sBlue = numAttr.create ( BLUE_NAME_LONG, BLUE_NAME_SHORT, MFnNumericData::kFloat, 1.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(1.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );

    sColour = numAttr.create( COLOUR_NAME_LONG, COLOUR_NAME_SHORT, sRed, sGreen, sBlue, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setUsedAsColor(true) );
    status = addAttribute( sColour );
    RETURN_STATUS_ON_FAILURE( status );

    sAlpha = numAttr.create ( ALPHA_NAME_LONG, ALPHA_NAME_SHORT, MFnNumericData::kFloat, 1.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(1.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setInternal( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );
    status = addAttribute( sAlpha );
    RETURN_STATUS_ON_FAILURE( status );


    sHeight = numAttr.create( HEIGHT_NAME_LONG, HEIGHT_NAME_SHORT, MFnNumericData::kFloat, 10.0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.1) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(100.0) );
    RETURN_STATUS_ON_FAILURE( numAttr.setInternal( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setHidden( false ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setKeyable( true ) );
    status = addAttribute( sHeight );

    return MS::kSuccess;
}

//********************************************************************************************
// Returns the bounding box for the shape.
// In this case just use the radius and height attributes
// to determine the bounding box.
//********************************************************************************************
MBoundingBox TreelineShapeNode::boundingBox() const
{
    MBoundingBox  result; 

    MStatus status;
    MObject obj = thisMObject();

    float height = 1.0f;
        
    MFnDagNode dagNodeFn(obj);
    MPlug plug;

    plug = dagNodeFn.findPlug( sHeight, &status );
    plug.getValue( height );

    MPlug vertices = dagNodeFn.findPlug( mControlPoints, &status );
    assert( status );

    unsigned int i;
    for ( i = 0; i < vertices.numElements(); ++i )
    {
        MPoint point;
        MPlug vertex1 = vertices.elementByLogicalIndex( i, &status );
        assert( status );

        MPlug x1 = vertex1.child( mControlValueX, &status );
        assert( status );
        x1.getValue( point.x );

        MPlug y1 = vertex1.child( mControlValueY, &status );
        assert( status );
        y1.getValue( point.y );

        MPlug z1 = vertex1.child( mControlValueZ, &status );
        assert( status );
        z1.getValue( point.z );

        result.expand( point );

        point.y = point.y + ( height * TEConstants::Scale );

        result.expand( point );
    }

    return result;
} 

//=============================================================================
// TreelineShapeNode::componentToPlugs
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& component, MSelectionList& selectionList )
//
// Return:      void 
//
//=============================================================================
void TreelineShapeNode::componentToPlugs( MObject& component, 
                                          MSelectionList& selectionList ) const
{
    if ( component.hasFn(MFn::kMeshVertComponent) ) 
    {
        MFnSingleIndexedComponent fnVtxComp( component );
        MObject thisNode = thisMObject();
        MPlug plug( thisNode, mControlPoints );
        int len = fnVtxComp.elementCount();
        for ( int i = 0; i < len; i++ ) 
        {
            MPlug vtxPlug = plug.elementByLogicalIndex( fnVtxComp.element(i) );
            selectionList.add( vtxPlug );
        }
    }
}

//=============================================================================
// TreelineShapeNode::matchComponent
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MSelectionList& item, const MAttributeSpecArray& spec, MSelectionList& list )
//
// Return:      MPxSurfaceShape
//
//=============================================================================
MPxSurfaceShape::MatchResult 
TreelineShapeNode::matchComponent( const MSelectionList& item, 
                                   const MAttributeSpecArray& spec, 
                                   MSelectionList& list )
//
// Description:
//
//    Component/attribute matching method.
//    This method validates component names and indices which are
//    specified as a string and adds the corresponding component
//    to the passed in selection list.
//
//    For instance, select commands such as "select shape1.vtx[0:7]"
//    are validated with this method and the corresponding component
//    is added to the selection list.
//
// Arguments
//
//    item - DAG selection item for the object being matched
//    spec - attribute specification object
//    list - list to add components to
//
// Returns
//
//    the result of the match
//
{
    MPxSurfaceShape::MatchResult result = MPxSurfaceShape::kMatchOk;
	MAttributeSpec attrSpec = spec[0];
	int dim = attrSpec.dimensions();

	// Look for attributes specifications of the form :
	//     vtx[ index ]
	//     vtx[ lower:upper ]
	//
	if ( (1 == spec.length()) && 
         (dim > 0) && 
         (attrSpec.name() == "controlPoints" ) ) 
    {
        MObject node;
        item.getDependNode( 0, node );
        MFnDependencyNode fnDepNode( node );
		int numVertices = fnDepNode.findPlug( mControlPoints ).numElements();
		MAttributeIndex attrIndex = attrSpec[0];

		int upper = 0;
		int lower = 0;
		if ( attrIndex.hasLowerBound() ) {
			attrIndex.getLower( lower );
		}
		if ( attrIndex.hasUpperBound() ) {
			attrIndex.getUpper( upper );
		}

		// Check the attribute index range is valid
		//
		if ( (lower > upper) || (upper >= numVertices) ) {
			result = MPxSurfaceShape::kMatchInvalidAttributeRange;	
		}
		else {
			MDagPath path;
			item.getDagPath( 0, path );
			MFnSingleIndexedComponent fnVtxComp;
			MObject vtxComp = fnVtxComp.create( MFn::kMeshVertComponent );

			for ( int i=lower; i<=upper; i++ )
			{
				fnVtxComp.addElement( i );
			}
			list.add( path, vtxComp );
		}
	}
	else {
		// Pass this to the parent class
		return MPxSurfaceShape::matchComponent( item, spec, list );
	}

	return result;
}

//=============================================================================
// TreelineShapeNode::match
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MSelectionMask & mask, const MObjectArray& componentList )
//
// Return:      bool 
//
//=============================================================================
bool TreelineShapeNode::match( const MSelectionMask & mask, const MObjectArray& componentList ) const
//
// Description:
//
//		Check for matches between selection type / component list, and
//		the type of this shape / or it's components
//
//      This is used by sets and deformers to make sure that the selected
//      components fall into the "vertex only" category.
//
// Arguments
//
//		mask          - selection type mask
//		componentList - possible component list
//
// Returns
//		true if matched any
//
{
	bool result = false;

	if( componentList.length() == 0 ) {
		result = mask.intersects( MSelectionMask::kSelectMeshes );
	}
	else 
    {
		for ( int i=0; i<(int)componentList.length(); i++ ) 
        {
			if ( (componentList[i].apiType() == MFn::kMeshVertComponent) &&
				 (mask.intersects(MSelectionMask::kSelectMeshVerts))
			) 
            {
				result = true;
				break;
			}
		}
	}
    
	return result;
}

//=============================================================================
// TreelineShapeNode::transformUsing
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MMatrix & mat, const MObjectArray & componentList )
//
// Return:      void 
//
//=============================================================================
void TreelineShapeNode::transformUsing( const MMatrix & mat, 
                                        const MObjectArray & componentList )
//
// Description
//
//    Transforms the given components. This method is used by
//    the move, rotate, and scale tools in component mode.
//    Bounding box has to be updated here, so do the normals and
//    any other attributes that depend on vertex positions.
//
// Arguments
//
//    mat           - matrix to tranform the components by
//    componentList - list of components to be transformed
//
{
    MStatus stat;

    MFnDependencyNode fnDepNode( thisMObject() );
    MPlug vertices = fnDepNode.findPlug( mControlPoints );

    int len = componentList.length();
	int i;
	for ( i=0; i<len; i++ )
	{
		MObject comp = componentList[i];
		MFnSingleIndexedComponent fnComp( comp );
		int elemCount = fnComp.elementCount();
		for ( int idx=0; idx<elemCount; idx++ )
		{
			int elemIndex = fnComp.element( idx );
            MPlug vertex = vertices.elementByLogicalIndex( elemIndex );

            MPoint point;
            vertex.child(0).getValue( point.x );
            vertex.child(1).getValue( point.y );
            vertex.child(2).getValue( point.z );

            point *= mat;

            vertex.child(0).setValue( point.x );
            vertex.child(1).setValue( point.y );
            vertex.child(2).setValue( point.z );
		}
    }

	// Moving vertices will likely change the bounding box.
	//

	// Tell maya the bounding box for this object has changed
	// and thus "boundingBox()" needs to be called.
	//
	childChanged( MPxSurfaceShape::kBoundingBoxChanged );
}

//=============================================================================
// TreelineShapeNode::closestPoint 
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MPoint & toThisPoint, MPoint & theClosestPoint, double tolerance )
//
// Return:      void 
//
//=============================================================================
void TreelineShapeNode::closestPoint ( const MPoint& toThisPoint, 
                                       MPoint& theClosestPoint, 
                                       double tolerance )
{
    MStatus stat;

    MFnDependencyNode fnDepNode( thisMObject() );
    MPlug vertices = fnDepNode.findPlug( mControlPoints );

    bool found = false;
    double dist = 10000000.0;

    unsigned int i;
    for ( i = 0; i < vertices.numElements(); ++i )
    {
        MPlug vertex = vertices.elementByLogicalIndex( i );
        MPoint point;
        
        vertex.child( 0 ).getValue( point.x );
        vertex.child( 1 ).getValue( point.y );
        vertex.child( 2 ).getValue( point.z );

        if ( point.distanceTo( toThisPoint ) < dist )
        {
            dist = point.distanceTo( toThisPoint );
            theClosestPoint = point;
        }        
    }    
}

//=============================================================================
// TreelineShapeNode::SnapTreeline
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& treeline )
//
// Return:      void 
//
//=============================================================================
void TreelineShapeNode::SnapTreeline( MObject& treeline )
{
    MFnDependencyNode fnDepNode( treeline );

    MPlug vertices = fnDepNode.findPlug( mControlPoints );

    unsigned int i;
    
    for ( i = 0; i < vertices.numElements(); ++i )
    {
        MPlug vertex = vertices.elementByLogicalIndex( i );

        MPoint point;

        vertex.child( 0 ).getValue( point.x );
        vertex.child( 1 ).getValue( point.y );
        vertex.child( 2 ).getValue( point.z );

        MPoint intersectPoint;
        //Look down...
        if ( MExt::MeshIntersectAlongVector( point, MPoint(0, -100000.0, 0 ), intersectPoint ) )
        {
            vertex.child( 0 ).setValue( intersectPoint.x );
            vertex.child( 1 ).setValue( intersectPoint.y );
            vertex.child( 2 ).setValue( intersectPoint.z );
        }
        else
        {
            //Try looking up...
            if ( MExt::MeshIntersectAlongVector( point, MPoint(0, 100000.0, 0 ), intersectPoint ) )
            {
                vertex.child( 0 ).setValue( intersectPoint.x );
                vertex.child( 1 ).setValue( intersectPoint.y );
                vertex.child( 2 ).setValue( intersectPoint.z );
            }
        }
    }
}

void TreelineShapeNode::ConvertToGeometry( MObject& obj )
{
    MFnMesh newMesh;
    MObject newMeshObj;

    MFnDependencyNode fnDepNode( obj );
    MPlug vertices = fnDepNode.findPlug( mControlPoints );
    
    if ( vertices.numElements() >= 2 )
    {
        double height;
        fnDepNode.findPlug( sHeight ).getValue( height );

        double uScale;
        fnDepNode.findPlug( sUScale).getValue( uScale );

        MFloatArray uArray, vArray;
        MIntArray uvCounts, uvIds;

        unsigned int i = 0; 
        unsigned int j = 0;
        do
        {
            //Create the new mesh...
            MPointArray points;
            MPoint point1, point2, point3, point4;

            MPlug vertex1 = vertices.elementByLogicalIndex( i );
            vertex1.child(0).getValue( point1.x );
            vertex1.child(1).getValue( point1.y );
            vertex1.child(2).getValue( point1.z );

            point2 = point1;
            point1.y += height * TEConstants::Scale;

            points.append( point1 );
            points.append( point2 );

            MPlug vertex2 = vertices.elementByLogicalIndex( i + 1 );
            vertex2.child(0).getValue( point3.x );
            vertex2.child(1).getValue( point3.y );
            vertex2.child(2).getValue( point3.z );

            point4 = point3;
            point4.y += height * TEConstants::Scale;

            points.append( point3 );
            points.append( point4 );

            newMeshObj = newMesh.addPolygon( points );            

            double dist =  point2.distanceTo( point3 );
            
            float U = ceil( dist / (uScale * TEConstants::Scale ) );

            uArray.append( 0 );
            uArray.append( 0 );
            uArray.append( U );
            uArray.append( U );

            vArray.append( 1 );
            vArray.append( 0 );
            vArray.append( 0 );
            vArray.append( 1 );

            uvCounts.append( 4 );

            uvIds.append( 0 + j );
            uvIds.append( 1 + j );
            uvIds.append( 2 + j );
            uvIds.append( 3 + j );

            ++i;
            j += 4;
        } 
        while ( i < vertices.numElements() - 1 );

        MString material;
        fnDepNode.findPlug( sShader ).getValue( material );

        if ( material.length() > 0 )
        {
            //Set the material to the new object.

            newMesh.setUVs( uArray, vArray );
            newMesh.assignUVs( uvCounts, uvIds );

            //MEL command for assigning the texture.
            //sets -e -forceElement pure3dSimpleShader1SG polySurface1;
            MString meshName = newMesh.name();

            MString cmd;

            cmd += MString("sets -e -forceElement ") + material + MString(" ") + meshName;

            MStatus status;
            MGlobal::executeCommand( cmd, status );
        }
     }
    else
    {
        assert(false);
        MExt::DisplayError( "Treeline: %s is invalid for converting to geometry!", fnDepNode.name().asChar() );
    }

    if ( TrackEditor::GetDeleteTreelines() )
    {
        MExt::DeleteNode( obj, true );
    }
}


/////////////////////////////////////////////////////////////////////
// UI IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

TreelineShapeNodeUI::TreelineShapeNodeUI()
{ 
}

TreelineShapeNodeUI::~TreelineShapeNodeUI()
{
}

void* TreelineShapeNodeUI::creator()
{
    return new TreelineShapeNodeUI();
}

//********************************************************************************************
// The draw data is used to pass geometry through the 
// draw queue. The data should hold all the information
// needed to draw the shape.
//********************************************************************************************
void TreelineShapeNodeUI::getDrawRequests( const MDrawInfo & info, bool objectAndActiveOnly, MDrawRequestQueue & queue )
{
    MDrawData data;
    MDrawRequest request = info.getPrototype( *this );
    TreelineShapeNode* shapeNode = (TreelineShapeNode*)surfaceShape();
    getDrawData( NULL, data );
    request.setDrawData( data );

    // Use display status to determine what color to draw the object
    switch ( info.displayStyle() )
    {      
        case M3dView::kGouraudShaded :
            request.setToken( SMOOTH_SHADED );
            getDrawRequestsShaded( request, info, queue, data );
            queue.add( request );
            break;
        
        case M3dView::kFlatShaded :
            request.setToken( FLAT_SHADED );
            getDrawRequestsShaded( request, info, queue, data );
            queue.add( request );
            break;

        default :
            request.setToken(WIREFRAME);
            getDrawRequestsWireframe( request, info );
            queue.add( request );
            break;
        
    }

    // Add draw requests for components
	//
	if ( !objectAndActiveOnly ) 
    {
		// Inactive components
		//
		if ( (info.displayStyle() == M3dView::kPoints) ||
			 (info.displayStatus() == M3dView::kHilite) )
		{
			MDrawRequest vertexRequest = info.getPrototype( *this ); 
			vertexRequest.setDrawData( data );
			vertexRequest.setToken( VERTICES );
			vertexRequest.setColor( DORMANT_VERTEX_COLOR,  //TODO: PICK COLOURS
									M3dView::kActiveColors );

			queue.add( vertexRequest );
		}

		// Active components
		//
		if ( surfaceShape()->hasActiveComponents() ) {

			MDrawRequest activeVertexRequest = info.getPrototype( *this ); 
			activeVertexRequest.setDrawData( data );
		    activeVertexRequest.setToken( VERTICES );
		    activeVertexRequest.setColor( ACTIVE_VERTEX_COLOR,  //TODO: PICK COLOURS
										  M3dView::kActiveColors );

		    MObjectArray clist = surfaceShape()->activeComponents();
		    MObject vertexComponent = clist[0]; // Should filter list
		    activeVertexRequest.setComponent( vertexComponent );

			queue.add( activeVertexRequest );
		}
	}

}

//********************************************************************************************
// Actual draw call 
//********************************************************************************************
void TreelineShapeNodeUI::drawQuad(int drawMode) const
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    TreelineShapeNode* shapeNode = (TreelineShapeNode*)surfaceShape();
    MFnDagNode dagNodeFn(shapeNode->thisMObject());
    MPlug plug;

    MStatus status;
    float height;
    MPlug heightPlug = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::sHeight, &status );
    assert( status );

    heightPlug.getValue( height );

    MPlug vertices = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::mControlPoints, &status );
    assert( status );

    if ( vertices.numElements() >= 2 )
    {
        double uScale;
        MPlug uScalePlug = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::sUScale, &status );
        assert( status );
        uScalePlug.getValue( uScale );

        int primType;
        if ((drawMode==SMOOTH_SHADED)||(drawMode==FLAT_SHADED))
        {
            glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL);
            primType = GL_POLYGON;
        }
        else 
        {
            glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE);
            primType = GL_LINE_LOOP;
        }

        glPushMatrix();

        unsigned int i = 0; 

        do
        {
            MPoint point1, point2;
            MPlug vertex1 = vertices.elementByLogicalIndex( i, &status );
            assert( status );

            MPlug vertex2 = vertices.elementByLogicalIndex( i + 1, &status );
            assert( status );
    
            MPlug x1 = vertex1.child( TETreeLine::TreelineShapeNode::mControlValueX, &status );
            assert( status );
            x1.getValue( point1.x );

            MPlug y1 = vertex1.child( TETreeLine::TreelineShapeNode::mControlValueY, &status );
            assert( status );
            y1.getValue( point1.y );

            MPlug z1 = vertex1.child( TETreeLine::TreelineShapeNode::mControlValueZ, &status );
            assert( status );
            z1.getValue( point1.z );

            MPlug x2 = vertex2.child( TETreeLine::TreelineShapeNode::mControlValueX, &status );
            assert( status );
            x2.getValue( point2.x );

            MPlug y2 = vertex2.child( TETreeLine::TreelineShapeNode::mControlValueY, &status );
            assert( status );
            y2.getValue( point2.y );

            MPlug z2 = vertex2.child( TETreeLine::TreelineShapeNode::mControlValueZ, &status );
            assert( status );
            z2.getValue( point2.z );

            MPoint normal, vect;
            rmt::Vector normalVec;
            rmt::Vector v;

            double dist =  point1.distanceTo( point2 );

            double U = ceil( dist / (uScale * TEConstants::Scale ) );

            vect = point2 - point1;
            v.Set( vect.x, vect.y, vect.z );
            normalVec.CrossProduct( v, rmt::Vector( 0.0f, 1.0f, 0.0f ) );

            glBegin(primType);
                glNormal3f( normalVec.x, normalVec.y, normalVec.z ); //TODO: CALCULATE THIS!
                glTexCoord2f ( 0, 1 );
                glVertex3f( point1.x, 
                    (point1.y) + (height * TEConstants::Scale), 
                            point1.z );
                glTexCoord2f ( 0, 0 );
                glVertex3f( point1.x, 
                            point1.y, 
                            point1.z );
                glTexCoord2f ( U, 0 );
                glVertex3f( point2.x, 
                            point2.y, 
                            point2.z );
                glTexCoord2f ( U, 1 );
                glVertex3f( point2.x, 
                            (point2.y) + (height * TEConstants::Scale), 
                            point2.z );
            glEnd();

            ++i;
        } 
        while ( i < vertices.numElements() - 1 );
        
        glPopMatrix();

        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL);
    }
    glPopAttrib();
}

//********************************************************************************************
// From the given draw request, get the draw data and display the quad
//********************************************************************************************
void TreelineShapeNodeUI::draw( const MDrawRequest & request, M3dView & view ) const
{  
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    MDagPath dagPath = request.multiPath();
    MDrawData data = request.drawData();
    short token = request.token();
    bool drawTexture = false;

    view.beginGL(); 

    if ( (token == SMOOTH_SHADED) || (token == FLAT_SHADED) )
    {
        glEnable( GL_POLYGON_OFFSET_FILL );
        // Set up the material
        //
        MMaterial material = request.material();
        material.setMaterial(dagPath,false);

        // Enable texturing
        //
        drawTexture = material.materialIsTextured();
        if ( drawTexture ) glEnable(GL_TEXTURE_2D);

        // Apply the texture to the current view
        //
        if ( drawTexture ) 
        {
            material.applyTexture( view, data );
        }
    }
 
    if ( token == VERTICES )
    {
        drawVertices( request, view );
    }
    else
    {
        drawQuad(token);
    }

    // Turn off texture mode
    //
    if ( drawTexture ) glDisable(GL_TEXTURE_2D);

    view.endGL(); 
    glPopAttrib();
}

//********************************************************************************************
// Select function. Gets called when the bbox for the object is selected.
// This function just selects the object without doing any intersection tests.
//********************************************************************************************
bool TreelineShapeNodeUI::select( MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts ) const
{
    bool selected = false;

	if ( selectInfo.displayStatus() == M3dView::kHilite ) {
		selected = selectVertices( selectInfo, selectionList,worldSpaceSelectPts );
	}

    if ( !selected )
    {
	    M3dView view = selectInfo.view();

        //
	    // Re-Draw the object and see if they lie withing the selection area
	    // Sets OpenGL's render mode to select and stores
	    // selected items in a pick buffer
	    //
        view.beginSelect();

        switch ( selectInfo.displayStyle() )
        {      
            case M3dView::kGouraudShaded :
                drawQuad(SMOOTH_SHADED);
                break;
        
            case M3dView::kFlatShaded :
                drawQuad(FLAT_SHADED);
                break;

            default :
                drawQuad(WIREFRAME);
                break;      
        }

        if( view.endSelect() > 0 )	
	    {
            MSelectionMask priorityMask( MSelectionMask::kSelectObjectsMask );
            MSelectionList item;
            item.add( selectInfo.selectPath() );
            MPoint xformedPt;
            selectInfo.addSelection( item, xformedPt, selectionList, worldSpaceSelectPts, priorityMask, false );
            return true;
        }
    }
    
    return selected;
}

//=============================================================================
// TreelineShapeNodeUI::drawVertices
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MDrawRequest & request, M3dView & view )
//
// Return:      void 
//
//=============================================================================
void TreelineShapeNodeUI::drawVertices( const MDrawRequest & request, M3dView & view ) const
//
// Description:
//
//     Component (vertex) drawing routine
//
// Arguments:
//
//     request - request to be drawn
//     view    - view to draw into
//
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
	MDrawData data = request.drawData();
    TreelineShapeNode* shapeNode = (TreelineShapeNode*)surfaceShape();
    MFnDagNode dagNodeFn(shapeNode->thisMObject());

	view.beginGL(); 

	// Query current state so it can be restored
	//
	bool lightingWasOn = glIsEnabled( GL_LIGHTING ) ? true : false;
	if ( lightingWasOn ) {
		glDisable( GL_LIGHTING );
	}
	float lastPointSize;
	glGetFloatv( GL_POINT_SIZE, &lastPointSize );

	// Set the point size of the vertices
	//
	glPointSize( POINT_SIZE );

	// If there is a component specified by the draw request
	// then loop over comp (using an MFnComponent class) and draw the
	// active vertices, otherwise draw all vertices.
	//
	MObject comp = request.component();
	if ( ! comp.isNull() ) 
    {
        MPlug vertices = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::mControlPoints );
        
		MFnSingleIndexedComponent fnComponent( comp );
		for ( int i=0; i<fnComponent.elementCount(); i++ )
		{
			int index = fnComponent.element( i );
			glBegin( GL_POINTS );
            MPlug vertexPlug = vertices.elementByLogicalIndex( index );

			MPoint vertex;
            vertexPlug.child( 0 ).getValue( vertex.x );
            vertexPlug.child( 1 ).getValue( vertex.y );
            vertexPlug.child( 2 ).getValue( vertex.z );

            glVertex3f( (float)vertex[0], 
						(float)vertex[1], 
						(float)vertex[2] );
			glEnd();

			char annotation[32];
			sprintf( annotation, "%d", index );
			view.drawText( annotation, vertex );
		}
	}
    else
    {
        MPlug vertices = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::mControlPoints );
        
		for ( int i=0; i<vertices.numElements(); i++ )
		{
			glBegin( GL_POINTS );
            MPlug vertexPlug = vertices.elementByLogicalIndex( i );

			MPoint vertex;
            vertexPlug.child( 0 ).getValue( vertex.x );
            vertexPlug.child( 1 ).getValue( vertex.y );
            vertexPlug.child( 2 ).getValue( vertex.z );

            glVertex3f( (float)vertex[0], 
						(float)vertex[1], 
						(float)vertex[2] );
		    glEnd();
        }
    }
	// Restore the state
	//
	if ( lightingWasOn ) {
		glEnable( GL_LIGHTING );
	}
	glPointSize( lastPointSize );

	view.endGL(); 
    glPopAttrib();
}

//=============================================================================
// TreelineShapeNodeUI::selectVertices
//=============================================================================
// Description: Comment
//
// Parameters:  ( MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts )
//
// Return:      bool 
//
//=============================================================================
bool TreelineShapeNodeUI::selectVertices( MSelectInfo &selectInfo, 
                                          MSelectionList &selectionList, 
                                          MPointArray &worldSpaceSelectPts ) const
//
// Description:
//
//     Vertex selection.
//
// Arguments:
//
//     selectInfo           - the selection state information
//     selectionList        - the list of selected items to add to
//     worldSpaceSelectPts  -
//
{
	bool selected = false;
	M3dView view = selectInfo.view();

	MPoint 		xformedPoint;
	MPoint 		currentPoint;
	MPoint 		selectionPoint;
	double		z,previousZ = 0.0;
 	int			closestPointVertexIndex = -1;

	const MDagPath & path = selectInfo.multiPath();

	// Create a component that will store the selected vertices
	//
	MFnSingleIndexedComponent fnComponent;
	MObject surfaceComponent = fnComponent.create( MFn::kMeshVertComponent );
	int vertexIndex;

	// if the user did a single mouse click and we find > 1 selection
	// we will use the alignmentMatrix to find out which is the closest
	//
	MMatrix	alignmentMatrix;
	MPoint singlePoint; 
	bool singleSelection = selectInfo.singleSelection();
	if( singleSelection ) {
		alignmentMatrix = selectInfo.getAlignmentMatrix();
	}

	// Get the geometry information
	//
    TreelineShapeNode* shapeNode = (TreelineShapeNode*)surfaceShape();
    MFnDagNode dagNodeFn(shapeNode->thisMObject());

	// Loop through all vertices of the mesh and
	// see if they lie withing the selection area
	//
    MPlug vertices = dagNodeFn.findPlug( TETreeLine::TreelineShapeNode::mControlPoints );

	int numVertices = vertices.numElements();

	for ( vertexIndex=0; vertexIndex<numVertices; vertexIndex++ )
	{
        MPlug vertexPlug = vertices.elementByLogicalIndex( vertexIndex );

		MPoint currentPoint;
        vertexPlug.child( 0 ).getValue( currentPoint.x );
        vertexPlug.child( 1 ).getValue( currentPoint.y );
        vertexPlug.child( 2 ).getValue( currentPoint.z );

		// Sets OpenGL's render mode to select and stores
		// selected items in a pick buffer
		//
		view.beginSelect();
      
		glBegin( GL_POINTS );

        glVertex3f( (float)currentPoint[0], 
					(float)currentPoint[1], 
					(float)currentPoint[2] );
		glEnd();

		if ( view.endSelect() > 0 )	// Hit count > 0
		{
			selected = true;

			if ( singleSelection ) {
				xformedPoint = currentPoint;
				xformedPoint.homogenize();
				xformedPoint*= alignmentMatrix;
				z = xformedPoint.z;
				if ( closestPointVertexIndex < 0 || z > previousZ ) {
					closestPointVertexIndex = vertexIndex;
					singlePoint = currentPoint;
					previousZ = z;
				}
			} else {
				// multiple selection, store all elements
				//
				fnComponent.addElement( vertexIndex );
			}
		}
	}

	// If single selection, insert the closest point into the array
	//
	if ( selected && selectInfo.singleSelection() ) {
		fnComponent.addElement(closestPointVertexIndex);

		// need to get world space position for this vertex
		//
		selectionPoint = singlePoint;
		selectionPoint *= path.inclusiveMatrix();
	}

	// Add the selected component to the selection list
	//
	if ( selected ) {
		MSelectionList selectionItem;
		selectionItem.add( path, surfaceComponent );

		MSelectionMask mask( MSelectionMask::kSelectComponentsMask );
		selectInfo.addSelection(
			selectionItem, selectionPoint,
			selectionList, worldSpaceSelectPts,
			mask, true );
	}

	return selected;
}

}  //namespace TETreeLine


