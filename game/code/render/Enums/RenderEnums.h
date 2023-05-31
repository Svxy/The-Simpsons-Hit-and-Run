#ifndef __RenderEnums_H__
#define __RenderEnums_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   RenderEnums
//
// Description: The RenderEnums does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/04/24]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================

//========================================================================
//
// Synopsis:   The RenderEnums; Synopsis by Inspection.
//
//========================================================================
class RenderEnums 
{
public:
   RenderEnums(){}
   ~RenderEnums(){}

   enum LayerEnum
   {
      GUI = 0x00000000,
      PresentationSlot,
      LevelSlot,
      MissionSlot1,
      MissionSlot2,
      numLayers,
      LayerOnlyMask = 0x000000FF
   };

   enum UserDataEnum
   {
      BogusUserData              = 0x00000000,
      AllRenderLoadingComplete   = 0x00100000,
      AllIntersectLoadingComplete= 0x00200000,
      DynamicLoadComplete        = 0x00400000,
      CompletionOnlyMask         = 0x00F00000
   };

   enum LoadZoneEnum
   {
      Zone1    = 0x00001000,
      ZoneMask = 0x000FF000,
      ZoneShift= 12
   };

   //GutsCallEnum is meant to mask to the top byte of a CB whose 
   //bottom byte is LayerEnum
   enum GutsCallEnum   
   {
      DrawableGuts      = 0x01000000,
      GeometryGuts      = 0x02000000,
      IntersectGuts     = 0x03000000,
      StaticEntityGuts  = 0x04000000,
      StaticPhysGuts    = 0x05000000,
      TreeDSGGuts       = 0x06000000,
      FenceGuts         = 0x07000000,
      AnimCollGuts      = 0x08000000,
      DynaPhysGuts      = 0x09000000,
      LocatorGuts       = 0x0A000000,
      WorldSphereGuts   = 0x0B000000,
      RoadSegmentGuts   = 0x0C000000,
	  PathSegmentGuts   = 0x0D000000,
      GlobalWSphereGuts = 0x0E000000,
      AnimGuts          = 0x0F000000,
      IgnoreGuts        = 0xFE000000,
      GutsOnlyMask      = 0xFF000000
   };

   enum LevelEnum
   {
      L1,
      L2,
      L3,
      L4,
      L5,
      L6,
      L7,
      //L8,
      //L9,
      //MULTI,
      numLevels,

      //Mini games
      B00 = numLevels,
      B01,
      B02,
      B03,
      B04,
      B05,
      B06,
      B07,

      MAX_LEVEL
   };

   enum LevelMissionCountEnum
   {
      L1MCount = 10,
      L2MCount = 10,
      L3MCount = 10,
      L4MCount = 10,
      L5MCount = 10,
      L6MCount = 10,
      L7MCount = 10,
      L8MCount = 10,
      L9MCount = 10
   };

   enum MissionEnum
   {
      M1,
      M2,
      M3,
      M4,
      M5,
      M6,
      M7,
      M8,
      M9,
      M10,
      numMissions
   };

private:
};

#endif
