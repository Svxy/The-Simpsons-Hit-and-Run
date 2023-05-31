/*===========================================================================
    transformmatrix.cpp
    Created: October 16, 2000
    Auuthor: Bryan Brandt

    Copyright (c) 2000 Radical Entertainment, Inc.
    All rights reserved.
===========================================================================*/

//#include <maya/MFnTransform.h>
//#include <maya/MPoint.h>
//#include <maya/MQuaternion.h>
//#include <maya/MEulerRotation.h>
//#include <maya/MVector.h>
//#include <maya/MDagPath.h>
//#include <maya/MFnIkJoint.h>
//#include <maya/MPlug.h>

#include "transformmatrix.h"


TransformMatrix::TransformMatrix():
    rotateOrder(ORDER_XYZ)
{
    SetIdentity();
}


TransformMatrix::TransformMatrix(const MDagPath& dagPath):
    rotateOrder(ORDER_XYZ)
{
    SetIdentity();
    Assign(dagPath);
}


TransformMatrix::TransformMatrix(const MTransformationMatrix& xform):
    rotateOrder(ORDER_XYZ)
{
    SetIdentity();
    Assign(xform);
}


bool TransformMatrix::Assign(const MDagPath& dagPath)
{
    MStatus status;

    MObject node = dagPath.node(&status);
    if (!status)
        return false;

    if (node.hasFn(MFn::kJoint))
    {
        MDagPath tmpDagPath = dagPath;
        return AssignJoint(tmpDagPath, node);
    }
    else if (node.hasFn(MFn::kTransform))
    {
        MFnTransform xformNode(dagPath, &status);
        if (!status)
            return false;

        MTransformationMatrix xform = xformNode.transformation(&status);
        if (!status)
            return false;

        return Assign(xform);
    }

    return false;
}


bool TransformMatrix::AssignJoint(MDagPath& dagPath, MObject& node)
{
    MStatus status;

    MFnIkJoint jointNode(node, &status);
    if (!status)
        return false;

    double tmp[3];
    MQuaternion tmpQuat;
    MEulerRotation tmpEuler;
    MVector tmpVector;

    scalePivot.x = 0.0f;
    scalePivot.y = 0.0f;
    scalePivot.z = 0.0f;

    status = jointNode.getScale(tmp);
    if (!status)
        return false;
    scaleX = (float)tmp[0];
    scaleY = (float)tmp[1];
    scaleZ = (float)tmp[2];

    // we've officially updated a scale dependency,
    // so set the scale matrix dirty flag
    scaleDirty = true;

    shearXY = 0.0f;
    shearXZ = 0.0f;
    shearYZ = 0.0f;

    scalePivotTranslate.x = 0.0f;
    scalePivotTranslate.y = 0.0f;
    scalePivotTranslate.z = 0.0f;

    rotatePivot.x = 0.0f;
    rotatePivot.y = 0.0f;
    rotatePivot.z = 0.0f;

    // we've officially updated a hierarchy dependency,
    // so set the hierarchy matrix dirty flag
    hierarchyDirty = true;

    status = jointNode.getScaleOrientation(tmpQuat);
    if (!status)
        return false;
    tmpEuler = tmpQuat.asEulerRotation();
    tmpEuler.reorderIt(MEulerRotation::kXYZ);
    rotateAxisX = (float)tmpEuler.x;
    rotateAxisY = (float)tmpEuler.y;
    rotateAxisZ = (float)tmpEuler.z;

    MTransformationMatrix::RotationOrder ro;
    jointNode.getRotation(tmp, ro);
    switch (ro)
    {
        case MTransformationMatrix::kXYZ:
            rotateOrder = ORDER_XYZ;
            break;
        case MTransformationMatrix::kYZX:
            rotateOrder = ORDER_YZX;
            break;
        case MTransformationMatrix::kZXY:
            rotateOrder = ORDER_ZXY;
            break;
        case MTransformationMatrix::kXZY:
            rotateOrder = ORDER_XZY;
            break;
        case MTransformationMatrix::kYXZ:
            rotateOrder = ORDER_YXZ;
            break;
        case MTransformationMatrix::kZYX:
            rotateOrder = ORDER_ZYX;
            break;

        default:
            return false;
    }

    rotateX = (float)tmp[0];
    rotateY = (float)tmp[1];
    rotateZ = (float)tmp[2];

    rotatePivotTranslate.x = 0.0f;
    rotatePivotTranslate.y = 0.0f;
    rotatePivotTranslate.z = 0.0f;

    status = jointNode.getOrientation(tmpEuler);
    tmpEuler.reorderIt(MEulerRotation::kXYZ);
    jointOrientX = (float)tmpEuler.x;
    jointOrientY = (float)tmpEuler.y;
    jointOrientZ = (float)tmpEuler.z;

    // default inverseScale to identity
    inverseScaleX = 1.0f;
    inverseScaleY = 1.0f;
    inverseScaleZ = 1.0f;

    // retrieve the segment scale compensate attribute
    MFnDependencyNode depNode(node, &status);
    if (!status)
        return false;

    MPlug plug = depNode.findPlug("segmentScaleCompensate", &status);
    if (!status)
        return false;

    bool segmentScaleCompensate;
    status = plug.getValue(segmentScaleCompensate);
    if (!status)
        return false;

    // if we are compensating for parent scale, do so
    if (segmentScaleCompensate)
    {
        plug = depNode.findPlug("inverseScaleX", &status);
        if (!status)
            return false;
        status = plug.getValue(inverseScaleX);
        if (!status)
            return false;
        if (inverseScaleX != 0.0f)
            inverseScaleX = (1.0f / inverseScaleX);

        plug = depNode.findPlug("inverseScaleY", &status);
        if (!status)
            return false;
        status = plug.getValue(inverseScaleY);
        if (!status)
            return false;
        if (inverseScaleY != 0.0f)
            inverseScaleY = (1.0f / inverseScaleY);

        plug = depNode.findPlug("inverseScaleZ", &status);
        if (!status)
            return false;
        status = plug.getValue(inverseScaleZ);
        if (!status)
            return false;
        if (inverseScaleZ != 0.0f)
            inverseScaleZ = (1.0f / inverseScaleZ);
    }

    tmpVector = jointNode.translation(MSpace::kWorld, &status);
    if (!status)
        return false;
    translate.x = (float)tmpVector.x;
    translate.y = (float)tmpVector.y;
    translate.z = (float)tmpVector.z;

    return true;
}


bool TransformMatrix::Assign(const MTransformationMatrix& xform)
{
    MStatus status;
    MPoint tmpPoint;
    double tmpArray[3];
    MQuaternion tmpQuat;
    MEulerRotation tmpEuler;
    MVector tmpVector;

    tmpPoint = xform.scalePivot(MSpace::kWorld, &status);
    if (!status)
        return false;
    scalePivot.x = (float)tmpPoint.x;
    scalePivot.y = (float)tmpPoint.y;
    scalePivot.z = (float)tmpPoint.z;

    // we've officially updated a scale dependency,
    // so set the scale matrix dirty flag
    scaleDirty = true;

    status = xform.getScale(tmpArray, MSpace::kWorld);
    if (!status)
        return false;
    scaleX = (float)tmpArray[0];
    scaleY = (float)tmpArray[1];
    scaleZ = (float)tmpArray[2];

    status = xform.getShear(tmpArray, MSpace::kWorld);
    if (!status)
        return false;
    shearXY = (float)tmpArray[0];
    shearXZ = (float)tmpArray[1];
    shearYZ = (float)tmpArray[2];

    tmpPoint = xform.scalePivotTranslation(MSpace::kWorld, &status);
    if (!status)
        return false;
    scalePivotTranslate.x = (float)tmpPoint.x;
    scalePivotTranslate.y = (float)tmpPoint.y;
    scalePivotTranslate.z = (float)tmpPoint.z;

    tmpPoint = xform.rotatePivot(MSpace::kWorld, &status);
    if (!status)
        return false;
    rotatePivot.x = (float)tmpPoint.x;
    rotatePivot.y = (float)tmpPoint.y;
    rotatePivot.z = (float)tmpPoint.z;

    // we've officially updated a hierarchy dependency,
    // so set the hierarchy matrix dirty flag
    hierarchyDirty = true;

    tmpQuat = xform.rotationOrientation();
    tmpEuler = tmpQuat.asEulerRotation();
    tmpEuler.reorderIt(MEulerRotation::kXYZ);
    rotateAxisX = (float)tmpEuler.x;
    rotateAxisY = (float)tmpEuler.y;
    rotateAxisZ = (float)tmpEuler.z;

    MTransformationMatrix::RotationOrder ro = xform.rotationOrder();
    switch (ro)
    {
        case MTransformationMatrix::kXYZ:
            rotateOrder = ORDER_XYZ;
            break;
        case MTransformationMatrix::kYZX:
            rotateOrder = ORDER_YZX;
            break;
        case MTransformationMatrix::kZXY:
            rotateOrder = ORDER_ZXY;
            break;
        case MTransformationMatrix::kXZY:
            rotateOrder = ORDER_XZY;
            break;
        case MTransformationMatrix::kYXZ:
            rotateOrder = ORDER_YXZ;
            break;
        case MTransformationMatrix::kZYX:
            rotateOrder = ORDER_ZYX;
            break;

        default:
            return false;
    }

    tmpQuat = xform.rotation();
    tmpEuler = tmpQuat.asEulerRotation();
    switch (rotateOrder)
    {
        case ORDER_XYZ:
            tmpEuler.reorderIt(MEulerRotation::kXYZ);
            break;
        case ORDER_YZX:
            tmpEuler.reorderIt(MEulerRotation::kYZX);
            break;
        case ORDER_ZXY:
            tmpEuler.reorderIt(MEulerRotation::kZXY);
            break;
        case ORDER_XZY:
            tmpEuler.reorderIt(MEulerRotation::kXZY);
            break;
        case ORDER_YXZ:
            tmpEuler.reorderIt(MEulerRotation::kYXZ);
            break;
        case ORDER_ZYX:
            tmpEuler.reorderIt(MEulerRotation::kZYX);
            break;

        default:
            return false;
    }
    rotateX = (float)tmpEuler.x;
    rotateY = (float)tmpEuler.y;
    rotateZ = (float)tmpEuler.z;

    tmpVector = xform.rotatePivotTranslation(MSpace::kWorld, &status);
    if (!status)
        return false;
    rotatePivotTranslate.x = (float)tmpVector.x;
    rotatePivotTranslate.y = (float)tmpVector.y;
    rotatePivotTranslate.z = (float)tmpVector.z;

    // only used in joints
    jointOrientX = 0.0f;
    jointOrientY = 0.0f;
    jointOrientZ = 0.0f;

    // only used in joints
    inverseScaleX = 1.0f;
    inverseScaleY = 1.0f;
    inverseScaleZ = 1.0f;

    tmpVector = xform.translation(MSpace::kWorld, &status);
    if (!status)
        return false;
    translate.x = (float)tmpVector.x;
    translate.y = (float)tmpVector.y;
    translate.z = (float)tmpVector.z;

    return true;
}


void TransformMatrix::SetScalePivot(const tlPoint& sp)
{
    scalePivot = sp;
    scaleDirty = true;
}


void TransformMatrix::SetScale(float x, float y, float z)
{
    scaleX = x;
    scaleY = y;
    scaleZ = z;
    scaleDirty = true;
}


void TransformMatrix::SetScaleX(float x)
{
    scaleX = x;
    scaleDirty = true;
}


void TransformMatrix::SetScaleY(float y)
{
    scaleY = y;
    scaleDirty = true;
}


void TransformMatrix::SetScaleZ(float z)
{
    scaleZ = z;
    scaleDirty = true;
}


void TransformMatrix::SetShear(float xy, float xz, float yz)
{
    shearXY = xy;
    shearXZ = xz;
    shearYZ = yz;
    scaleDirty = true;
}


void TransformMatrix::SetShearXY(float xy)
{
    shearXY = xy;
    scaleDirty = true;
}


void TransformMatrix::SetShearXZ(float xz)
{
    shearXZ = xz;
    scaleDirty = true;
}


void TransformMatrix::SetShearYZ(float yz)
{
    shearYZ = yz;
    scaleDirty = true;
}


void TransformMatrix::SetScalePivotTranslate(float x, float y, float z)
{
    scalePivotTranslate.x = x;
    scalePivotTranslate.y = y;
    scalePivotTranslate.z = z;
    scaleDirty = true;
}


void TransformMatrix::SetScalePivotTranslate(const tlPoint& spt)
{
    scalePivotTranslate = spt;
    scaleDirty = true;
}


void TransformMatrix::SetScalePivotX(float x)
{
    scalePivotTranslate.x = x;
    scaleDirty = true;
}


void TransformMatrix::SetScalePivotY(float y)
{
    scalePivotTranslate.y = y;
    scaleDirty = true;
}


void TransformMatrix::SetScalePivotZ(float z)
{
    scalePivotTranslate.z = z;
    scaleDirty = true;
}


void TransformMatrix::SetRotatePivot(float x, float y, float z)
{
    rotatePivot.x = x;
    rotatePivot.y = y;
    rotatePivot.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivot(const tlPoint& rp)
{
    rotatePivot = rp;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotX(float x)
{
    rotatePivot.x = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotY(float y)
{
    rotatePivot.y = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotZ(float z)
{
    rotatePivot.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateAxis(float x, float y, float z)
{
    rotateAxisX = x;
    rotateAxisY = y;
    rotateAxisZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateAxisX(float rax)
{
    rotateAxisX = rax;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateAxisY(float ray)
{
    rotateAxisY = ray;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateAxisZ(float raz)
{
    rotateAxisZ = raz;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotate(float x, float y, float z)
{
    rotateX = x;
    rotateY = y;
    rotateZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateX(float x)
{
    rotateX = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateY(float y)
{
    rotateY = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateZ(float z)
{
    rotateZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotateOrder(RotateOrder ro)
{
    rotateOrder = ro;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotTranslate(float x, float y, float z)
{
    rotatePivotTranslate.x = x;
    rotatePivotTranslate.y = y;
    rotatePivotTranslate.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotTranslate(const tlPoint& rpt)
{
    rotatePivotTranslate = rpt;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotTranslateX(float x)
{
    rotatePivotTranslate.x = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotTranslateY(float y)
{
    rotatePivotTranslate.y = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetRotatePivotTranslateZ(float z)
{
    rotatePivotTranslate.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetJointOrient(float x, float y, float z)
{
    jointOrientX = x;
    jointOrientY = y;
    jointOrientZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetJointOrientX(float x)
{
    jointOrientX = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetJointOrientY(float y)
{
    jointOrientY = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetJointOrientZ(float z)
{
    jointOrientZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetInverseScale(float x, float y, float z)
{
    inverseScaleX = x;
    inverseScaleY = y;
    inverseScaleZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetInverseScaleX(float x)
{
    inverseScaleX = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetInverseScaleY(float y)
{
    inverseScaleY = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetInverseScaleZ(float z)
{
    inverseScaleZ = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetTranslate(float x, float y, float z)
{
    translate.x = x;
    translate.y = y;
    translate.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetTranslate(const tlPoint& t)
{
    translate = t;
    hierarchyDirty = true;
}


void TransformMatrix::SetTranslateX(float x)
{
    translate.x = x;
    hierarchyDirty = true;
}


void TransformMatrix::SetTranslateY(float y)
{
    translate.y = y;
    hierarchyDirty = true;
}


void TransformMatrix::SetTranslateZ(float z)
{
    translate.z = z;
    hierarchyDirty = true;
}


void TransformMatrix::SetScaleMatrixIdentity()
{
    scalePivot.x = 0.0f;
    scalePivot.y = 0.0f;
    scalePivot.z = 0.0f;

    scaleX = 1.0f;
    scaleY = 1.0f;
    scaleZ = 1.0f;

    shearXY = 0.0f;
    shearXZ = 0.0f;
    shearYZ = 0.0f;

    scalePivotTranslate.x = 0.0f;
    scalePivotTranslate.y = 0.0f;
    scalePivotTranslate.z = 0.0f;

    scaleMatrix.IdentityMatrix();
    scaleDirty = false;
}


void TransformMatrix::SetHierarchyMatrixIdentity()
{
    rotatePivot.x = 0.0f;
    rotatePivot.y = 0.0f;
    rotatePivot.z = 0.0f;

    rotateAxisX = 0.0f;
    rotateAxisY = 0.0f;
    rotateAxisZ = 0.0f;

    rotateX = 0.0f;
    rotateY = 0.0f;
    rotateZ = 0.0f;

    rotatePivotTranslate.x = 0.0f;
    rotatePivotTranslate.y = 0.0f;
    rotatePivotTranslate.z = 0.0f;

    jointOrientX = 0.0f;
    jointOrientY = 0.0f;
    jointOrientZ = 0.0f;

    inverseScaleX = 1.0f;
    inverseScaleY = 1.0f;
    inverseScaleZ = 1.0f;

    translate.x = 0.0f;
    translate.y = 0.0f;
    translate.z = 0.0f;

    hierarchyMatrix.IdentityMatrix();
    hierarchyDirty = false;
}


void TransformMatrix::SetIdentity()
{
    SetScaleMatrixIdentity();
    SetHierarchyMatrixIdentity();
}


void TransformMatrix::ComputeScaleMatrix()
{
    if (!scaleDirty)
        return;

    scaleMatrix.IdentityMatrix();
    scaleMatrix.Translate(-scalePivot);
    scaleMatrix.Scale(scaleX, scaleY, scaleZ, true);
    scaleMatrix.Shear(shearXY, shearXZ, shearYZ);
    scaleMatrix.Translate(scalePivot);
    scaleMatrix.Translate(scalePivotTranslate);

    scaleMatrix.Scale(inverseScaleX, inverseScaleY, inverseScaleZ, true);

    scaleDirty = false;
}


void TransformMatrix::ComputeHierarchyMatrix()
{
    if (!hierarchyDirty)
        return;

    hierarchyMatrix.IdentityMatrix();
    hierarchyMatrix.Translate(-rotatePivot);

    hierarchyMatrix.RotateX(rotateAxisX);
    hierarchyMatrix.RotateY(rotateAxisY);
    hierarchyMatrix.RotateZ(rotateAxisZ);

    switch (rotateOrder)
    {
        case ORDER_XYZ:
            hierarchyMatrix.RotateX(rotateX);
            hierarchyMatrix.RotateY(rotateY);
            hierarchyMatrix.RotateZ(rotateZ);
            break;

        case ORDER_YZX:
            hierarchyMatrix.RotateY(rotateY);
            hierarchyMatrix.RotateZ(rotateZ);
            hierarchyMatrix.RotateX(rotateX);
            break;

        case ORDER_ZXY:
            hierarchyMatrix.RotateZ(rotateZ);
            hierarchyMatrix.RotateX(rotateX);
            hierarchyMatrix.RotateY(rotateY);
            break;

        case ORDER_XZY:
            hierarchyMatrix.RotateX(rotateX);
            hierarchyMatrix.RotateZ(rotateZ);
            hierarchyMatrix.RotateY(rotateY);
            break;

        case ORDER_YXZ:
            hierarchyMatrix.RotateY(rotateY);
            hierarchyMatrix.RotateX(rotateX);
            hierarchyMatrix.RotateZ(rotateZ);
            break;

        case ORDER_ZYX:
            hierarchyMatrix.RotateZ(rotateZ);
            hierarchyMatrix.RotateY(rotateY);
            hierarchyMatrix.RotateX(rotateX);
            break;
    }

    hierarchyMatrix.Translate(rotatePivot);
    hierarchyMatrix.Translate(rotatePivotTranslate);
    hierarchyMatrix.RotateX(jointOrientX);
    hierarchyMatrix.RotateY(jointOrientY);
    hierarchyMatrix.RotateZ(jointOrientZ);
    //hierarchyMatrix.Scale(inverseScaleX, inverseScaleY, inverseScaleZ, true);
    hierarchyMatrix.Translate(translate);

    hierarchyDirty = false;
}


const tlMatrix& TransformMatrix::GetScaleMatrix() const
{
    const_cast<TransformMatrix*>(this)->ComputeScaleMatrix();
    return scaleMatrix;
}


const tlMatrix& TransformMatrix::GetHierarchyMatrix() const
{
    const_cast<TransformMatrix*>(this)->ComputeHierarchyMatrix();
    return hierarchyMatrix;
}


void TransformMatrix::GetScaleMatrixLHS(tlMatrix& matrix) const
{
    GetScaleMatrixRHS(matrix);
    matrix.RHSToLHS();
}


void TransformMatrix::GetHierarchyMatrixLHS(tlMatrix& matrix) const
{
    GetHierarchyMatrixRHS(matrix);
    matrix.RHSToLHS();
}


void TransformMatrix::GetScaleMatrixRHS(tlMatrix& matrix) const
{
    const_cast<TransformMatrix*>(this)->ComputeScaleMatrix();
    matrix = scaleMatrix;
}


void TransformMatrix::GetHierarchyMatrixRHS(tlMatrix& matrix) const
{
    const_cast<TransformMatrix*>(this)->ComputeHierarchyMatrix();
    matrix = hierarchyMatrix;
}


// End of file.

