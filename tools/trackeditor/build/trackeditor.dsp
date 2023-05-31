# Microsoft Developer Studio Project File - Name="trackeditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=trackeditor - Win32 Tools Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "trackeditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "trackeditor.mak" CFG="trackeditor - Win32 Tools Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "trackeditor - Win32 Tools Release" (based on "Win32 (x86) Application")
!MESSAGE "trackeditor - Win32 Tools Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "trackeditor"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "trackeditor___Win32_Tools_Release"
# PROP BASE Intermediate_Dir "trackeditor___Win32_Tools_Release"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\code\\" /I "C:\AW\Maya4.0\include" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /Fp"Release/PCH.pch" /YX"precompiled/PCH.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /I "..\..\..\game\code\constants" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_RELEASE" /Fp"Release/PCH.pch" /YX"precompiled/pch.h" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\trackeditor.mll" /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\trackeditor.mll" /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy trackeditor.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Release\trackeditor.mll ..\..\MayaTools\Maya4.0\bin\plug-ins\trackeditor.mll	copy .\Release\trackeditor.mll C:\AW\Maya4.0\bin\plug-ins\trackeditor.mll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "trackeditor___Win32_Tools_Debug"
# PROP BASE Intermediate_Dir "trackeditor___Win32_Tools_Debug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\code\\" /I "C:\AW\Maya4.0\include" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /FR /YX"PCH.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /I "..\..\..\game\code\constants" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /FR /YX"PCH.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\trackeditor.mll" /pdbtype:sept /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\trackeditor.mll" /pdbtype:sept /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy trackeditor.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Debug\trackeditor.mll C:\AW\Maya4.0\bin\plug-ins\trackeditord.mll	copy .\Debug\trackeditor.mll C:\AW\Maya4.0\bin\plug-ins\trackeditor.mll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "trackeditor - Win32 Tools Release"
# Name "trackeditor - Win32 Tools Debug"
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\main\constants.h
# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.cpp
DEP_CPP_PLUGI=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MApiVersion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnPlugin.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\mayahandles.h"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\commands\export.h"\
	"..\code\commands\intersectioncommands.h"\
	"..\code\commands\trackeditorcommands.h"\
	"..\code\commands\treelinecommand.h"\
	"..\code\contexts\bvcontext.h"\
	"..\code\contexts\intersectioncontext.h"\
	"..\code\contexts\ppcontext.h"\
	"..\code\contexts\treelinecontext.h"\
	"..\code\main\constants.h"\
	"..\code\main\pluginMain.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\fenceline.h"\
	"..\code\nodes\intersection.h"\
	"..\code\nodes\pedpath.h"\
	"..\code\nodes\road.h"\
	"..\code\nodes\tiledisplay.h"\
	"..\code\nodes\treelineshapenode.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD BASE CPP /YX"precompiled/PCH.h"
# ADD CPP /YX"precompiled/PCH.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.h
# End Source File
# Begin Source File

SOURCE=..\code\main\shapeconstants.h
# End Source File
# Begin Source File

SOURCE=..\code\main\trackeditor.cpp
DEP_CPP_TRACK=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\fenceline.h"\
	"..\code\nodes\intersection.h"\
	"..\code\nodes\pedpath.h"\
	"..\code\nodes\road.h"\
	"..\code\nodes\tiledisplay.h"\
	"..\code\nodes\treelineshapenode.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\main\trackeditor.h
# End Source File
# End Group
# Begin Group "commands"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\commands\export.cpp
DEP_CPP_EXPOR=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\..\globalcode\utility\MUI.h"\
	"..\code\commands\export.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\fenceline.h"\
	"..\code\nodes\intersection.h"\
	"..\code\nodes\pedpath.h"\
	"..\code\nodes\road.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\commands\export.h
# End Source File
# Begin Source File

SOURCE=..\code\commands\intersectioncommands.cpp
DEP_CPP_INTER=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\commands\intersectioncommands.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\intersection.h"\
	"..\code\nodes\road.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\commands\intersectioncommands.h
# End Source File
# Begin Source File

SOURCE=..\code\commands\trackeditorcommands.cpp
DEP_CPP_TRACKE=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\commands\trackeditorcommands.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\commands\trackeditorcommands.h
# End Source File
# Begin Source File

SOURCE=..\code\commands\treelinecommand.cpp
DEP_CPP_TREEL=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\code\commands\treelinecommand.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\treelineshapenode.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\commands\treelinecommand.h
# End Source File
# End Group
# Begin Group "nodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\nodes\fenceline.cpp
DEP_CPP_FENCE=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\fenceline.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\fenceline.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\intersection.cpp
DEP_CPP_INTERS=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\GLExt.h"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\..\globalcode\utility\nodehelper.h"\
	"..\..\globalcode\utility\transformmatrix.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\intersection.h"\
	"..\code\precompiled\PCH.h"\
	
NODEP_CPP_INTERS=\
	"..\..\globalcode\utility\main\toolhack.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\intersection.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\NU.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\pedpath.cpp
DEP_CPP_PEDPA=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\pedpath.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\pedpath.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\road.cpp
DEP_CPP_ROAD_=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\..\globalcode\utility\transformmatrix.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\road.h"\
	"..\code\precompiled\PCH.h"\
	
NODEP_CPP_ROAD_=\
	"..\..\globalcode\utility\main\toolhack.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\road.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\tiledisplay.cpp
DEP_CPP_TILED=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\GLExt.h"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\tiledisplay.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\tiledisplay.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\treelineshapenode.cpp
DEP_CPP_TREELI=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\..\globalcode\utility\MUI.h"\
	"..\code\main\constants.h"\
	"..\code\main\shapeconstants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\treelineshapenode.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\treelineshapenode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\walllocator.cpp
DEP_CPP_WALLL=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\GLExt.h"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\..\globalcode\utility\nodehelper.h"\
	"..\code\main\constants.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\nodes\walllocator.h
# End Source File
# End Group
# Begin Group "contexts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\contexts\bvcontext.cpp
DEP_CPP_BVCON=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\contexts\bvcontext.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\fenceline.h"\
	"..\code\nodes\NU.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\contexts\bvcontext.h
# End Source File
# Begin Source File

SOURCE=..\code\contexts\intersectioncontext.cpp
DEP_CPP_INTERSE=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\contexts\intersectioncontext.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\intersection.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\contexts\intersectioncontext.h
# End Source File
# Begin Source File

SOURCE=..\code\contexts\ppcontext.cpp
DEP_CPP_PPCON=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\contexts\ppcontext.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\NU.h"\
	"..\code\nodes\pedpath.h"\
	"..\code\nodes\walllocator.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\contexts\ppcontext.h
# End Source File
# Begin Source File

SOURCE=..\code\contexts\treelinecontext.cpp
DEP_CPP_TREELIN=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\contexts\treelinecontext.h"\
	"..\code\main\constants.h"\
	"..\code\main\trackeditor.h"\
	"..\code\nodes\treelineshapenode.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\contexts\treelinecontext.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\globalcode\utility\GLExt.cpp
DEP_CPP_GLEXT=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\GLExt.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\GLExt.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\mayahandles.cpp
DEP_CPP_MAYAH=\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\globalcode\utility\mayahandles.h"\
	"..\..\globalcode\utility\stdafx.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\mayahandles.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt.cpp
DEP_CPP_MEXT_=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt.h"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt_template.cpp
DEP_CPP_MEXT_T=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\MExt_template.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt_template.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MUI.cpp
DEP_CPP_MUI_C=\
	"..\..\globalcode\utility\mayahandles.h"\
	"..\..\globalcode\utility\MUI.h"\
	"..\..\globalcode\utility\stdafx.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MUI.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\overlaymarquee.cpp
DEP_CPP_OVERL=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\overlaymarquee.h"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\overlaymarquee.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\transformmatrix.cpp
DEP_CPP_TRANS=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\..\globalcode\utility\transformmatrix.h"\
	"..\code\precompiled\PCH.h"\
	
NODEP_CPP_TRANS=\
	"..\..\globalcode\utility\main\toolhack.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\transformmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.c

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\winutil.c
DEP_CPP_WINUT=\
	"..\..\globalcode\utility\winutil.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD CPP /YX"precompiled\pch.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD CPP /YX"precompiled\pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\winutil.h
# End Source File
# End Group
# Begin Group "mel scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\scripts\te_BVContext.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_BVContext.mel
InputName=te_BVContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_BVContext.mel
InputName=te_BVContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_cleanup.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_cleanup.mel
InputName=te_cleanup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_cleanup.mel
InputName=te_cleanup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_editorwindow.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_editorwindow.mel
InputName=te_editorwindow

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_editorwindow.mel
InputName=te_editorwindow

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_globals.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_globals.mel
InputName=te_globals

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_globals.mel
InputName=te_globals

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_IntersectionContext.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_IntersectionContext.mel
InputName=te_IntersectionContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_IntersectionContext.mel
InputName=te_IntersectionContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_main.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_main.mel
InputName=te_main

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_main.mel
InputName=te_main

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_PPContext.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_PPContext.mel
InputName=te_PPContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_PPContext.mel
InputName=te_PPContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_setup.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_setup.mel
InputName=te_setup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\te_setup.mel
InputName=te_setup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\te_treelineContext.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build
InputPath=..\code\scripts\te_treelineContext.mel
InputName=te_treelineContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build
InputPath=..\code\scripts\te_treelineContext.mel
InputName=te_treelineContext

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "precompiled header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\precompiled\PCH.cpp
DEP_CPP_PCH_C=\
	"..\..\..\game\code\constants\srrchunks.h"\
	"..\..\..\game\libs\pure3d\constants\atenum.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunkids.hpp"\
	"..\..\..\game\libs\pure3d\constants\chunks.h"\
	"..\..\..\game\libs\pure3d\constants\psenum.hpp"\
	"..\..\..\game\libs\pure3d\pddi\pddienum.hpp"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\M3dView.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAngle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAnimControl.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArgList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataBuilder.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MArrayDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeIndex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpec.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MAttributeSpecArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MBoundingBox.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MColorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MCursor.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDagPath.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataBlock.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDataHandle.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDGModifier.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDistance.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDoubleArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequest.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MDrawRequestQueue.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEulerRotation.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MEvent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFloatVectorArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFn.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAnimCurve.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnBase.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnCompoundAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDagNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDependencyNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnDoubleArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnEnumAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIkJoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnIntArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMatrixData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMesh.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnMessageAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnNumericData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnSingleIndexedComponent.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnStringArrayData.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTransform.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MFnTypedAttribute.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MGlobal.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MIntArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDag.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItDependencyNodes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItMeshVertex.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MItSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMaterial.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MNodeMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObject.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MObjectArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlug.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPlugArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPoint.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPointArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContext.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxContextCommand.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxGeometryIterator.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxLocatorNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxNode.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShape.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MPxSurfaceShapeUI.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MQuaternion.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectInfo.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionList.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSelectionMask.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStatus.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MString.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MStringArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MSyntax.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTime.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTimeArray.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTransformationMatrix.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypeId.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MTypes.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MUiMessage.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVector.h"\
	"..\..\..\game\libs\pure3d\sdks\Maya4_0\include\maya\MVectorArray.h"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimatedObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimationChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimCollDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAnimDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlAttributeTableChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBBoxChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBreakableObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlBSphereChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlChannelChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCollisionObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlCompositeDrawableChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlDynaPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlExtraMatrixChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFenceDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFencelineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFollowCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlFrameControllerChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlImageChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaEntityDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstancesChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstaStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlInstParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlIntersectionChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMeshChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlMultiControllerChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlObjectAttributeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlParticleSystemChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPedpathChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysicsObjectChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPhysWrapperChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlPrimGroupChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlRoadSegmentDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlScenegraphChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSkinChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlSpriteChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlStaticPhysDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlTreeDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWalkerCamDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWallChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBLocatorChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBRailCamChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBSplineChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWBTriggerVolumeChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\chunks16\inc\tlWorldSphereDSGChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\dospath.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\getopt.h"\
	"..\..\..\game\libs\pure3d\toollib\inc\glob.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\hash.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\lzr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlBox.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlColour.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFile.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFont.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImage.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLight.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPose.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlRay.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlString.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTable.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlUV.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlversion.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp"\
	"..\..\..\game\libs\pure3d\toollib\inc\toollib.hpp"\
	"..\..\..\game\libs\radmath\radmath\buildconfig.hpp"\
	"..\..\..\game\libs\radmath\radmath\geometry.hpp"\
	"..\..\..\game\libs\radmath\radmath\matrix.hpp"\
	"..\..\..\game\libs\radmath\radmath\platform\ps2\vu0microcode.hpp"\
	"..\..\..\game\libs\radmath\radmath\quaternion.hpp"\
	"..\..\..\game\libs\radmath\radmath\radmath.hpp"\
	"..\..\..\game\libs\radmath\radmath\random.hpp"\
	"..\..\..\game\libs\radmath\radmath\spline.hpp"\
	"..\..\..\game\libs\radmath\radmath\trig.hpp"\
	"..\..\..\game\libs\radmath\radmath\util.hpp"\
	"..\..\..\game\libs\radmath\radmath\vector.hpp"\
	"..\code\precompiled\PCH.h"\
	

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# ADD BASE CPP /Yc"PCH.h"
# ADD CPP /Yc"PCH.h"

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# ADD BASE CPP /Yc"PCH.h"
# ADD CPP /Yc"PCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\precompiled\PCH.h
# End Source File
# End Group
# Begin Group "AETemplates"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\AETemplates\AEIntersectionLocatorNodeTemplate.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEIntersectionLocatorNodeTemplate.mel
InputName=AEIntersectionLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEIntersectionLocatorNodeTemplate.mel
InputName=AEIntersectionLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AERoadNodeTemplate.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AERoadNodeTemplate.mel
InputName=AERoadNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AERoadNodeTemplate.mel
InputName=AERoadNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AETELocatorSuppress.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETELocatorSuppress.mel
InputName=AETELocatorSuppress

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETELocatorSuppress.mel
InputName=AETELocatorSuppress

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AETEShowRoadSegButton.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETEShowRoadSegButton.mel
InputName=AETEShowRoadSegButton

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETEShowRoadSegButton.mel
InputName=AETEShowRoadSegButton

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AETreelineShapeNodeTemplate.mel

!IF  "$(CFG)" == "trackeditor - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETreelineShapeNodeTemplate.mel
InputName=AETreelineShapeNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "trackeditor - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETreelineShapeNodeTemplate.mel
InputName=AETreelineShapeNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
