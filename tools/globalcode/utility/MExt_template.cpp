#include "precompiled/PCH.h"

//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MExt_template.cpp
//
// Description: 
//
// Modification History:
//  + Created Dec 19, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------

//----------------------------------------
// Project Includes
//----------------------------------------
#include "MExt_template.h"

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

//-----------------------------------------------------------------------------
// G e t < MDoubleArray > 
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
template < >
void MExt::Attr::Get< MDoubleArray >( MDoubleArray* array, const MPlug& plug )
{
    assert( array );

    MStatus status;
    
    MObject data;
    status = plug.getValue( data );
    assert( status );

    MFnDoubleArrayData fnArray( data, &status );
    assert( status );

    MDoubleArray myArray = fnArray.array( &status );
    assert( status );

    array->clear();
    unsigned int i;
    for ( i = 0; i < myArray.length(); i++ )
    {
        array->append( myArray[i] );
    }
}

//-----------------------------------------------------------------------------
// G e t < MIntArray > 
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
template < >
void MExt::Attr::Get< MIntArray >( MIntArray* array, const MPlug& plug )
{
    assert( array );

    MStatus status;
    
    MObject data;
    status = plug.getValue( data );
    assert( status );

    MFnIntArrayData fnArray( data, &status );
    assert( status );

    MIntArray myArray = fnArray.array( &status );
    assert( status );

    array->clear();
    unsigned int i;
    for ( i = 0; i < myArray.length(); i++ )
    {
        array->append( myArray[i] );
    }
}
//-----------------------------------------------------------------------------
// G e t < MPoint > 
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
template < >
void MExt::Attr::Get< MPoint >( MPoint* vertex, const MPlug& plug )
{
    assert( vertex );

    MDoubleArray coordinates;
    Get( &coordinates, plug );

    if ( coordinates.length() > 0 )
    {
        assert( coordinates.length() == 3 );
        vertex->x = coordinates[0];
        vertex->y = coordinates[1];
        vertex->z = coordinates[2];
    }
    else
    {
        *vertex = MPoint::origin;
    }
}

//-----------------------------------------------------------------------------
// G e t < MStringArray > 
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
template < >
void MExt::Attr::Get< MStringArray >( MStringArray* array, const MPlug& plug )
{
    assert( array );

    MStatus status;
    
    MObject data;
    status = plug.getValue( data );
    assert( status );

    MFnStringArrayData fnArray( data, &status );
    assert( status );

    MStringArray myArray = fnArray.array( &status );
    assert( status );

    array->clear();
    unsigned int i;
    for ( i = 0; i < myArray.length(); i++ )
    {
        array->append( myArray[i] );
    }
}

//-----------------------------------------------------------------------------
// S e t < MDoubleArray > 
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
template < >
void MExt::Attr::Set< MDoubleArray >( const MDoubleArray& array, MPlug& plug )
{
    MStatus status;
    
    MFnDoubleArrayData fnArray;
    MObject object = fnArray.create( array, &status );
    assert( status );

    status = plug.setValue( object );
    assert( status );
}

//-----------------------------------------------------------------------------
// S e t < MIntArray > 
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
template < >
void MExt::Attr::Set< MIntArray >( const MIntArray& array, MPlug& plug )
{
    MStatus status;
    
    MFnIntArrayData fnArray;
    MObject object = fnArray.create( array, &status );
    assert( status );

    status = plug.setValue( object );
    assert( status );
}

//-----------------------------------------------------------------------------
// S e t < MPoint > 
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
template < >
void MExt::Attr::Set< MPoint >( const MPoint& vertex, MPlug& plug )
{
    MStatus status;

    MDoubleArray coordinates;
    coordinates.append( vertex.x );
    coordinates.append( vertex.y );
    coordinates.append( vertex.z );

    Set( coordinates, plug );
}

//-----------------------------------------------------------------------------
// S e t < MStringArray > 
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
template < >
void MExt::Attr::Set< MStringArray >( const MStringArray& array, MPlug& plug )
{
    MStatus status;
    
    MFnStringArrayData fnArray;
    MObject object = fnArray.create( array, &status );
    assert( status );

    status = plug.setValue( object );
    assert( status );
}

