#ifndef TREELINE_SHAPE_NODE_H
#define TREELINE_SHAPE_NODE_H

#include "precompiled/PCH.h"

class tlDataChunk;

namespace TETreeLine
{

class p3dBaseShape : public MPxSurfaceShape
{
public:
    p3dBaseShape() {}
    virtual ~p3dBaseShape() {} 
};

class p3dBaseShapeUI : public MPxSurfaceShapeUI
{
public:
    p3dBaseShapeUI()
        {
            lead_color = 18;  //green
            active_color = 15; //white
            active_affected_color = 8; //purple
            dormant_color = 4; //blue
            hilite_color = 17; //pale blue
        }
    virtual ~p3dBaseShapeUI() {}

    virtual void   getDrawRequestsWireframe( MDrawRequest&, const MDrawInfo& );
    virtual void   getDrawRequestsShaded(    MDrawRequest&, const MDrawInfo&, MDrawRequestQueue&, MDrawData& data );

protected:
    int lead_color;
    int active_color;
    int active_affected_color;
    int dormant_color;
    int hilite_color;
};

/////////////////////////////////////////////////////////////////////
//
// Shape class - defines the non-UI part of a shape node
//
class TreelineShapeNode : public p3dBaseShape
{
public:
    TreelineShapeNode();
    virtual ~TreelineShapeNode(); 

    virtual void            postConstructor();
    virtual MStatus         compute( const MPlug& plug, MDataBlock& datablock );
    virtual MStatus         connectionMade ( const MPlug &plug, const MPlug &otherPlug, bool asSrc );
    static  void*           creator();
    static  MStatus         initialize();
    virtual bool            isBounded() const {return true;}
    virtual MBoundingBox    boundingBox() const; 

    virtual void            componentToPlugs( MObject& component, MSelectionList& selectionList ) const;
    virtual MPxSurfaceShape::MatchResult 
                            matchComponent( const MSelectionList& item, 
                                            const MAttributeSpecArray& spec, 
                                            MSelectionList& list );
    virtual bool match( const MSelectionMask& mask, 
                        const MObjectArray& componentList ) const;
    virtual void transformUsing( const MMatrix& mat, const MObjectArray& componentList );
    virtual void closestPoint ( const MPoint & toThisPoint, MPoint & theClosestPoint, double tolerance );

    static void SnapTreeline( MObject& treeline );

    static void ConvertToGeometry( MObject& obj );

    static MTypeId id;
    static const char* stringId;

private:
    // Attributes
    static const char* SHADER_NAME_LONG;
    static const char* SHADER_NAME_SHORT;
    static MObject sShader;

    static const char* USCALE_NAME_LONG;
    static const char* USCALE_NAME_SHORT;
    static MObject sUScale;

    static const char* COLOUR_NAME_LONG;
    static const char* COLOUR_NAME_SHORT;
    static MObject sColour;

    static const char* RED_NAME_LONG;
    static const char* RED_NAME_SHORT;
    static MObject sRed;
    
    static const char* GREEN_NAME_LONG;
    static const char* GREEN_NAME_SHORT;
    static MObject sGreen;

    static const char* BLUE_NAME_LONG;
    static const char* BLUE_NAME_SHORT;
    static MObject sBlue;
    
    static const char* ALPHA_NAME_LONG;
    static const char* ALPHA_NAME_SHORT;
    static MObject sAlpha;

    static const char* HEIGHT_NAME_LONG;
    static const char* HEIGHT_NAME_SHORT;
    static MObject sHeight;
};

/////////////////////////////////////////////////////////////////////
//
// UI class - defines the UI part of a shape node
//
class TreelineShapeNodeUI : public p3dBaseShapeUI
{
public:
    TreelineShapeNodeUI();
    virtual ~TreelineShapeNodeUI(); 

    virtual void   getDrawRequests( const MDrawInfo & info, bool objectAndActiveOnly, MDrawRequestQueue & requests );
    virtual void   draw( const MDrawRequest & request, M3dView & view ) const;
    virtual bool   select( MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts ) const;
    static  void * creator();
   
protected:
    void drawQuad(int drawMode) const;
    void drawVertices( const MDrawRequest & request, M3dView & view ) const;
    bool selectVertices( MSelectInfo &selectInfo, 
                         MSelectionList &selectionList, 
                         MPointArray &worldSpaceSelectPts ) const;
};


}  //namespace TETreeLine



#endif //TREELINE_SHAPE_NODE_H


