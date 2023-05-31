//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SkidMarkGenerator
//
// Description: A per-vehicle class that generates skidmarks
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <stdlib.h>

#include <worldsim/SkidMarks/SkidMarkGenerator.h>
#include <worldsim/SkidMarks/skidmarkmanager.h>
#include <worldsim/SkidMarks/skidmark.h>

#include <radmath/util.hpp>
#include <float.h>
#include <debug/profiler.h>
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#ifdef RAD_RELEASE
const float SKID_INTENSITY_DAMPENING = 0.5f;
#else
static float SKID_INTENSITY_DAMPENING = 0.5f;
#endif


SkidMarkGenerator::SkidMarkGenerator()
{
#ifndef RAD_RELEASE
    static bool s_AddedDebugging = false;
    if ( s_AddedDebugging == false )
    {
        radDbgWatchAddFloat( &SKID_INTENSITY_DAMPENING, "Skid mark dampening", "Skidmarks" );
        s_AddedDebugging = true;
    }
#endif
}

SkidMarkGenerator::~SkidMarkGenerator()
{
    for(int i = 0; i < 4; i++)
    {
        if(m_CurrentSkidData[i].currentSkid)
        {
            GetSkidmarkManager()->ReturnUsedSkidmark( m_CurrentSkidData[i].currentSkid );
        }
        m_CurrentSkidData[i].currentSkid = NULL;
    }
}

// Height in meters to raise the skids above the ground to avoid z fighting

// Keep pointers to the tShaders
static tShader* spPavementShader = NULL;
static tShader* spDirtShader = NULL;
static tShader* spGrassShader = NULL;

// The shader names for use in searching for skid mark shaders in the inventory
const char* PAVEMENT_SKIDMARK_SHADERNAME = "skid_m";
const char* DIRT_SKIDMARK_SHADERNAME = "dirt_skidmark_shader";
const char* GRASS_SKIDMARK_SHADERNAME = "grass_skidmark_shader";

// enables z test, disables z test, required for correct 
// skid mark appearance
static void SetShaderSkidSettings( tShader* pShader )
{
    pShader->SetInt(PDDI_SP_ALPHAOP1, PDDI_BLEND_ALPHA); 
    pShader->SetInt(PDDI_SP_TEXTUREOP1, PDDI_TEXBLEND_MODULATE);
    pShader->SetInt(PDDI_SP_COLOUROP1, PDDI_BLEND_SUBTRACT);
}

// Find the shaders in the inventory, set zwrite/ztest (since the artists can't
// export these shader settings directly and addref them
// SkidMarkGenerator::ReleaseShaders() must be called afterwards
void SkidMarkGenerator::InitShaders()
{

    // Just so that we never do an accidental double addref
    ReleaseShaders();
    
    spPavementShader = p3d::find< tShader >( PAVEMENT_SKIDMARK_SHADERNAME );
    if ( spPavementShader != NULL )
    {
      spPavementShader->AddRef();
      SetShaderSkidSettings( spPavementShader );
    }
    rTuneWarningMsg( spPavementShader != NULL, "Pavement skid shader not found in inventory! Some skids will not appear!");

    spDirtShader = p3d::find< tShader >( DIRT_SKIDMARK_SHADERNAME );
    if ( spDirtShader != NULL )
    {
        spDirtShader->AddRef();
        SetShaderSkidSettings( spDirtShader );
    }
    rTuneWarningMsg( spDirtShader != NULL, "Dirt skid shader not found in inventory! Some skids will not appear!");

    spGrassShader = p3d::find< tShader >( GRASS_SKIDMARK_SHADERNAME );
    if ( spGrassShader != NULL )
    {
        spGrassShader->AddRef();
        SetShaderSkidSettings( spGrassShader );
    }

    rTuneWarningMsg( spGrassShader != NULL, "Grass skid shader not found in inventory! Some skids will not appear!");


}
// Fetch the appropriate shader for the terrain type
tShader* SkidMarkGenerator::GetShader( eTerrainType terrainType )
{
    tShader* shader = NULL;

    switch (terrainType)
    {
    case TT_Dirt:
        shader = spDirtShader;
        break;
    case TT_Grass:
        shader = spGrassShader;
        break;
    case TT_Road:
    default:
        shader = spPavementShader;
        break;
    };
    return shader;
}


// Releases all the shaders that were addrefed in InitShaders()
void SkidMarkGenerator::ReleaseShaders()
{
    if ( spPavementShader != NULL )
    {
        spPavementShader->Release();
        spPavementShader = NULL;
    }
    if ( spDirtShader != NULL )
    {
        spDirtShader->Release();
        spDirtShader = NULL;
    }
    if ( spGrassShader != NULL )
    {
        spGrassShader->Release();
        spGrassShader = NULL;
    }
}   




void SkidMarkGenerator::GenerateSkid( int wheel, const SkidMarkGenerator::SkidData& skidData )
{
    if ( skidData.velocityDirection.MagnitudeSqr() < 0.01f || 
        skidData.intensity < 0.01f )
        return;

    
    float dampenedIntensity = skidData.intensity * SKID_INTENSITY_DAMPENING;


    rAssert( wheel >=0 && wheel < 4 );
    TireGeneratorData& generatorData = m_CurrentSkidData[ wheel ];    
    // Is the tire currently skidding?
    // if so, continue it
    // otherwise start a new skid

    rmt::Vector alignedOffset;
    skidData.transform.RotateVector( generatorData.offset, &alignedOffset );
    rmt::Vector skidCenter = skidData.transform.Row(3) + alignedOffset;

    rmt::Vector skidDirection = skidData.velocityDirection;
    skidDirection.Normalize();

    // Check to see if the terrain type changed
    if ( generatorData.currentSkid != NULL )
    {
        const float ANGLE_TOO_SHARP = 0.7f;
        bool angleTooSharp = generatorData.currentSkid->GetCurrentDirection().Dot( skidDirection ) < ANGLE_TOO_SHARP;
        bool terrainTypeChanged = generatorData.terrainType != skidData.terrainType;

        if ( angleTooSharp || terrainTypeChanged )
        {
            // Finish off the current skid and start a new one
            generatorData.currentSkid->FadeOutTrailingVertices();
            GetSkidmarkManager()->ReturnUsedSkidmark( generatorData.currentSkid );
            generatorData.currentSkid = NULL;
            GenerateSkid( wheel, skidData );
        }
    }
    // Check to see if 

    if ( generatorData.currentSkid != NULL )
    {
        // Is there space left for more vertices?
        if ( generatorData.currentSkid->SpaceLeft() )
        {
            generatorData.currentSkid->Extend( skidCenter, skidDirection, skidData.groundPlaneNormal, generatorData.halfWidth, dampenedIntensity ); 
            generatorData.skidExtendedThisFrame = true;
        }
        else
        {
            // No space left. We must continue this skid with a new skidmark 
            Skidmark* skidMark = GetSkidmarkManager()->GetUnusedSkidmark();
            if ( skidMark != NULL )
            {
                generatorData.currentSkid->ContinueSkidmark( skidMark );
                GetSkidmarkManager()->ReturnUsedSkidmark( generatorData.currentSkid );
                generatorData.currentSkid = skidMark;
                generatorData.currentSkid->Extend( skidCenter, skidDirection, skidData.groundPlaneNormal, generatorData.halfWidth, dampenedIntensity ); 
                generatorData.skidExtendedThisFrame = true;
            }
            else
            {
                // No space available!
                // Tell the current skid to fade out, we are done. Pretend the skid is finished
                generatorData.currentSkid->FadeOutTrailingVertices();
                GetSkidmarkManager()->ReturnUsedSkidmark( generatorData.currentSkid );
                generatorData.currentSkid = NULL;
                generatorData.skidExtendedThisFrame = false;
            }
        }
    }
    else
    {
        // Fetch a skidmark thats not being used right now
        generatorData.currentSkid = GetSkidmarkManager()->GetUnusedSkidmark();
        if ( generatorData.currentSkid != NULL )
        {
            generatorData.currentSkid->Extend( skidCenter, skidDirection, skidData.groundPlaneNormal, generatorData.halfWidth, dampenedIntensity ); 
            generatorData.skidExtendedThisFrame = true;
            generatorData.terrainType = skidData.terrainType;
            generatorData.currentSkid->SetShader( GetShader( skidData.terrainType ) );
            generatorData.currentSkid->FadeInVertices();
        }
    }
}
        


void
SkidMarkGenerator::Update(  )
{
    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( m_CurrentSkidData[i].skidExtendedThisFrame == false)
        {
            if ( m_CurrentSkidData[i].currentSkid != NULL )
            {
                m_CurrentSkidData[i].currentSkid->FadeOutTrailingVertices();
                GetSkidmarkManager()->ReturnUsedSkidmark( m_CurrentSkidData[i].currentSkid );
                m_CurrentSkidData[i].currentSkid = NULL;
            }
        }
        else
        {
            m_CurrentSkidData[i].skidExtendedThisFrame = false;
        }
    }
}




