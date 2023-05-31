# Microsoft Developer Studio Project File - Name="relatedfiles" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=relatedfiles - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "relatedfiles.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "relatedfiles.mak" CFG="relatedfiles - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "relatedfiles - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "relatedfiles - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "relatedfiles"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "relatedfiles - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../game/libs/pure3d/toollib/inc" /I "../../game/libs/pure3d/constants" /I "../../game/libs/pure3d/toollib/chunks16/inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "relatedfiles - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../game/libs/pure3d/toollib/inc" /I "../../game/libs/pure3d/constants" /I "../../game/libs/pure3d/toollib/chunks16/inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "relatedfiles - Win32 Release"
# Name "relatedfiles - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "chunks16"

# PROP Default_Filter ""
# Begin Group "chunks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlAnimatedObjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlAnimationChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlBBoxChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlBillboardObjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlBSphereChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlChannelChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlCollisionObjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlExpressionChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFencelineChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFlexibleJointChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFlexibleObjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFontChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFrameControllerChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlGameAttrChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlImageChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlIntersectionChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlLightChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlLocatorChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlMeshChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlOpticEffectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlParticleSystemChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlPhotonMapChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlPhysicsObjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlPrimGroupChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlRoadChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlRoadSegmentChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlScenegraphChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlScroobyProjectChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlShaderChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlShadowSkinChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlSkinChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlSmartPropChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlSpriteChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlTextureChunk.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlWallChunk.sc
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlCameraAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlCameraChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlCompositeDrawableChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlEntityChannelChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlExportInfoChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlExpressionAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlFrameControllerChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlHistoryChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlLightAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlLightGroupChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlMultiControllerChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlPoseAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlScenegraphTransformAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlSkeletonChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlTextureAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlVertexAnimChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlVertexOffsetExpressionChunk16.sc
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\Schema16\tlVisibilityAnimChunk16.sc
# End Source File
# End Group
# Begin Group "inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\DataManip.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\dospath.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\glob.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\greyscale.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\hash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\lzr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\lzrf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\pddi\ps2\packet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\porc.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\pddi\ps2\ps2prim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\simdata.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlAdjEdgeList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlAdjFaceList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlAnimatedObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlAnimation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlBillboardObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlBillboardObjectAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlBox.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlBSPTree.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlByteStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlcamera.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlCameraAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlChannel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlChunk16.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlCollisionObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlColour.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlCompositeDrawable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlCompressedByteStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlDataChunk.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlEdge.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlEntity.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlentitytable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlEventAnimation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlExportInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlExpressionAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlExpressionOffsets.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFile.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFileByteStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFlexibleJoint.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFlexibleObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFont.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFourCC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlFrameController.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlGameAttr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlGCComponentQuantizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlGCPrimBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlHashList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlHashTable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlHeightField.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlHistory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlImage.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlImageQuantizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlIndexedSkinVertex.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlIndexedVertex.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlinventory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlIterator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlKeyDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlLight.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlLightAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlLightGroup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlLoadManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlLocator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlMatrix.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlMemByteStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlMultiController.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlOffsetList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlOffsetSkinVertex.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlOpticEffect.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlParticleSystem.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPDDI.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPhotonMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPhysicsObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPlane.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPoint.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPose.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPoseAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPrimBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPrimGroup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPrimGroupMesh.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPS2PrimBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlPS2SkinPrimBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlQuat.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlRay.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlRC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSafeEntityCaster.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlScenegraph.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlScenegraphTransformAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSegment.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlShader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSimpleShader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSkeleton.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSkin.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSkinVertex.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSphere.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlSprite.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlStringTable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTarga.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTextDataParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTexture.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTextureAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTranAnimKeys.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriangle.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriangleFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersect.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriangleIntersectFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriMesh.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTriMeshIntersect.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTsEdge.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTsTriangle.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTsTriangleToTriStripConverter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTsVertexCache.h
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTsVertexConnectivity.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlTypes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlUV.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlversion.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlVertex.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlVertexAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlVertexAnimKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlVisAnim.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\tlXBOXPrimBuffer.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\ChunkManip.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\code\constants\srrchunks.h
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\src\tlDataChunk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\inc\toollib.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\libs\pure3d\toollib\toollib_extras.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
