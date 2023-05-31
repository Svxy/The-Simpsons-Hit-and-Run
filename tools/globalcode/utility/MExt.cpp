//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MExt.cpp
//
// Description: Functions that extend the Maya API to perform other common 
//              tasks.
//
// Modification History:
//  + Created Aug 21, 2001 -- bkusy 
//-----------------------------------------------------------------------------
#include "precompiled/PCH.h"

//----------------------------------------
// System Includes
//----------------------------------------
//#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <math.h>

/*  Using precompiled headers  
#include <maya/M3dView.h>
#include <maya/MArgList.h>
#include <maya/MDagPath.h>
#include <maya/MDoubleArray.h>
#include <maya/MDGModifier.h>
#include <maya/MDoubleArray.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
*/

//----------------------------------------
// Project Includes
//----------------------------------------
#include "MExt.h"
#include "util.h"

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------
static const double EPSILON             = 0.00001;
static const int    SCRATCHPAD_SIZE     = 256;
static char         scratchpad[ SCRATCHPAD_SIZE + 1 ];


//-----------------------------------------------------------------------------
// G e t S c a l e d 
//
// Synopsis:    Retrieves an attribute that is first scaled by the scale in 
//              the node's parenting transform.
//
// Parameters:  vertex - reference parameter to receive scaled attribute.
//              node   - the node the attribute is on.
//              attr   - the attribute to retrieve.
//
// Returns:     The status of the request, hopefully MS::kSuccess.
//
// Constraints: This method only applies the scale that is stored in the 
//              immediate parent of the node.  Prior parenting tranforms have
//              no effect.
//-----------------------------------------------------------------------------
void MExt::Attr::GetScaled( MPoint* vertex, 
                            const MObject& node, 
                            const MObject& attr 
                          )
{
    MStatus status;

    //
    // Get the nodes parenting transform.
    //
    MFnDagNode fnNode( node, &status );
    assert( status );

    MObject transform = fnNode.parent( 0, &status );
    assert( status );

    MFnTransform fnTransform( transform, &status );
    assert( status );

    //
    // Get the scale in the parenting transform.
    //
    double scale[3];
	status = fnTransform.getScale( scale );
    assert( status );

    //
    // Get the attribute.
    //
    Get( vertex, node, attr );

    //
    // Scale the attribute.
    //
    vertex->x *= scale[0];
    vertex->y *= scale[1];
    vertex->z *= scale[2];
}

//-----------------------------------------------------------------------------
// S e t S c a l e d
//
// Synopsis:    Sets the attribute after taking into account the scale set in
//              the immediate parenting transform.  If the attribute initially
//              has a value of (10,10,10) and a scaling vector of 
//              (0.5, 2.0, 0.5) is in the parenting transform.  The attribute
//              will be stored as (20,5,20) so that it will reflect the 
//              original value when the attribute is retrieved through the
//              transform at a later time.
//
// Parameters:  vertex - the vertex values to set the attribute to.
//              node   - the node to set the attribute on.
//              attr   - the attribute to set.
//
// Returns:     NOTHING
//
// Constraints: Only the scaling in the immediate parenting transform is taken
//              into account.
//
//-----------------------------------------------------------------------------
void MExt::Attr::SetScaled( const MPoint& vertex, 
                            MObject& node,
                            MObject& attr 
                          )
{
    MStatus status;

    //
    // Get the nodes parenting transform.
    //
    MFnDagNode fnNode( node, &status );
    assert( status );

    MObject transform = fnNode.parent( 0, &status );
    assert( status );

    MFnTransform fnTransform( transform, &status );
    assert( status );

    //
    // Get the scale in the parenting transform.
    //
    double scale[3];
	status = fnTransform.getScale( scale );
    assert( status );

    //
    // Create the "unscaled" vertex.
    //
    MPoint scaledVertex = vertex;
    scaledVertex.x = scaledVertex.x / scale[0];
    scaledVertex.y = scaledVertex.y / scale[1];
    scaledVertex.z = scaledVertex.z / scale[2];

    Set( scaledVertex, node, attr );
}

//-----------------------------------------------------------------------------
// S e t S c a l e d
//
// Synopsis:    Sets the attribute after taking into account the scale set in
//              the immediate parenting transform.  If the attribute initially
//              has a value of (10,10,10) and a scaling vector of 
//              (0.5, 2.0, 0.5) is in the parenting transform.  The attribute
//              will be stored as (20,5,20) so that it will reflect the 
//              original value when the attribute is retrieved through the
//              transform at a later time.
//
// Parameters:  vertex - the vertex values to set the attribute to.
//              node   - the node to set the attribute on.
//              attr   - the name of the attribute.
//
// Returns:     NOTHING
//
// Constraints: Only the scaling in the immediate parenting transform is taken
//              into account.
//
//-----------------------------------------------------------------------------
void MExt::Attr::SetScaled( const MPoint& vertex, 
                            MObject& node,
                            const MString& attr 
                          )
{
    MStatus status;
    
    //
    // Get the attribute object that corresponds to the named attribute.
    //
    MFnDagNode fnNode( node, &status );
    assert( status );

    MPlug plug = fnNode.findPlug( attr, &status );
    assert( status );

    SetScaled( vertex, node, plug.attribute() );
}

//-----------------------------------------------------------------------------
// O p t i o n P a r s e r : : O p t i o n P a r s e r  
//
// Synopsis:    Constructor
//
// Parameters:  args - the MArgList passed into functions like doIt();
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MExt::OptionParser::OptionParser( const char* command, const MArgList& args ) :
  m_argc( 0 ),
  m_argv( 0 ),
  m_opt( 0 )
{
    assert( command );

    typedef char* charPointer;

    //
    // Create our simultated argument environment.  We add one to m_argc because
    // we are inserting the command name as well.
    //
    m_argc = args.length() + 1;
    m_argv = new charPointer[ m_argc ];
    assert( m_argv );

    //
    // Copy in the command name.
    //
    m_argv[0] = new char[ strlen( command ) + 1 ];
    assert( m_argv[0] );
    strcpy( m_argv[0], command );

    //
    // Copy in the arguments from argList.
    //
    int i;
    for ( i = 1; i < m_argc; i++ )
    {
        MString arg;
        args.get( i - 1, arg );
        m_argv[i] = new char[ strlen( arg.asChar() ) + 1 ];
        assert( m_argv[i] );
        strcpy( m_argv[i], arg.asChar() );
    }

    //
    // Initialize the parser.
    //
    util_getopt_init();
}

//-----------------------------------------------------------------------------
// O p t i o n P a r s e r : : ~ O p t i o n P a r s e r 
//
// Synopsis:    Destructor
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MExt::OptionParser::~OptionParser()
{
    if ( m_argv )
    {
        int i;
        for ( i = 0; i < m_argc; i++ )
        {
            if ( m_argv[i] )
            {
                delete m_argv[i];
            }
        }

        delete m_argv;
    }

    m_argc = 0;

    if ( m_opt )
    {
        delete m_opt;
        m_opt = 0;
    }
}

//-----------------------------------------------------------------------------
// O p t i o n P a r s e r : : s e t O p t i o n s 
//
// Synopsis:    Specify the options that will be parsed.
//
// Parameters:  optionSpec - the specification string.  eg. "hgu:t:" would
//                           specify two boolean flags, "h" and "g" ( they 
//                           do not take arguments ), and two argument flags,
//                           "u" and "t", that take arguments.  Flags that
//                           take arguments must be followed by a ":".
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::OptionParser::setOptions( const char* optionSpec )
{
    int len = strlen( optionSpec ) + 1;
    m_opt = new char[ len + 1 ];
    assert( m_opt );

    strncpy( m_opt, optionSpec, len );
}

//-----------------------------------------------------------------------------
// O p t i o n P a r s e r : : n e x t O p t i o n 
//
// Synopsis:    Get the next option.
//
// Parameters:  NONE
//
// Returns:     The character flag for the next option or -1 if no more 
//              options.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int MExt::OptionParser::nextOption()
{
    int result = util_getopt( m_argc, m_argv, m_opt );
    return result;
}

//-----------------------------------------------------------------------------
// O p t i o n P a r s e r : : g e t A r g 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MString MExt::OptionParser::getArg()
{
    MString result( util_optarg );
    return result;
}

//-----------------------------------------------------------------------------
// O p t i o n V a r : : G e t
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::OptionVar::Get( char* buffer, unsigned int buffer_size, const char* symbol )
{
    bool doesExist = false;

    MString command = "optionVar -exists ";
    command += symbol;

    int exists;
    MGlobal::executeCommand( command, exists );

    if ( exists )
    {
        command = "optionVar -query ";
        command += symbol;

        MString result;
        MGlobal::executeCommand( command, result );

        assert( result.length() < buffer_size );
        strncpy( buffer, result.asChar(), buffer_size );

        doesExist = true;
    }

    return doesExist;
}


//-----------------------------------------------------------------------------
// O p t i o n V a r : : S e t 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::OptionVar::Set( const char* buffer, const char* symbol )
{
    MString command;
    command = "optionVar -stringValue ";
    command += symbol;
    command += " \"";
    command += buffer;
    command += "\"";

    MGlobal::executeCommand( command );
}

//-----------------------------------------------------------------------------
// A d d C h i l d
//
// Synopsis:    Make a locator node the child of another by parenting the transforms.
//
// Parameters:  parentLocatorNode - locator node to be the parent
//              childLocatorNode  - locator node to be the child
//
// Returns:     NOTHING
//
// Constraints: Must both have transforms.
//
//-----------------------------------------------------------------------------
void MExt::AddChild( MObject& parentLocatorNode, MObject& childLocatorNode )
{
//    assert( parentLocatorNode.apiType() == MFn::kLocator );
//    assert( childLocatorNode.apiType() == MFn::kLocator );

    //Get the transform of the parent node
    MFnDagNode fnDag( parentLocatorNode );
    MObject parentTransform = fnDag.parent( 0 );

    //Get teh transform of the child node.
    fnDag.setObject( childLocatorNode );
    MObject childTransform = fnDag.parent( 0 );

    //Parent the fence to the wall
    fnDag.setObject( parentTransform );
    fnDag.addChild( childTransform );
}

//-----------------------------------------------------------------------------
// C o n n e c t 
//
// Synopsis:    Connect two nodes via the specified attributes.   
//
// Parameters:  node      - the source node.
//              attr      - the source attribute.
//              otherNode - the destination node.
//              otherAttr - the destination attribute.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::Connect( MObject& node, 
                    MObject& attr, 
                    MObject& otherNode, 
                    MObject& otherAttr
                  )
{
    MStatus status;

    MDGModifier modifier;
    status = modifier.connect( node, attr, otherNode, otherAttr );
    assert( status );

    status = modifier.doIt();
    if ( !status ) MGlobal::displayError( status.errorString() );
    assert( status );
}

void MExt::Connect( MObject& node, 
                    const char* attr,
                    MObject& otherNode,
                    const char* otherAttr
                  )
{
    MStatus status;

    MFnDependencyNode fnNode;

    fnNode.setObject( node );
    MPlug nodePlug = fnNode.findPlug( MString( attr ), &status );
    assert( status );


    fnNode.setObject( otherNode );
    MPlug otherNodePlug = fnNode.findPlug( MString( otherAttr ), &status );
    assert( status );

    MDGModifier modifier;
    status = modifier.connect( node, nodePlug.attribute(), otherNode, otherNodePlug.attribute() );
    assert( status );

    status = modifier.doIt();
    if ( !status ) MGlobal::displayError( status.errorString() );
    assert( status );
}

//-----------------------------------------------------------------------------
// C r e a t e N o d e
//
// Synopsis:
//
// Parameters:  node        - reference parameter to receive node MObject.
//              transform   - reference parameter to receive the parenting
//                            transform.
//              type        - the type of node to create.
//              name        - the name to assign to the node.  If NULL default
//                            name is used.
//              group       - MObject representing the group under which to
//                            attach the new node.  If NULL then the new node
//                            is attached at the DAG root.
//
// Returns:     the status of the request -- hopefully MS::kSuccess.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::CreateNode( MObject* node,
                       MObject* transform,
                       const MString& type,
                       const MString* name,
                       const MObject& group
                     )
{
    MStatus status;

    //
    // Determine names for the nodes.  This must be done before the 
    // nodes are created so as to avoid conflicting with the default
    // names that Maya will assign them prior to us renaming them.
    //
    MString nodeName = type;
    if ( name ) nodeName = *name;
    MString transformName = ( "TForm" + nodeName );

	MakeNameUnique( &nodeName, nodeName, group );
    MakeNameUnique( &transformName, transformName, group );

    //
    // Create the transform.
    //
    MFnTransform fnTransform;
    fnTransform.create( MObject::kNullObj, &status );
    assert( status );

    //
    // Create the node under the transform.
    //
    MFnDagNode fnNode;
    fnNode.create( type, fnTransform.object(), &status );
    assert( status );

    if ( group != MObject::kNullObj )
    {
        //
        // Place the new node under the group node.
        // 
        MFnTransform fnGroup( group, &status );
        assert( status );

        status = fnGroup.addChild( fnTransform.object() );
        assert( status );
    }

    //
    // Name the nodes.
    //
    fnTransform.setName( transformName, &status );
    assert( status );

    fnNode.setName( nodeName, &status );
    assert( status );

    //
    // Return the node and transform objects in the reference parameters.
    //
    if ( transform )
    {
        *transform = fnTransform.object();
    }

    if ( node )
    {
        *node = fnNode.object();
    }
}

//-----------------------------------------------------------------------------
// C r e a t e N o d e
//
// Synopsis:
//
// Parameters:  node        - reference parameter to receive node MObject.
//              transform   - reference parameter to receive the parenting
//                            transform.
//              type        - the type of node to create.
//              name        - the name to assign to the node.  If NULL default
//                            name is used.
//              group       - MObject representing the group under which to
//                            attach the new node.  If NULL then the new node
//                            is attached at the DAG root.
//
// Returns:     the status of the request -- hopefully MS::kSuccess.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::CreateNode( MObject& node,
                       MObject& transform,
                       const MString& type,
                       const MString* name,
                       const MObject& group
                     )
{
    MStatus status;

    //
    // Determine names for the nodes.  This must be done before the 
    // nodes are created so as to avoid conflicting with the default
    // names that Maya will assign them prior to us renaming them.
    //
    MString nodeName = type;
    if ( name ) nodeName = *name;
    MString transformName = ( "TForm" + nodeName );

	MakeNameUnique( &nodeName, nodeName, group );
    MakeNameUnique( &transformName, transformName, group );

    //
    // Create the transform.
    //
    MFnTransform fnTransform;
    fnTransform.create( MObject::kNullObj, &status );
    assert( status );

    //
    // Create the node under the transform.
    //
    MFnDagNode fnNode;
    fnNode.create( type, fnTransform.object(), &status );
    assert( status );

    if ( group != MObject::kNullObj )
    {
        //
        // Place the new node under the group node.
        // 
        MFnTransform fnGroup( group, &status );
        assert( status );

        status = fnGroup.addChild( fnTransform.object() );
        assert( status );
    }

    //
    // Name the nodes.
    //
    fnTransform.setName( transformName, &status );
    assert( status );

    fnNode.setName( nodeName, &status );
    assert( status );

    //
    // Return the node and transform objects in the reference parameters.
    //
    transform = fnTransform.object();
    node = fnNode.object();
}

//-----------------------------------------------------------------------------
// C r e a t e V e r t e x A t t r i b u t e 
//
// Synopsis:    Intended for use within the initialize() method for a node 
//              class.  Creates a vertex attribute using a double array.
//
// Parameters:  name       - the name of the attribute.
//              breif_name - the brief name of the attribute.
//
// Returns:     the attribute object.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::CreateVertexAttribute( MObject* attr,
                            const char* name, 
                            const char* briefName 
                          )
{
    MStatus status;

    MDoubleArray doubleArray;
    MFnDoubleArrayData doubleArrayData;
    MObject defaultVertex = doubleArrayData.create( doubleArray, &status );
    assert( status );

    MFnTypedAttribute fnAttribute;
    *attr = fnAttribute.create( name,
                                briefName,
                                MFnData::kDoubleArray,
                                defaultVertex,
                                &status
                              );
    assert( status );
}

//=============================================================================
// MExt::DeleteNode
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& node, bool deleteParent )
//
// Return:      void 
//
//=============================================================================
void MExt::DeleteNode( MObject& node, bool deleteParent )
{
    //Get the parent and delete it too if it's a transform and the bool says so.

    MStatus status;
    MObject parent;

    if ( deleteParent )
    {
        //Get the parent please.
        MFnDagNode fnDag( node );
        parent = fnDag.parent( 0, &status );
        assert( status );
    }

    //Delete this node
    MGlobal::deleteNode( node );

    if ( deleteParent )
    {
        //Delete the parent.
        MGlobal::deleteNode( parent );
    }
}

//-----------------------------------------------------------------------------
// D i s c o n n e c t A l l
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::DisconnectAll( MObject& node, MObject& attr )
{
    MStatus status;

    MFnDependencyNode fnNode( node, &status );
    assert( status );

    //
    // Get the plug for the attribute to be disconnected.
    //
    MPlug plug = fnNode.findPlug( attr, &status );
    assert( status );

    //
    // Find all connections and disconnect them.
    //
    MDGModifier modifier;
    MPlugArray sources;
    MPlugArray targets;
    MExt::ResolveConnections( &sources, &targets, plug, true, true );

    unsigned int count = sources.length();
    unsigned int i;

    for ( i = 0; i < count; i++ )
    {
        status = modifier.disconnect( sources[i], targets[i] );
        assert( status );
    }

    status = modifier.doIt();
    assert( status );
}  

void MExt::DisconnectAll( MObject& node, const char* attrName )
{
    MStatus status;

    MFnDependencyNode fnNode( node, &status );
    assert( status );

    //
    // Get the plug for the attribute to be disconnected.
    //
    MPlug plug = fnNode.findPlug( MString( attrName ), &status );
    assert( status );

    //
    // Find all connections and disconnect them.
    //
    MDGModifier modifier;
    MPlugArray sources;
    MPlugArray targets;
    MExt::ResolveConnections( &sources, &targets, plug, true, true );

    unsigned int count = sources.length();
    unsigned int i;

    for ( i = 0; i < count; i++ )
    {
        status = modifier.disconnect( sources[i], targets[i] );
        assert( status );
    }

    status = modifier.doIt();
    assert( status );
}

//-----------------------------------------------------------------------------
// D i s p l a y E r r o r 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int MExt::DisplayError( const char* fmt, ... )
{
    va_list argp;
    va_start( argp, fmt );
    int size = _vsnprintf( scratchpad, SCRATCHPAD_SIZE, fmt, argp );
    MGlobal::displayError( scratchpad );
    return size;
}

//-----------------------------------------------------------------------------
// D i s p l a y W a r n i n g 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int MExt::DisplayWarning( const char* fmt, ... )
{
    va_list argp;
    va_start( argp, fmt );
    int size = _vsnprintf( scratchpad, SCRATCHPAD_SIZE, fmt, argp );
    MGlobal::displayWarning( scratchpad );
    return size;
}

//-----------------------------------------------------------------------------
// D i s p l a y I n f o 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int MExt::DisplayInfo( const char* fmt, ... )
{
    va_list argp;
    va_start( argp, fmt );
    int size = _vsnprintf( scratchpad, SCRATCHPAD_SIZE, fmt, argp );
    MGlobal::displayInfo( scratchpad );
    return size;
}




//-----------------------------------------------------------------------------
// F i l t e r S e l e c t i o n L i s t 
//
// Synopsis:    Filters the given source list for nodes of "typeName" and 
//              places them in the filtered list.  If transforms are in the
//              source list all their children are filtered as well.
//
// Parameters:  filteredList - reference paremeter to receive the filtered 
//                             list.
//              typeName     - the type name to check for.
//              sourceList   - the list to filter.
//
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::FilterSelectionList( MSelectionList* filteredList,
                                const MString& typeName,
                                const MSelectionList& sourceList
                              )
{
    assert( filteredList );

    MItSelectionList it_source( sourceList );
    while( ! it_source.isDone() )
    {
        MObject node;
        it_source.getDependNode( node );

        MFnDependencyNode fnNode;
        fnNode.setObject( node );

        if ( fnNode.typeName() == typeName )
        {
            filteredList->add( fnNode.object() );
        }
        else if ( strcmp( "transform" , fnNode.typeName().asChar() ) == 0 )
        {
            SelectNodesBelowRoot( filteredList, typeName, fnNode.object() );
        }

        it_source.next();
    }
}

//=============================================================================
// MExt::FindAllSkeletonRoots
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObjectArray* objects )
//
// Return:      bool 
//
//=============================================================================
bool MExt::FindAllSkeletonRoots( MObjectArray* objects )
{
    MStatus status;
    bool returnVal = false;

    MItDag dagIt( MItDag::kDepthFirst, MFn::kTransform, &status );
    assert( status );

    while( !dagIt.isDone() )
    {
        MFnTransform fnTransform( dagIt.item() );

        MPlug p3dBoolPlug = fnTransform.findPlug( MString("p3dBooleanAttributes"), &status );
        if ( status )
        {
            //This has p3d info.
            int value = 0;
            p3dBoolPlug.getValue( value );

            if ( value & 0x0002 ) //This is the skelton root bit...  HACK
            {
                objects->append( fnTransform.object() );
                returnVal = true;
            }
        }

        dagIt.next();
    }

    return returnVal;
}

//=============================================================================
// MExt::FindAllTransforms
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObjectArray* transforms, const MObject& root )
//
// Return:      bool 
//
//=============================================================================
bool MExt::FindAllTransforms( MObjectArray* transforms, const MObject& root )
{
    bool returnVal = false;

    MItDag dagIt( MItDag::kDepthFirst, MFn::kTransform );
    MDagPath path;
    MDagPath::getAPathTo( root, path );

    dagIt.reset( path, MItDag::kDepthFirst, MFn::kTransform );

    while ( !dagIt.isDone() )
    {
        transforms->append( dagIt.item() );
        returnVal = true;

        dagIt.next();
    }

    return returnVal;
}

//-----------------------------------------------------------------------------
// F i n d D a g N o d e B y N a m e 
//
// Synopsis:    Find a node in the DAG using its name as the search key.
//
// Parameters:  path    - reference object to receive the path of the found
//                        node.
//              name    - the name to search for.
//              root    - only search under this node.
//
// Returns:     true if found, false otherwise.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::FindDagNodeByName( MDagPath* path, 
                        const MString& name,
                        const MObject& root
                      )
{
    MStatus status;

    MItDag it_dag; 
    if ( root != MObject::kNullObj )
    {
        status = it_dag.reset( root );
        if ( MS::kSuccess != status ) return false;
    }

    bool found = false;
    while ( !found && !it_dag.isDone() )
    {
        MFnDependencyNode node( it_dag.item(), &status );
        assert( status );
        if ( name == node.name() )
        {
            found = true;
            if ( path )
            {
                it_dag.getPath( *path );
            }
        }
        it_dag.next();
    }

    return found;
}

//-----------------------------------------------------------------------------
// F i n d D a g N o d e B y N a m e 
//
// Synopsis:    Find a node in the DAG using its name as the search key.
//
// Parameters:  path    - reference object to receive the path of the found
//                        node.
//              name    - the name to search for.
//              root    - only search under the node named root.
//
// Returns:     true if found, false otherwise.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::FindDagNodeByName( MDagPath* path, 
                              const MString& name, 
                              const MString& root
                            )
{
    MDagPath myPath;
    bool found = FindDagNodeByName( &myPath, root );
    if ( found )
    {
        found = FindDagNodeByName( path, name, myPath.node() );
    }

    return found;
}

//-----------------------------------------------------------------------------
// G e t W o r l d P o s i t i o n 
//
// Synopsis:    Retrieves the world position of the given node.
//
// Parameters:  wp   - reference parameter to receive the world positioin.
//              node - the node object to retrieve the world position of.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::GetWorldPosition( MPoint* wp, const MObject& node )
{
    MStatus status;

	MFnDependencyNode fnNode( node );

    //
    // Attempt to get the world matrix attribute.
    //
	MObject attrObject = fnNode.attribute( "worldMatrix", &status );
    assert( status );

    //
    // Build the world matrix plug.  Use the first element on the plug.
    //
    MPlug plug( const_cast<MObject&>(node), attrObject );
    plug = plug.elementByLogicalIndex( 0, &status );
    assert( status );

    //
	// Get the world matrix.   We have to go through a few Maya layers on this
    // one.
	//
	MObject matrixObject;
	status = plug.getValue( matrixObject );
    assert( status );

    MFnMatrixData matrixData( matrixObject, &status );
    assert( status );

    MMatrix matrix = matrixData.matrix( &status );
    assert( status );

    //
	// The translation vector of the matrix is our position.
	//
	wp->x = matrix( 3, 0 );
	wp->y = matrix( 3, 1 );
	wp->z = matrix( 3, 2 );
}


//-----------------------------------------------------------------------------
// G e t W o r l d P o s i t i o n B e t w e e n
//
// Synopsis:    Retrieves the world position  of a point between the the given nodes.
//
// Parameters:  node1 - The first node
//              node1 - The second node
//              betweenPoint - receives the point between the two nodes
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MPoint MExt::GetWorldPositionBetween( MObject& node1, MObject& node2 )
{
    MVector newWPVect;
    MPoint node1WP;
    MPoint node2WP;

    MExt::GetWorldPosition( &node1WP, node1 );
    MExt::GetWorldPosition( &node2WP, node2 );

    newWPVect = node2WP - node1WP;
    newWPVect /= 2.0f;
    newWPVect += node1WP;

    MPoint newPoint( newWPVect );
    return newPoint;
}

//=============================================================================
// MExt::GetWorldMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& node )
//
// Return:      MMatrix 
//
//=============================================================================
MMatrix MExt::GetWorldMatrix( MObject& node )
{
    MStatus status;

	MFnDependencyNode fnNode( node );

    //
    // Attempt to get the world matrix attribute.
    //
	MObject attrObject = fnNode.attribute( "worldMatrix", &status );
    assert( status );

    //
    // Build the world matrix plug.  Use the first element on the plug.
    //
    MPlug plug( const_cast<MObject&>(node), attrObject );
    plug = plug.elementByLogicalIndex( 0, &status );
    assert( status );

    //
	// Get the world matrix.   We have to go through a few Maya layers on this
    // one.
	//
	MObject matrixObject;
	status = plug.getValue( matrixObject );
    assert( status );

    MFnMatrixData matrixData( matrixObject, &status );
    assert( status );

    MMatrix matrix = matrixData.matrix( &status );
    assert( status );

    return matrix; 
}

//-----------------------------------------------------------------------------
// MExt : : I s C o n n e c t e d
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::IsConnected( MObject& node, const char* attr )
{
    MStatus status;
    MFnDependencyNode fnNode;


    fnNode.setObject( node );
    
    MPlug plug = fnNode.findPlug( MString( attr ), &status );
    assert( status );

    return plug.isConnected();
}

bool MExt::IsConnected( MObject& node, MObject& attr )
{
    MStatus status;
    MFnDependencyNode fnNode;


    fnNode.setObject( node );
    
    MPlug plug = fnNode.findPlug( attr, &status );
    assert( status );

    return plug.isConnected();
}

bool MExt::IsConnected( MPlug& plug1, MPlug& plug2 )
{
    MPlugArray plugArray;

    plug1.connectedTo( plugArray, true, true );

    unsigned int i;
    for ( i = 0; i < plugArray.length(); ++i )
    {
        if ( plugArray[i] == plug2 )
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// M a k e N a m e U n i q u e 
//
// Synopsis:    Append numerical suffixes to a name to make it unique under 
//              a specified root node.
//
// Parameters:  unique  - a reference parameter to receive the new unique name.
//              name    - the original name.
//              root    - the node under which the name must be unique.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::MakeNameUnique( MString* unique, 
                           const MString& name,
                           const MObject& root
                         )
{
    const int bufferSize = 256;

    MString myName = name;
    assert( myName.length() < bufferSize );

    if ( FindDagNodeByName( 0, myName, root ) )
    {
        char buffer[ bufferSize ];
        strncpy( buffer, myName.asChar(), bufferSize );

        //
        // Isolate the base name by removing any numerical suffixes.
        //
        char* suffix = const_cast<char*>(util_reverseSpan( buffer, "0123456789" ));
        if ( suffix )
        {
            *suffix = '\0';
        }

        myName = buffer;
        int isuffix = 0;
        while( FindDagNodeByName( 0, myName, root ) )
        {
            isuffix++;
            myName = buffer;
            myName += isuffix;  
        }

    }

    *unique = myName;
}

//-----------------------------------------------------------------------------
// M a k e N a m e U n i q u e 
//
// Synopsis:    Append numerical suffixes to a name to make it unique under 
//              a specified root node.
//
// Parameters:  unique  - a reference parameter to receive the new unique name.
//              name    - the original name.
//              root    - name of the node under which the name must be unique.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::MakeNameUnique( MString* unique,
                           const MString& name,
                           const MString& root
                         )
{
    MObject rootObject = MObject::kNullObj;
    MDagPath path;
    if ( FindDagNodeByName( &path, root ) )
    {
       rootObject = path.node();
    }

    MakeNameUnique( unique, name, rootObject );
}

//=============================================================================
// MExt::MeshClickIntersect
//=============================================================================
// Description: Comment
//
// Parameters:  ( short xClick, short yClick, MPoint& intersect, bool closest = true )
//
// Return:      bool
//
//=============================================================================
bool MExt::MeshClickIntersect( short xClick, 
                               short yClick,
                               MPoint& intersect, 
                               bool closest )
{
    //Get the mesh below the clicked point and find it's y height.
    short xStart, xEnd, yStart, yEnd;

    xStart = 0;
    xEnd = M3dView::active3dView().portWidth();
    yStart = M3dView::active3dView().portHeight();
    yEnd = 0;

    MGlobal::selectFromScreen( xStart, 
                               yStart, 
                               xEnd,
                               yEnd,
                               MGlobal::kReplaceList );

    MSelectionList selectionList;

    MGlobal::getActiveSelectionList( selectionList );

    if ( selectionList.length() > 0 )
    {
        //Go through each selected object and see if the ray intersects it.
        MItSelectionList selectIt( selectionList, MFn::kMesh );

        MPoint nearClick, farClick;
        M3dView activeView = M3dView::active3dView();
        activeView.viewToWorld( xClick, yClick, nearClick, farClick );
        MVector rayDir( MVector( farClick ) - MVector( nearClick ) );
        MPointArray intersectPoints;
        MDagPath objDag;

        bool found = false;

        MPoint resultPoint;

        if ( closest )
        {
            resultPoint.x = 100000.0;
            resultPoint.y = 100000.0;
            resultPoint.z = 100000.0;
        }
        else
        {
            resultPoint.x = 0;
            resultPoint.y = 0;
            resultPoint.z = 0;
        }

        while ( !selectIt.isDone() )
        {
            selectIt.getDagPath( objDag );

            MFnMesh mesh( objDag );

            mesh.intersect( nearClick, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

            unsigned int i;
            for ( i = 0; i < intersectPoints.length(); ++i )
            {
                //test each point...
                if ( closest )
                {
                    if ( intersectPoints[i].distanceTo(nearClick) < (resultPoint.distanceTo(nearClick) ) )
                    {
                        resultPoint = intersectPoints[i];
                        found = true;
                    }
                }
                else
                {
                    if ( intersectPoints[i].distanceTo(nearClick) > (resultPoint.distanceTo(nearClick) ) )
                    {
                        resultPoint = intersectPoints[i];
                        found = true;
                    }
                }
            }

            selectIt.next();
        }

        if ( found )
        {
            intersect = resultPoint;
            MGlobal::clearSelectionList();
            return true;
        }
    }

    MGlobal::clearSelectionList();
    return false;
}

//=============================================================================
// MExt::MeshIntersectAlongVector
//=============================================================================
// Description: Comment
//
// Parameters:  ( MPoint from, MPoint direction, MPoint& intersect, bool closest = true )
//
// Return:      bool
//
//=============================================================================
bool MExt::MeshIntersectAlongVector( MPoint from, 
                                     MPoint direction, 
                                     MPoint& intersect, 
                                     bool closest )
{
    MSelectionList selectionList;
    selectionList.clear();

    MItDag itDag(MItDag::kDepthFirst, MFn::kMesh );

    while ( !itDag.isDone() )
    {     
        MDagPath dagPath;
        itDag.getPath( dagPath );

        selectionList.add( dagPath );

        itDag.next();
    }

    if ( selectionList.length() > 0 )
    {
        //Go through each selected object and see if the ray intersects it.
        MItSelectionList selectIt( selectionList, MFn::kMesh );

        MPointArray intersectPoints;
        MDagPath objDag;

        bool found = false;

        MPoint resultPoint;

        if ( closest )
        {
            resultPoint.x = 100000.0;
            resultPoint.y = 100000.0;
            resultPoint.z = 100000.0;
        }
        else
        {
            resultPoint.x = 0;
            resultPoint.y = 0;
            resultPoint.z = 0;
        }

        while ( !selectIt.isDone() )
        {
            selectIt.getDagPath( objDag );

            MStatus status;

            MFnMesh mesh( objDag, &status );
            assert( status );


            const char* name = mesh.name().asChar();

            mesh.intersect( from, direction, intersectPoints, 0.001f, MSpace::kWorld );

            unsigned int i;
            for ( i = 0; i < intersectPoints.length(); ++i )
            {
                //test each point...
                if ( closest )
                {
                    if ( intersectPoints[i].distanceTo(from) < (resultPoint.distanceTo(from) ) )
                    {
                        resultPoint = intersectPoints[i];
                        found = true;
                    }
                }
                else
                {
                    if ( intersectPoints[i].distanceTo(from) > (resultPoint.distanceTo(from) ) )
                    {
                        resultPoint = intersectPoints[i];
                        found = true;
                    }
                }
            }

            selectIt.next();
        }

        if ( found )
        {
            intersect = resultPoint;
            MGlobal::clearSelectionList();
            return true;
        }
    }

    MGlobal::clearSelectionList();
    return false;
}

//-----------------------------------------------------------------------------
// P l u g H a s C o n n e c t i o n  
//
// Synopsis:    Determines if there are any connections on the specified plug.
//
// Parameters:  connectedNode   - reference parameter to receive the associated
//                                node for the first connection found.
//              plug            - the plug to get the connections for.
//              asSrc           - if true, retrieve connections where "plug" is
//                                the source of the connection.
//              asDst           - if true, retrieve connections where "plug" is
//                                the target of the connection.
//
// Returns:     true, if the plug has connections; false, otherwise
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::PlugHasConnection( MObject* connectedNode, 
                              MPlug& plug,
                              bool asDst,
                              bool asSrc,
                              const char* type
                            )
{
    bool isOk = false;

    MPlug myPlug;
    isOk = PlugHasConnection( &myPlug, plug, asDst, asSrc, type );

    if ( isOk )
    {
        if ( connectedNode ) *connectedNode = myPlug.node();
    }

    return isOk;
}

//-----------------------------------------------------------------------------
// P l u g H a s C o n n e c t i o n  
//
// Synopsis:    Determines if there are any connections on the specified plug.
//
// Parameters:  connectedPlug   - reference parameter to receive the plug of
//                                the first connection found.
//              plug            - the plug to get the connections for.
//              asSrc           - if true, retrieve connections where "plug" is
//                                the source of the connection.
//              asDst           - if true, retrieve connections where "plug" is
//                                the target of the connection.
//
// Returns:     true, if the plug has connections; false, otherwise
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool MExt::PlugHasConnection( MPlug* connectedPlug, 
                              MPlug& plug,
                              bool asDst,
                              bool asSrc,
                              const char* type
                            )
{
    MPlugArray buffer;

    if ( asSrc )
    {
        MPlugArray destinations;
        ResolveConnections( &buffer, &destinations, plug, false, true );
        
        unsigned int i;
        for( i = 0; i < destinations.length(); i++ )
        {
            bool isOk = true;
            if ( type )
            {
                MFnDependencyNode fnNode( destinations[i].node() );
                if ( fnNode.typeName() != type )
                {
                    isOk = false;
                }
            }

            if ( isOk )
            {
                if ( connectedPlug ) *connectedPlug = destinations[i];
                return true;
            }
        }
    }

    if ( asDst )
    {
        MPlugArray sources;
        ResolveConnections( &sources, &buffer, plug, true, false );

        unsigned int i;
        for( i = 0; i < sources.length(); i++ )
        {
            bool isOk = true;
            if ( type )
            {
                MFnDependencyNode fnNode( sources[i].node() );
                if ( fnNode.typeName() != type )
                {
                    isOk = false;
                }
            }

            if ( isOk )
            {
                if ( connectedPlug ) *connectedPlug = sources[i];
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// R e s o l v e C o n n e c t i o n s
//
// Synopsis:    Retrieves the plugs that are connected to this plug.  This
//              will work on plugs that are associated with array as well as
//              non-array attributes.
//
// Parameters:  sources     - reference parameter to recieve list of 
//                            connection sources.
//              targets     - reference parameter to recieve list of
//                            connection targets.
//              plug        - the plug to get the connections for.
//              asSrc       - if true, retrieve connections where "plug" is
//                            the source of the connection.
//              asDst       - if true, retrieve connections where "plug" is
//                            the target of the connection.
//
// Returns:     true, if the plug has connections; false, otherwise
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::ResolveConnections( MPlugArray* sources,
                               MPlugArray* targets, 
                               MPlug& plug, 
                               bool asDst,
                               bool asSrc
                             )
{
    assert( sources );
    assert( targets );

    MStatus status;
    MPlugArray myPlugs;
    unsigned int count = 0;
    unsigned int i = 0;

    if ( plug.isArray() )
    {
        count = plug.numElements( &status );
        assert( status );

        for ( i = 0; i < count; i++ )
        {
            MPlug element = plug.elementByPhysicalIndex( i, &status );
            assert( status );

            if ( element.isConnected() )
            {

                MString name = element.name();
                const char* dbg_name = name.asChar();

                myPlugs.append( element );
            }
        }
    }
    else
    {
        myPlugs.append( plug );
    }

    sources->clear();
    targets->clear();
    count = myPlugs.length();
    for ( i = 0; i < count; i++ )
    {
        MPlugArray connectedPlugs;

        if ( asDst )
        {
            myPlugs[i].connectedTo( connectedPlugs, true, false, &status );
            assert( status );

            if ( connectedPlugs.length() > 0 )
            {
                sources->append( connectedPlugs[0] );
                targets->append( myPlugs[i] );
            }
        }

        if ( asSrc )
        {
            myPlugs[i].connectedTo( connectedPlugs, false, true, &status );
            assert( status );

            if ( connectedPlugs.length() > 0 )
            {
                sources->append( myPlugs[i] );
                targets->append( connectedPlugs[0] );
            }
        }
    }

    assert( sources->length() == targets->length() );
}

//-----------------------------------------------------------------------------
// S e l e c t N o d e s B e l o w R o o t
//
// Synopsis:    Select nodes meeting the specified criteria and place them
//              in the provided selection list.
//
// Parameters:  list   - the list to receive the nodes.
//              typeId - the typeId of the node type to select.
//              root   - the root of the subsection of the DAG to search.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::SelectNodesBelowRoot( MSelectionList* list, 
                                 const MString& typeName, 
                                 const MObject& root,
                                 MSelectionList* intersectionList
                               )
{
    assert( list );

    MStatus status;

    bool mergeWithExisting = true;

    MItDag itor;

    if ( root != MObject::kNullObj )
    {
        itor.reset( root );
    }
            
    while( ! itor.isDone() )
    {
        MFnDependencyNode fnNode;
        fnNode.setObject( itor.item() );

        if ( fnNode.typeName() == typeName )
        {
            bool doAdd = true;

            if ( intersectionList )
            {
                MDagPath path;
                status = MDagPath::getAPathTo( fnNode.object(), path );

                //
                // We only add the item if it is in the intersectionList.
                //
                if ( ! intersectionList->hasItem( path ) )
                {
                    doAdd = false;
                }
            }

            if ( doAdd )
            {
                list->add( fnNode.object(), mergeWithExisting );
            }
        }

        itor.next();
    }
}

//-----------------------------------------------------------------------------
// S e t W o r l d P o s i t i o n 
//
// Synopsis:    Sets the world position of the given node.
//
// Parameters:  wp   - the new world position.
//              node - the node for which to set the position.
//
// Returns:     Hopefully MS::kSuccess, but other MS::????? errors will occur
//              if the object is not a suitable type of node.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::SetWorldPosition( const MPoint& wp, const MObject& node )
{
    MStatus status;

	MFnDependencyNode fnNode( node );

    //
    // Attempt to get the world matrix attribute.
    //
	MObject attrObject = fnNode.attribute( "worldMatrix", &status );
    assert( status );

    //
    // Build the world matrix plug.  Use the first element on the plug.
    //
    MPlug plug( const_cast<MObject&>(node), attrObject );
    plug = plug.elementByLogicalIndex( 0, &status );
    assert( status );

    //
	// Get the world matrix.   We have to go through a few Maya layers on this
    // one.
	//
	MObject matrixObject;
	status = plug.getValue( matrixObject );
    assert( status );

    MFnMatrixData matrixData( matrixObject, &status );
    assert( status );

    //
    // Create a world tranformation matrix.
    //
    MTransformationMatrix matrix( matrixData.matrix( &status ) );
    assert( status );
    
    //
    // Get the world translation vector.
    //
    MVector worldTranslation = matrix.translation( MSpace::kWorld, &status );


    //
    // Get the nodes immediate transform and create a function wrapper for it.
    //
    MDagPath nodePath;
    status = MDagPath::getAPathTo( node, nodePath );
    assert( status );

    MObject transformObject = nodePath.transform( &status );
    assert( status );

    MFnTransform fnTransform( transformObject, &status );
    assert( status );

    //
    // Get the node translation vector.
    //
    MVector nodeTranslation = fnTransform.translation( MSpace::kTransform, &status );

    //
    // The exclusive translation vector is that vector which reflect the 
    // amount of translation the node undergoes as a result of transforms
    // exclusive of its immediate parent.
    //
    MVector exclusiveTranslation = worldTranslation - nodeTranslation; 

    //
    // Set the nodeTranslation to that or our desired world position less the
    // exclusiveTranslation vector.
    //
    MVector position( wp );
    nodeTranslation = position - exclusiveTranslation;
    
    //
    // Push the result back into the transform and we are done.
    //
    status = fnTransform.setTranslation( nodeTranslation, MSpace::kTransform );
    assert( status );

}

//-----------------------------------------------------------------------------
// v i e w T o W o r l d A t Y 
//
// Synopsis:    Convert the specified view coordinates to world coordinates on
//              the specified y plane.
//
// Parameters:  world   - reference parameter to recieve the world coordinates.
//              view    - the view position to be converted.
//              y       - the y plane to translate to.
//
// Returns:    The status of the request.  Will return failure if the the 
//             view plane is perpendicular to the y-plane.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MExt::ViewToWorldAtY( MPoint* wc, MPoint& vc, double y )
{
    assert( wc );

    MStatus status = MS::kFailure;
    
    M3dView view = M3dView::active3dView();
    MPoint rayOrigin;
    MVector rayVector;
    status = view.viewToWorld( static_cast<short>(vc.x),
                               static_cast<short>(vc.y),
                               rayOrigin,
                               rayVector
                             );
    assert( status );

    MPoint result;
    if ( fabs(rayVector.y) > EPSILON )
    {
        //
        // The following formulas for x and z use the point slope formula in 
        // the form
        //      x = ( y - y0 ) / M   +   x0
        //        = ( y - y0 ) / ( dy / dx ) + x0
        //        = ( ( y - y0 ) / dy ) * dx   +  x0
        //
        double coeff = ( y - rayOrigin.y ) / rayVector.y;
        wc->x = ( coeff * rayVector.x ) + rayOrigin.x;
        wc->y = y;
        wc->z = ( coeff * rayVector.z ) + rayOrigin.z;
    }
}
