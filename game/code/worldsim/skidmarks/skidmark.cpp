#include <worldsim/SkidMarks/skidmark.h>
#include <camera/supercammanager.h>
#include <debug/profiler.h>

const float METERS_PER_TEX_UNIT = 0.5f;
const float TEX_UNITS_PER_METER = 1.0f / METERS_PER_TEX_UNIT;



Skidmark::Skidmark():
m_NumSegments( 0 ),
mpShader( NULL ),
m_IsInDSG( false ),
m_LastU( 0.0f ),
m_TextureDirection( 1.0f ),
m_FadedOut( false ),
m_FadingIn( false )
{
    // Skidmarks are always translucent
    mTranslucent = true;
    SetName( "Skidmark" );
}

Skidmark::~Skidmark()
{
    if ( mpShader != NULL )
    {
        mpShader->Release();
        mpShader = NULL;
    }
    if ( m_IsInDSG )
    {
        RemoveFromDSG();
        m_IsInDSG = false;
    }
}

bool Skidmark::IsVisible()const
{
    if ( m_NumSegments < 2 || m_FadedOut )
        return false;

    rmt::Sphere boundingSphere = m_BoundingBox.GetBoundingSphere();

	if (GetSuperCamManager()->GetSCC( 0 )->GetCamera()->SphereVisible( boundingSphere.centre, boundingSphere.radius ) )
	{
        return true;
    }
    else
    {
        return false;
    }
}


void Skidmark::Display()
{
    if ( m_NumSegments < 2 ||  mpShader == NULL )
        return;

    BEGIN_PROFILE("Skidmark::Display")
    bool oldZWrite = p3d::pddi->GetZWrite();
    pddiCompareMode oldZComp = p3d::pddi->GetZCompare();
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( false );
    }

    pddiPrimStream* pStream = p3d::pddi->BeginPrims( mpShader->GetShader(), PDDI_PRIM_TRISTRIP, PDDI_V_CT, m_NumSegments * 2 );
    for ( int i = 0 ; i < m_NumSegments ; i++ )
    {
        tColour colour( m_Segments[i].intensity, m_Segments[i].intensity, m_Segments[i].intensity, m_Segments[i].intensity );
        pStream->Vertex( &m_Segments[ i ].left.position, colour, &m_Segments[ i ].left.uv );
        pStream->Vertex( &m_Segments[ i ].right.position, colour, &m_Segments[ i ].right.uv );
    }
    p3d::pddi->EndPrims( pStream );
    
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( true );
    }
    END_PROFILE("Skidmark::Display")
}

void Skidmark::Extend( const rmt::Vector& position, const rmt::Vector& forward, const rmt::Vector groundNormal, float halfWidth, float intensity )
{
    const float Z_FIGHTING_OFFSET = 0.1f;
    
    rAssert( SpaceLeft () );
    rmt::Vector elevatedPosition = position + (groundNormal * Z_FIGHTING_OFFSET);



    rmt::Box3D oldBoundingBox;
    GetBoundingBox( &oldBoundingBox );

    m_CurrentDirection = forward;

    unsigned char ucIntensity = static_cast< unsigned char >( intensity * 255.0f );

    // if vertices have been laid already
    //   find texture break position
    //   it is on the straight line vector from the last position to the current one
    //   extend the last two vertices laid to the texture break position
    //   the texture break position is recorded as the last position written
    //   lay down two more at the current (given position)
    if ( m_NumSegments > 0 )
    {
        // Check to see that we have travelled the minimum distance from the last 
        // point that we laid down a segment
        if ( ( elevatedPosition - m_LastPositionWritten ).MagnitudeSqr() < 0.05f )
            return;

        rmt::Vector toPosition = elevatedPosition - m_LastPositionWritten;
        toPosition.Normalize();
        rmt::Vector right;
        right.CrossProduct( toPosition, groundNormal );
        right.Normalize();

        rmt::Vector textureBreakPosition;
        float breakPointU;
        float distanceToTextureBreakPoint;
        float distanceToGivenPoint;
        if ( FindTextureBreakPosition( elevatedPosition, &textureBreakPosition, &breakPointU, &distanceToTextureBreakPoint, &distanceToGivenPoint ) )
        {
            float distanceFromStart = distanceToTextureBreakPoint + m_Segments[ m_NumSegments - 1].distanceFromStart;
           // ExtendVertices( textureBreakPosition, right, halfWidth, ucIntensity, breakPointU, distanceFromStart );
            m_LastPositionWritten = textureBreakPosition;
            // Reverse texture direction
            m_TextureDirection *= -1.0f;
            // Calculate the texture coordinate of the elevated position
            // u = distance( tbp, ep ) * TEX_UNITS_PER_METER * texturedirection
            //float distance = (textureBreakPosition - elevatedPosition).Magnitude();
            float currentU = breakPointU + distanceToTextureBreakPoint * TEX_UNITS_PER_METER * m_TextureDirection;
            if ( currentU > 1.0f ) 
                currentU = 1.0f;
            else if ( currentU < 0 )
                currentU = 0;
            m_LastU = currentU;

            distanceFromStart = distanceToGivenPoint + m_Segments[ m_NumSegments - 1].distanceFromStart;
            WriteVertices( elevatedPosition, right, halfWidth, ucIntensity, currentU, distanceFromStart ); 
            WriteVertices( elevatedPosition, right, halfWidth, ucIntensity, currentU, distanceFromStart ); 
            m_LastPositionWritten = elevatedPosition;
        }
        else
        {
            // We haven't crossed a texture break point boundary
            // extend the last two vertices written to the current position
            // then write two more vertices, also at the current position for use in 
            // the next extension
            float distance = (m_LastPositionWritten - elevatedPosition).Magnitude();
                
            float currentU = m_LastU + distance * TEX_UNITS_PER_METER * m_TextureDirection;
      
            float distanceFromStart = m_Segments[ m_NumSegments - 1 ].distanceFromStart + distance;

            ExtendVertices( elevatedPosition, right, halfWidth, ucIntensity, currentU, distanceFromStart );
            WriteVertices( elevatedPosition, right, halfWidth, ucIntensity, currentU, distanceFromStart );
            m_LastPositionWritten = elevatedPosition;
            m_LastU = currentU;
        }
    
    }
    else
    {
        rmt::Vector right;
        right.CrossProduct( forward, groundNormal );
        right.Normalize();

        // Brand new strip
        // write 4 vertices, first two are fixed, 2nd two are temporaries
        WriteVertices( elevatedPosition, right, halfWidth, ucIntensity, 0, 0 );
        WriteVertices( elevatedPosition, right, halfWidth, ucIntensity, 0, 0 );
        m_TextureDirection = 1.0f;
        m_LastU = 0;
        m_LastPositionWritten = elevatedPosition;
    }
    // Toss it in the dsg if its not there already
    if ( m_IsInDSG == false )
    {
        AddToDSG( RenderEnums::LevelSlot );
    }
    else
    {
        MoveInDSG( oldBoundingBox );
    }

}
   
void Skidmark::ClearVertices()
{
    // Reset the bounding box

    // Which direction are we laying our coordinates, either 1.0 or -1.0
    m_LastU = 0.0f;

    RemoveFromDSG();
    m_NumSegments = 0;
    m_FadedOut = false;
    m_FadingIn = false;

    m_BoundingBox.low = rmt::Vector(FLT_MAX,FLT_MAX,FLT_MAX);
    m_BoundingBox.high = rmt::Vector(-FLT_MAX,-FLT_MAX,-FLT_MAX);

}

void Skidmark::ContinueSkidmark( Skidmark* newSkidmark )
{
    tRefCounted::Assign( newSkidmark->mpShader, mpShader );
    newSkidmark->ClearVertices();
    newSkidmark->m_LastPositionWritten = m_LastPositionWritten;
    newSkidmark->m_Segments[ 0 ] = m_Segments[ m_NumSegments - 1 ];
    newSkidmark->m_Segments[ 1 ] = m_Segments[ m_NumSegments - 2 ];
    newSkidmark->m_NumSegments = 2;
    newSkidmark->m_LastU = m_LastU;
    newSkidmark->m_TextureDirection = m_TextureDirection;
    newSkidmark->m_CurrentDirection = m_CurrentDirection;
    rmt::Box3D oldBox;
    GetBoundingBox( &oldBox );

    newSkidmark->m_BoundingBox.Expand( newSkidmark->m_Segments[ 0 ].left.position );
    newSkidmark->m_BoundingBox.Expand( newSkidmark->m_Segments[ 0 ].right.position );
    newSkidmark->m_BoundingBox.Expand( newSkidmark->m_Segments[ 1 ].left.position );
    newSkidmark->m_BoundingBox.Expand( newSkidmark->m_Segments[ 1 ].right.position );

    m_NumSegments--;

}

void Skidmark::FadeInVertices()
{
    const float DISTANCE_TO_FADE_IN = 1.0f;

    // We want to fade in the last few vertices, but we don't want to make the fade greater in intensity than
    // existing ones, so check before writing new values

    if ( m_NumSegments < 1 )
        return;

    for ( int i = 0 ; i < m_NumSegments ; i++ )
    {
        int intensity = static_cast< int >( m_Segments[ i ].distanceFromStart / DISTANCE_TO_FADE_IN * 255.0f );
        if ( m_Segments[ i ].intensity > intensity )
            m_Segments[ i ].intensity = intensity;
    }

}

void Skidmark::FadeOutTrailingVertices()
{
    const float DISTANCE_TO_FADE_OUT = 1.0f;

    rAssert(m_NumSegments != 0); // happened to me once, probably bad, you can click 
                                 // through okay now, but we might want to fix it at some point
                                 // nbrooke, apr 11, 2003
    if(m_NumSegments == 0)
        return;

    // We want to fade out the last few vertices, but we don't want to make the fade greater in intensity than
    // existing ones, so check before writing new values

    // Last segment is zero
    // next to last is -1.dist - this.dist

    float distFadedOutSoFar = 0;
    m_Segments[ m_NumSegments - 1 ].intensity = 0; 
    int intensity = 0;

    for ( int i = m_NumSegments - 2 ; i >= 0 ; i-- )
    {
        distFadedOutSoFar += m_Segments[ i + 1 ].distanceFromStart  - m_Segments[ i ].distanceFromStart;
        intensity = static_cast< int >( distFadedOutSoFar / DISTANCE_TO_FADE_OUT * 255.0f );
        if ( m_Segments[ i ].intensity > intensity )
            m_Segments[ i ].intensity = intensity;
    }

}

void Skidmark::FadeOut( float deltaAlpha )
{
    int iDeltaAlpha = static_cast< unsigned char > ( deltaAlpha * 255.0f );
    
    bool stillVisible = false;
    for ( int i = 0 ; i < m_NumSegments ; i++ )
    {
        int newAlpha = m_Segments[i].intensity - iDeltaAlpha;
        if ( newAlpha <= 0 )
        {
            newAlpha = 0;
        }
        else
        {
            stillVisible = true;
        }
        m_Segments[ i ].intensity = newAlpha;
    }
    if ( stillVisible == false )
        m_FadedOut = true;
}

void Skidmark::AddToDSG( RenderEnums::LayerEnum renderLayer )
{
    if ( m_IsInDSG )
        RemoveFromDSG();
    
    m_RenderLayer = renderLayer;
    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( m_RenderLayer ));
    rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );    
    pWorldRenderLayer->pWorldScene()->Add( this );
    m_IsInDSG = true;

}

void Skidmark::RemoveFromDSG()
{       

    if ( m_IsInDSG )
    {
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( m_RenderLayer ));
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );    
        pWorldRenderLayer->pWorldScene()->Remove( this );
        m_IsInDSG = false;
    }
}

void Skidmark::SetShader( tShader* pShader )
{
    tRefCounted::Assign( mpShader, pShader );
}

void Skidmark::WriteVertices( const rmt::Vector& elevatedPosition, 
                              const rmt::Vector& right,
                              float halfWidth, 
                              unsigned char intensity,
                              float u,
                              float distanceFromStart )
{


    rmt::Vector leftVertex = elevatedPosition - (right * halfWidth);
    rmt::Vector rightVertex = elevatedPosition + (right * halfWidth);

    // Increase bounding box
    m_BoundingBox.Expand( rightVertex );
    m_BoundingBox.Expand( leftVertex );  


    // Set position data
    m_Segments[ m_NumSegments ].left.position = leftVertex;
    m_Segments[ m_NumSegments ].right.position = rightVertex;


    // Intensity
    m_Segments[ m_NumSegments ].intensity = intensity;

    // Tex coordinates
    m_Segments[ m_NumSegments ].left.uv = pddiVector2( 0.0f, u );
    m_Segments[ m_NumSegments ].right.uv = pddiVector2( 1.0f, u );

    m_Segments[ m_NumSegments ].distanceFromStart = distanceFromStart;

   m_NumSegments++;
}

void Skidmark::ExtendVertices( const rmt::Vector& elevatedPosition, 
                               const rmt::Vector& right,
                               float halfWidth, 
                               unsigned char intensity,
                               float u,
                               float distance )
{

    rmt::Vector leftVertex = elevatedPosition - (right * halfWidth);
    rmt::Vector rightVertex = elevatedPosition + (right * halfWidth);

    // Increase bounding box
    m_BoundingBox.Expand( rightVertex );
    m_BoundingBox.Expand( leftVertex );  

    // Set position data
    m_Segments[ m_NumSegments - 1 ].left.position = leftVertex;
    m_Segments[ m_NumSegments - 1 ].right.position = rightVertex;

    // Intensity
    m_Segments[ m_NumSegments - 1 ].intensity = intensity;

    // Tex coordinates      
    m_Segments[ m_NumSegments -1 ].left.uv = pddiVector2( 0.0f, u );
    m_Segments[ m_NumSegments -1 ].right.uv = pddiVector2( 1.0f, u );

    m_Segments[ m_NumSegments - 1].distanceFromStart = distance;
}

bool Skidmark::FindTextureBreakPosition( const rmt::Vector& position, rmt::Vector* texBreakPosition, float* u, float* distanceToTextureBreakPoint, float* distanceToGivenPoint )
{
    // Calculate the point on the vector from the last vertex written
    rmt::Vector toPosition = position - m_LastPositionWritten;
    toPosition.Normalize();
    float deltaU;
    float breakTexCoordinate;
    if ( m_TextureDirection > 0 )
    {
        deltaU = 1.0f - m_LastU;
        breakTexCoordinate = 1.0f;
    }
    else
    {
        deltaU = m_LastU - 0.0f;
        breakTexCoordinate = 0.0f;
    }

    float distance = METERS_PER_TEX_UNIT * deltaU;
    // Check to see that the break position is actually between the last position and
    // the given position
    *texBreakPosition = m_LastPositionWritten + toPosition * distance;
    *u = breakTexCoordinate;

    float distanceToBreakPointSqr = ( *texBreakPosition - m_LastPositionWritten ).MagnitudeSqr();
    float distanceToGivenPointSqr = ( position - m_LastPositionWritten ).MagnitudeSqr();
    if ( distanceToGivenPointSqr > distanceToBreakPointSqr )
    {
        *distanceToTextureBreakPoint = rmt::Sqrt ( distanceToBreakPointSqr );
        *distanceToGivenPoint = rmt::Sqrt ( distanceToGivenPointSqr );
        return true;
    }
    else
        return false;

}

void Skidmark::MoveInDSG( rmt::Box3D& oldBox )
{
    if ( m_IsInDSG )
    {
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( m_RenderLayer ));
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );    
        pWorldRenderLayer->pWorldScene()->Move( oldBox, this );            
    }
}


