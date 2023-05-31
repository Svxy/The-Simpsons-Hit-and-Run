#ifndef NODE_HELPERS

#include "precompiled/PCH.h"

namespace NodeHelper
{

inline int OverrideNodeColour( MObject& node, int defaultColour )
{
    int colour = defaultColour;

    MStatus status;
    MFnDagNode fnDag( node );
    MPlug overColour = fnDag.findPlug( MString( "overrideColor" ), &status );
    if ( status )
    {
        overColour.getValue( colour );
        if ( colour != 0 ) //The override is Active...
        {
            colour--;
            if ( colour < 0 )
            {
                colour = 0;
            }
        }
        else
        {
            colour = defaultColour;
        }
    }

    return colour;
}

};

#endif