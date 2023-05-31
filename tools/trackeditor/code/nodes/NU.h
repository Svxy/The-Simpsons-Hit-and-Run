#include "precompiled/PCH.h"

#ifndef NODE_UTIL_H
#define NODE_UTIL_H

namespace NODE_UTIL
{
    inline void DisableAttributes( MObject& node, bool justY = true )
    {
        MFnDagNode fnDagNode( node );

        MObject parent = fnDagNode.parent( 0 );
        MFnDependencyNode fnParent( parent );

        if ( justY )
        {
            MPlug ptyPlug = fnParent.findPlug( MString( "translateY" ) );
            ptyPlug.setLocked( true );
        }
        else
        {
            MPlug ptPlug = fnParent.findPlug( MString( "translate" ) );
            ptPlug.setLocked( true );
        }

        MPlug spPlug = fnParent.findPlug( MString( "scale" ) );
        spPlug.setLocked( true );

        MPlug rpPlug = fnParent.findPlug( MString( "rotate" ) );
        rpPlug.setLocked( true );
    }
}
 
#endif