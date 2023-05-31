#ifndef SKIDMARK_H
#define SKIDMARK_H

#include <render/DSG/StaticEntityDSG.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/RenderManager/RenderManager.h>


// A contigous skidmark
class Skidmark : public StaticEntityDSG
{
public:
    Skidmark();
    ~Skidmark();
    virtual void Display();
    virtual void GetBoundingBox( rmt::Box3D* box ){ *box = m_BoundingBox; } 
    virtual void GetBoundingSphere( rmt::Sphere* sphere ) { *sphere = m_BoundingBox.GetBoundingSphere(); }
    bool SpaceLeft()const { return (NUM_SEGMENTS - m_NumSegments) >= 3; }
    bool IsVisible()const;
    void Extend( const rmt::Vector& position, const rmt::Vector& forward, const rmt::Vector groundNormal, float halfWidth, float intensity );
    void ClearVertices();
    // Continue a skidmark by copying the last two vertices + texture information to the new 
    // skidmark
    void ContinueSkidmark( Skidmark* newSkidmark );
    void FadeInVertices();
    void FadeOutTrailingVertices();
    void FadeOut( float deltaAlpha );
    void AddToDSG( RenderEnums::LayerEnum renderLayer );
    void RemoveFromDSG();
    bool IsInDSG()const{ return m_IsInDSG; }
    void SetShader( tShader* pShader );
    void SetFadeIn( bool useFadeIn ){ m_FadingIn = useFadeIn; } 
    const rmt::Vector& GetCurrentDirection()const { return m_CurrentDirection; }

private:

    void WriteVertices( const rmt::Vector& elevatedPosition, 
                        const rmt::Vector& rightVector,
                        float halfWidth, 
                        unsigned char intensity,
                        float u,
                        float distance );    

    void ExtendVertices( const rmt::Vector& elevatedPosition, 
                        const rmt::Vector& rightVector,
                        float halfWidth, 
                        unsigned char intensity,
                        float u,
                        float distance );    
    void MoveInDSG( rmt::Box3D& oldBox );


    bool FindTextureBreakPosition( const rmt::Vector& position, rmt::Vector* texBreakPosition, float* u, float* distanceToTBP, float* distanceToGivenPoint );

    struct Vertex
    {
        pddiVector position;
        pddiVector2 uv;
    };

    struct Segment
    {
        Vertex left,right;
        unsigned char intensity;
        float distanceFromStart;
    };
    enum { NUM_SEGMENTS = 33 };
    int m_NumSegments;
    Segment m_Segments[ NUM_SEGMENTS ];

    tShader* mpShader;
    rmt::Box3D m_BoundingBox;
    bool m_IsInDSG;
    RenderEnums::LayerEnum m_RenderLayer;
    // The midpoint of the last position written, needed for texture coordinate generation
    rmt::Vector m_LastPositionWritten;
    float m_LastU;
    float m_TextureDirection;
    bool m_FadedOut;
    // Is this the start of the skid? If so, fade it in
    bool m_FadingIn;
    rmt::Vector m_CurrentDirection;
};

#endif
