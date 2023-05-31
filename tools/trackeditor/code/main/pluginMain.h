#include "precompiled/PCH.h"

//----------------------------------------
// MACROS
//----------------------------------------

#define REGISTER_COMMAND( p, c ) if ( ! ( ( p ).registerCommand( c##::stringId, \
                                                                 c##::creator ) \
                                        ) \
                                    ) return MS::kFailure

#define REGISTER_CONTEXT( p, c ) if ( ! ( ( p ).registerContextCommand( c##::stringId, \
                                                                        c##Cmd::creator ) \
                                        ) \
                                    ) return MS::kFailure


#define REGISTER_LOCATOR( p, n ) if ( ! ( ( p ).registerNode( n##::stringId, \
                                                              n##::id, \
                                                              n##::creator, \
                                                              n##::initialize, \
                                                              MPxNode::kLocatorNode ) \
                                      ) \
                                 ) return MS::kFailure

#define REGISTER_NODE( p, n ) if ( ! ( ( p ).registerNode( n##::stringId, \
                                                           n##::id, \
                                                           n##::creator, \
                                                           n##::initialize ) \
                                      ) \
                                 ) return MS::kFailure

#define REGISTER_SHAPE( p, n ) if ( ! ( ( p ).registerShape( n##::stringId, \
                                                             n##::id, \
                                                             n##::creator, \
                                                             n##::initialize, \
                                                             n##UI::creator ) \
                                       ) \
                                  ) return MS::kFailure
                                      
#define DEREGISTER_COMMAND( p, c ) ( p ).deregisterCommand( c##::stringId )

#define DEREGISTER_CONTEXT( p, c ) ( p ).deregisterContextCommand( c##::stringId )


#define DEREGISTER_NODE( p, n ) ( p ).deregisterNode( n##::id )

