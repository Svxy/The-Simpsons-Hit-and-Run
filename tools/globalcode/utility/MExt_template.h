#include "precompiled/PCH.h"

#ifndef _MEXT_TEMPLATE_H
#define _MEXT_TEMPLATE_H
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MExt_template.h
//
// Description: 
//
// Modification History:
//  + Created Dec 19, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//This is a warning provided by the STL...  It seems that toollib gets whacky when there 
//is other templates made...  Sigh...
#pragma warning(disable:4786)

//----------------------------------------
// System Includes
//----------------------------------------
//#include <assert.h>

/* Using precompiled headers
#include <maya/MDoubleArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnStringArrayData.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
*/

//----------------------------------------
// Project Includes
//----------------------------------------

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

namespace MExt
{
    namespace Attr
    {

        template < class T >
        void Get( T* value, const MPlug& plug )
        {
            assert( value );

            MStatus status;

            status = plug.getValue( *value );
            assert( status );
        }

        template < >
        void Get< MDoubleArray >( MDoubleArray*, const MPlug& );

        template < >
        void Get< MIntArray >( MIntArray*, const MPlug& );

        template < >
        void Get< MPoint >( MPoint*, const MPlug& );

        template < >
        void Get< MStringArray >( MStringArray*, const MPlug& );

        template < class T >
        void Get( T* value, const MObject& node, const MObject& attr )
        {
            MPlug plug( node, attr );
            Get( value, plug );
        }

        template < class T >
        void Get( T* value, const MObject& node, const MString& attr )
        {
            MStatus status;

            MFnDependencyNode fnNode( node, &status );
            assert( status );

            MPlug plug = fnNode.findPlug( attr, &status );
            assert( status );

            Get( value, plug );
        }

        template < class T >
        void Set( const T& value, MPlug& plug )
        {
            MStatus status;

            status = plug.setValue( const_cast<T&>(value) );
            assert( status );
        }

        template < >
        void Set< MDoubleArray >( const MDoubleArray& array, MPlug& plug );

        template < >
        void Set< MIntArray >( const MIntArray& array, MPlug& plug );

        template < >
        void Set< MPoint >( const MPoint& vertex, MPlug& plug );

        template < >
        void Set< MStringArray >( const MStringArray& array, MPlug& plug );

        template < class T >
        void Set( const T& value, MObject& node, MObject& attr )
        {
            MPlug plug( node, attr );
            Set( value, plug );
        }

        template < class T >
        void Set( const T& value, MObject& node, const MString& attr )
        {
            MStatus status;

            MFnDependencyNode fnNode( node, &status );
            assert( status );

            MPlug plug = fnNode.findPlug( attr, &status );
            assert( status );

            Set( value, plug );
        }

    } // namespace Attr

} // namespace MExt

#endif
