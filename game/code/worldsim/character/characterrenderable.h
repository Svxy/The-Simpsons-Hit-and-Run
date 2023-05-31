#ifndef CHARACTERRENDERABLE_H_
#define CHARACTERRENDERABLE_H_

#include <radmath/vector.hpp>
#include <p3d/anim/pose.hpp>

class tDrawablePose;
class tShadowSkin;
class tShader;
class tTexture;
class tDrawable;

struct BlobShadowParams
{
    BlobShadowParams( const rmt::Vector& Pos, const rmt::Vector& Normal, const rmt::Vector& Facing ) :
        GroundPos( Pos ), GroundNormal( Normal ), ShadowFacing( Facing ), ShadowAlpha( 1.0f ), ShadowScale( 1.0f ) {}
    const rmt::Vector& GroundPos;
    const rmt::Vector& GroundNormal;
    const rmt::Vector& ShadowFacing;
    float ShadowAlpha;
    float ShadowScale;
};

class CharacterRenderable
{
public:
    static const int NUM_CHARACTER_SWATCHES = 5;

public:
	// Constructor
	CharacterRenderable( tDrawablePose* pDrawablePoseHigh,
        tDrawablePose* pDrawablePoseMedium,
        tDrawablePose* pDrawablePoseLow);

    ~CharacterRenderable( void );

	void Display( rmt::Vector iPosn, tPose* pose );
    
    enum CharacterLOD
    {
        Low,
        Medium,
        High,
        MAX_LOD
    };
    
    int  GetLOD( void ) const;
    void SetLOD( int LOD );
    
    tDrawablePose* GetDrawable( ) const;

    int CastsShadow();

	// If GroundPos/GroundNormal are null then it's a volumetric shadow, otherwise it's a simple shadow.
    void DisplayShadow( tPose* pose, const BlobShadowParams* BlobParams );

    void SetInAnyonesFrustrum( bool inFrustrum ) {mbInAnyonesFrustrum = inFrustrum;}

    void SetSwatch( int index );
    void SetSwatchShader( tShader* pShader );
    void SetSwatchTexture( int index, tTexture* pTexture );
    void SetShockEffect( tDrawable* pDrawable );

    void SetFadeAlpha( int fadeAlpha );

    void SetShadowColour( tColour colour );

    void SetShocked( bool isShocked ){ mIsShocked = isShocked; }
    bool GetIsShocked()const{ return mIsShocked; }

private:
	// No default construction.
	CharacterRenderable( void );
    CharacterRenderable( const CharacterRenderable& characterRenderable );

    void DisplayModel( tPose* pose );
    void DisplayShocked( tPose* pose );

    tDrawablePose* mpDrawableList[ MAX_LOD ];
    int mCurrentLOD;

    bool mbInAnyonesFrustrum;

    tTexture* mpSwatchTexture;
    tTexture* mpSwatchTextures[NUM_CHARACTER_SWATCHES];
    tShader* mpSwatchShader;

    tPose::Joint* mShadowJoints[ 4 ];
    bool mHaveShadowJoints;

    int mFadeAlpha;

    tColour mShadowColour;
    tDrawable* mpShockedDrawable;
    bool mIsShocked;
    bool mDisplayingSkeletonShock;
};


#endif // CHARACTERRENDERABLE_H_