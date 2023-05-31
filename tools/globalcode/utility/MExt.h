#include "precompiled/PCH.h"

#ifndef _MEXT_H
#define _MEXT_H
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MExt.h
//
// Description: Functions that extend the Maya API to perform other common 
//              tasks.
//
// Modification History:
//  + Created Aug 21, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------
/* Using precompiled headers.
#include <maya/MStringArray.h>
#include <maya/MPoint.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
*/

//----------------------------------------
// Project Includes
//----------------------------------------
#include "MExt_template.h"

//----------------------------------------
// Forward References
//----------------------------------------
class MString;
class MDagPath;
class MArgList;
class MPlug;
class MObject;
class MSelectionList;
class MTypeId;
class MDoubleArray;

//----------------------------------------
// Macros
//----------------------------------------
#define RETURN_STATUS_ON_FAILURE( STATUS ) if ( ! (STATUS) ) return (STATUS)
#define RETURN_FALSE_ON_FAILURE( STATUS ) if ( ! (STATUS) ) return false

//These are used when dealing with plugs.
#define AS_DEST true, false
#define AS_SOURCE false, true
#define AS_BOTH true, true
//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------



namespace MExt
{
    namespace Attr
    {
        void GetScaled( MPoint*, const MObject& node, const MObject& attr );

        void SetScaled( const MPoint&, MObject& node, MObject& attr );

        void SetScaled( const MPoint&, MObject& node, const MString& attr );

    } // namespace Attr

    namespace OptionVar
    {
        bool Get( char* buffer, unsigned int buffer_size, const char* symbol );

        void Set( const char* buffer, const char* symbol );

    } // namespace OptionVar


    class OptionParser
    {
        public:
        
            OptionParser( const char* command, const MArgList& args );
            ~OptionParser();

            void    setOptions( const char* optionSpec );
            int     nextOption();
            MString getArg();

        private:

            int m_argc;
            char** m_argv;
            char* m_opt;

    };


    void    AddChild( MObject& parentLocatorNode, MObject& childLocatorNode );

    void    Connect( MObject& node, 
                     MObject& attr,
                     MObject& otherNode,
                     MObject& otherAttr
                   );

    void    Connect( MObject& node, 
                     const char* attr,
                     MObject& otherNode,
                     const char* otherAttr
                   );
    

    void    CreateNode( MObject* node,
                        MObject* transform,
                        const MString& type,
                        const MString* name = 0,
                        const MObject& parent = MObject::kNullObj
                      );

    void    CreateNode( MObject& node,
                        MObject& transform,
                        const MString& type,
                        const MString* name = 0,
                        const MObject& parent = MObject::kNullObj
                      );


    void    CreateVertexAttribute( MObject* attr,
                                   const char* name,
                                   const char* brief_name
                                 );   

    void    DeleteNode( MObject& node, bool deleteParent );
                                 
    void    DisconnectAll( MObject& node, MObject& attr );   
    void    DisconnectAll( MObject& node, const char* attrName );

    int     DisplayError( const char* fmt, ... );

    int     DisplayWarning( const char* fmt, ... );

    int     DisplayInfo( const char* fmt, ... );
        
    void    FilterSelectionList( MSelectionList* filteredList,
                                 const MString& typeName,
                                 const MSelectionList& sourceList
                               );

    bool    FindAllSkeletonRoots( MObjectArray* objects );

    bool    FindAllTransforms( MObjectArray* transforms, const MObject& root );


    bool    FindDagNodeByName( MDagPath* path,
                               const MString& node,
                               const MObject& root = MObject::kNullObj
                             );

    bool    FindDagNodeByName( MDagPath* path,
                               const MString& node, 
                               const MString& root
                             );

    void    GetWorldPosition( MPoint*, const MObject& node );

    MPoint  GetWorldPositionBetween( MObject& node1, MObject& node2 );

    MMatrix GetWorldMatrix( MObject& node );

    bool    IsConnected( MObject& node, const char* attr );
    bool    IsConnected( MObject& node, MObject& attr );
    bool    IsConnected( MPlug& plug1, MPlug& plug2 );

    void    MakeNameUnique( MString* unique,
                            const MString& name,
                            const MObject& root = MObject::kNullObj
                          );

    void    MakeNameUnique( MString* unique,
                            const MString& name,
                            const MString& root
                          );

    bool    MeshClickIntersect( short xClick, short yClick, MPoint& intersect, bool closest = true );

    bool    MeshIntersectAlongVector( MPoint from, MPoint direction, MPoint& intersect, bool closest = true );

    bool    PlugHasConnection( MObject* connectedNode,
                               MPlug& plug,
                               bool asDst = true,
                               bool asSrc = true,
                               const char* type = 0
                             );

    bool    PlugHasConnection( MPlug* connectedPlug,
                               MPlug& plug,
                               bool asDst = true,
                               bool asSrc = true,
                               const char* type = 0
                             );


    void    ResolveConnections( MPlugArray* sources, 
                                MPlugArray* targets,
                                MPlug& plug,
                                bool asDst = true,
                                bool asSrc = true
                              );


    void    SelectNodesBelowRoot( MSelectionList* list,
                                  const MString& typeName,
                                  const MObject& root = MObject::kNullObj,
                                  MSelectionList* intersectionList = 0
                                );

    void    SetWorldPosition( const MPoint&, const MObject& node );

    void    ViewToWorldAtY( MPoint* world, MPoint& view, double y = 0.0  );

} // namespace MExt

#endif
