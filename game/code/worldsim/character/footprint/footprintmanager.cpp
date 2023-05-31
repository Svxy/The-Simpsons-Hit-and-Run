//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   footprintmanager
//
// Description: Holds a list of all footprints, and fades them out over time
//
// Authors:     Michael Riegger
//
//===========================================================================


//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <worldsim/character/footprint/footprintmanager.h>
#include <memory/srrmemory.h>
#include <p3d/utility.hpp>
#include <contexts/bootupcontext.h>
#include <p3d/texture.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/shader.hpp>
//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float FOOTPRINT_LIVE_TIME = 0.5f;
const float FOOTPRINT_FADE_PER_MS  = 1 / 2000.0f;


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

FootprintManager* FootprintManager::spFootprintManager = 0;

//===========================================================================
// Member Functions
//===========================================================================

FootprintManager::Footprint::Footprint():
alpha( 0 )
{
    const float FOOTPRINT_WIDTH = 1.0f;
    const float FOOTPRINT_HEIGHT = 1.0f;
    const float HALF_FOOTPRINT_WIDTH = FOOTPRINT_WIDTH / 2.0f;
    const float HALF_FOOTPRINT_HEIGHT = FOOTPRINT_HEIGHT / 2.0f;

    rmt::Vector topLeft( -HALF_FOOTPRINT_WIDTH, 0, HALF_FOOTPRINT_HEIGHT );
    rmt::Vector topRight( HALF_FOOTPRINT_WIDTH, 0 , HALF_FOOTPRINT_HEIGHT );
    rmt::Vector bottomLeft( -HALF_FOOTPRINT_WIDTH,0 ,-HALF_FOOTPRINT_HEIGHT );
    rmt::Vector bottomRight( HALF_FOOTPRINT_WIDTH,0, -HALF_FOOTPRINT_HEIGHT );

    points[0] = bottomLeft;
    points[1] = bottomRight;
    points[2] = topLeft;
    points[3] = topRight;
}

FootprintManager::FootprintManager()
{
    for ( int i = 0 ; i < eNumFootprintTypes ; i++ )
    {
        m_ActiveFootprints[ i ].Allocate( MAX_NUM_FOOTPRINTS );
        mpTextures[ i ] = NULL;
    }
}

FootprintManager::~FootprintManager()
{
    FreeTextures();
}

FootprintManager* FootprintManager::CreateInstance( void )
{
	rAssertMsg( spFootprintManager == 0, "FootprintManager already created.\n" );
    HeapManager::GetInstance()->PushHeap( GMA_PERSISTENT );
	spFootprintManager = new FootprintManager;
    rAssert( spFootprintManager );
    HeapManager::GetInstance()->PopHeap(GMA_PERSISTENT);
    return FootprintManager::GetInstance();
}

FootprintManager* FootprintManager::GetInstance( void )
{
	rAssertMsg( spFootprintManager != 0, "FootprintManager has not been created yet.\n" );
	return spFootprintManager;
}

void FootprintManager::DestroyInstance( void )
{
	rAssertMsg( spFootprintManager != 0, "FootprintManager has not been created.\n" );
	delete spFootprintManager;
    spFootprintManager = 0;
}

bool FootprintManager::CreateFootprint( const rmt::Matrix& transform, TYPE type )
{   

    bool footprintCreated;

    if ( mpTextures[ type ] == NULL )
        return false;

    Footprint footprint;
    footprint.alpha = 1.0f;
    for ( int i = 0 ; i < 4 ; i++ )
    {
        transform.Transform( footprint.points[i], &footprint.points[i] );    
    }
    
    if ( m_ActiveFootprints[ type ].mSize > m_ActiveFootprints[ type ].mUseSize )
    {
        m_ActiveFootprints[ type ].Add( footprint );
        footprintCreated = true;
    }
    else
    {
        footprintCreated = false;
    }
    return footprintCreated;
}

void FootprintManager::Render()
{
    return;
    int i,j;


    SetTexture( FootprintManager::eSquishies, "footprint.bmp" );

    bool oldZWrite = p3d::pddi->GetZWrite();
    pddiCompareMode oldZComp = p3d::pddi->GetZCompare();
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( false );
    }
    if( oldZComp != PDDI_COMPARE_LESS )
    {
        p3d::pddi->SetZCompare( PDDI_COMPARE_LESS );
    }
    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);

    for ( i = 0 ; i < eNumFootprintTypes ;i++)
    {
        if ( m_ActiveFootprints[i].mUseSize == 0 )
            continue;
           
        if ( mpTextures[i] == NULL )
            continue;
        
        pddiShader* pShader = GetBootupContext()->GetSharedShader();
        pShader->SetInt( PDDI_SP_ISLIT, 0 );
        pShader->SetInt( PDDI_SP_ALPHATEST, 0 );
        pShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT );
        pShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
        pShader->SetTexture( PDDI_SP_BASETEX, mpTextures[ i ]->GetTexture() );

        pddiPrimStream* stream = p3d::pddi->BeginPrims( pShader, PDDI_PRIM_TRIANGLES, PDDI_V_CT, m_ActiveFootprints[i].mUseSize * 6 );

        for ( j = 0 ; j < m_ActiveFootprints[i].mUseSize ; j++ )
        {
            Footprint& footprint = m_ActiveFootprints[i][j];
            tColour colour;
            colour.Set( 255,255,255, static_cast< int >( footprint.alpha * 255.0f ));

            // 2 triangles for every foot step
            // Now draw the triangle.
            // bottom left tri.
            stream->Colour( colour );
            stream->UV( 0.0f, 0.0f );
            stream->Coord( footprint.points[0].x, footprint.points[0].y, footprint.points[0].z );

            stream->Colour( colour );
            stream->UV( 1.0f, 0.0f );
            stream->Coord( footprint.points[1].x, footprint.points[1].y, footprint.points[1].z );

            stream->Colour( colour );
            stream->UV( 0.0f, 1.0f );
            stream->Coord( footprint.points[2].x, footprint.points[2].y, footprint.points[2].z );

            // Top right tri.
            stream->Colour( colour );
            stream->UV( 1.0f, 1.0f );
            stream->Coord( footprint.points[3].x, footprint.points[3].y, footprint.points[3].z );

            stream->Colour( colour );
            stream->UV( 0.0f, 1.0f );
            stream->Coord( footprint.points[2].x, footprint.points[2].y, footprint.points[2].z );

            stream->Colour( colour );
            stream->UV( 1.0f, 0.0f );
            stream->Coord( footprint.points[1].x, footprint.points[1].y, footprint.points[1].z );

        }
        p3d::pddi->EndPrims( stream );
        pShader->SetTexture( PDDI_SP_BASETEX, 0 );

    }

    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( true );
    }
    if( oldZComp != PDDI_COMPARE_ALWAYS )
    {
       	p3d::pddi->SetZCompare( oldZComp );
    }
    p3d::pddi->SetCullMode(cm);


}

void FootprintManager::Update( unsigned int timeInMS )
{
    for ( int i = 0 ; i < eNumFootprintTypes ; i++ )
    {
        for ( int j = 0 ; j < m_ActiveFootprints[ i ].mUseSize ; j++ )
        {
            m_ActiveFootprints[i][j].alpha -= static_cast< float >( timeInMS )* FOOTPRINT_FADE_PER_MS;
            if ( m_ActiveFootprints[i][j].alpha < 0 )
            {
                m_ActiveFootprints[i].Remove( j );
            }
        }
    }
}
     
void FootprintManager::SetTexture( TYPE type, const char* textureName )
{
    tTexture* pTexture = p3d::find< tTexture >( textureName );
    if ( pTexture != NULL )
    {
        tRefCounted::Assign( mpTextures[type], pTexture );
    }
}

void FootprintManager::FreeTextures()
{
    for ( int i = 0 ; i < eNumFootprintTypes ; i++)
    {
        if ( mpTextures[i] != NULL )
        {
            mpTextures[ i ]->Release();
            mpTextures[ i ] = NULL;
        }
    }
}


