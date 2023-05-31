#include "precompiled/PCH.h"

/*===========================================================================
    transformmatrix.hpp
    Created: October 16, 2000
    Auuthor: Bryan Brandt

    Copyright (c) 2000 Radical Entertainment, Inc.
    All rights reserved.
===========================================================================*/
#ifndef _TRANSFORMMATRIX_HPP
#define _TRANSFORMMATRIX_HPP

#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif
#include <toollib.hpp>


class MObject;
class MDagPath;
class MFnTransform;
class MFnIkJoint;
class MTransformationMatrix;


class TransformMatrix
{
public:

    enum RotateOrder
    {
        ORDER_XYZ = 0,
        ORDER_YZX,
        ORDER_ZXY,
        ORDER_XZY,
        ORDER_YXZ,
        ORDER_ZYX
    };

    TransformMatrix();
    TransformMatrix(const MDagPath& dagPath);
    TransformMatrix(const MTransformationMatrix& xform);

    bool Assign(const MDagPath& dagPath);
    bool Assign(const MTransformationMatrix& xform);

    void SetScalePivot(float x, float y, float z);
    void SetScalePivot(const tlPoint& sp);
    void SetScalePivotX(float x);
    void SetScalePivotY(float y);
    void SetScalePivotZ(float z);
    const tlPoint& GetScalePivot() const
        { return scalePivot; }
    float GetScalePivotX() const
        { return scalePivot.x; }
    float GetScalePivotY() const
        { return scalePivot.y; }
    float GetScalePivotZ() const
        { return scalePivot.z; }

    void SetScale(float x, float y, float z);
    void SetScaleX(float x);
    void SetScaleY(float y);
    void SetScaleZ(float z);
    float GetScaleX() const
        { return scaleX; }
    float GetScaleY() const
        { return scaleY; }
    float GetScaleZ() const
        { return scaleZ; }

    void SetShear(float xy, float xz, float yz);
    void SetShearXY(float xy);
    void SetShearXZ(float xz);
    void SetShearYZ(float yz);
    float GetShearXY() const
        { return shearXY; }
    float GetShearXZ() const
        { return shearXZ; }
    float GetShearYZ() const
        { return shearYZ; }

    void SetScalePivotTranslate(float x, float y, float z);
    void SetScalePivotTranslate(const tlPoint& spt);
    void SetScalePivotTranslateX(float x);
    void SetScalePivotTranslateY(float y);
    void SetScalePivotTranslateZ(float z);
    const tlPoint& GetScalePivotTranslate() const
        { return scalePivotTranslate; }
    float GetScalePivotTranslateX() const
        { return scalePivotTranslate.x; }
    float GetScalePivotTranslateY() const
        { return scalePivotTranslate.y; }
    float GetScalePivotTranslateZ() const
        { return scalePivotTranslate.z; }

    void SetRotatePivot(float x, float y, float z);
    void SetRotatePivot(const tlPoint& rp);
    void SetRotatePivotX(float x);
    void SetRotatePivotY(float y);
    void SetRotatePivotZ(float z);
    const tlPoint& GetRotatePivot() const
        { return rotatePivot; }
    float GetRotatePivotX() const
        { return rotatePivot.x; }
    float GetRotatePivotY() const
        { return rotatePivot.y; }
    float GetRotatePivotZ() const
        { return rotatePivot.z; }

    void SetRotateAxis(float x, float y, float z);
    void SetRotateAxisX(float rax);
    void SetRotateAxisY(float ray);
    void SetRotateAxisZ(float raz);
    float GetRotateAxisX() const
        { return rotateAxisX; }
    float GetRotateAxisY() const
        { return rotateAxisY; }
    float GetRotateAxisZ() const
        { return rotateAxisZ; }

    void SetRotateOrder(RotateOrder ro);
    RotateOrder GetRotateOrder() const
        { return rotateOrder; }

    void SetRotate(float x, float y, float z);
    void SetRotateX(float x);
    void SetRotateY(float y);
    void SetRotateZ(float z);
    float GetRotateX() const
        { return rotateX; }
    float GetRotateY() const
        { return rotateY; }
    float GetRotateZ() const
        { return rotateZ; }

    void SetRotatePivotTranslate(float x, float y, float z);
    void SetRotatePivotTranslate(const tlPoint& rpt);
    void SetRotatePivotTranslateX(float x);
    void SetRotatePivotTranslateY(float y);
    void SetRotatePivotTranslateZ(float z);
    const tlPoint& GetRotatePivotTranslate() const
        { return rotatePivotTranslate; }
    float GetRotatePivotTranslateX() const
        { return rotatePivotTranslate.x; }
    float GetRotatePivotTranslateY() const
        { return rotatePivotTranslate.y; }
    float GetRotatePivotTranslateZ() const
        { return rotatePivotTranslate.z; }

    void SetJointOrient(float x, float y, float z);
    void SetJointOrientX(float x);
    void SetJointOrientY(float y);
    void SetJointOrientZ(float z);
    float GetJointOrientX() const
        { return jointOrientX; }
    float GetJointOrientY() const
        { return jointOrientY; }
    float GetJointOrientZ() const
        { return jointOrientZ; }

    void SetInverseScale(float x, float y, float z);
    void SetInverseScaleX(float x);
    void SetInverseScaleY(float y);
    void SetInverseScaleZ(float z);
    float GetInverseScaleX() const
        { return inverseScaleX; }
    float GetInverseScaleY() const
        { return inverseScaleY; }
    float GetInverseScaleZ() const
        { return inverseScaleZ; }

    void SetTranslate(float x, float y, float z);
    void SetTranslate(const tlPoint& t);
    void SetTranslateX(float x);
    void SetTranslateY(float y);
    void SetTranslateZ(float z);
    const tlPoint& GetTranslate() const
        { return translate; }
    float GetTranslateX() const
        { return translate.x; }
    float GetTranslateY() const
        { return translate.y; }
    float GetTranslateZ() const
        { return translate.z; }

    void SetScaleMatrixIdentity();
    void SetHierarchyMatrixIdentity();
    void SetIdentity();

    // these are the outputs of this class:
    //    scale matrix - to be baked into the mesh
    //    hierarchy matrix - stored in the skeleton
    const tlMatrix& GetScaleMatrix() const;
    const tlMatrix& GetHierarchyMatrix() const;

    // routines for extracting the above matrices,
    // except with an option to change coordinate systems
    //
    void GetScaleMatrixLHS(tlMatrix& matrix) const;
    void GetHierarchyMatrixLHS(tlMatrix& matrix) const;
    void GetScaleMatrixRHS(tlMatrix& matrix) const;
    void GetHierarchyMatrixRHS(tlMatrix& matrix) const;

private:

    bool AssignJoint(MDagPath& dagPath, MObject& jointNode);

    void ComputeScaleMatrix();
    void ComputeHierarchyMatrix();

    tlPoint scalePivot;
    float scaleX;
    float scaleY;
    float scaleZ;
    float shearXY;
    float shearXZ;
    float shearYZ;
    tlPoint scalePivotTranslate;
    tlPoint rotatePivot;
    float rotateAxisX;
    float rotateAxisY;
    float rotateAxisZ;
    RotateOrder rotateOrder;
    float rotateX;
    float rotateY;
    float rotateZ;
    tlPoint rotatePivotTranslate;
    float jointOrientX;
    float jointOrientY;
    float jointOrientZ;
    float inverseScaleX;
    float inverseScaleY;
    float inverseScaleZ;
    tlPoint translate;

    bool scaleDirty;
    bool hierarchyDirty;

    tlMatrix scaleMatrix;
    tlMatrix hierarchyMatrix;
};


#endif // _TRANSFORMMATRIX_HPP

