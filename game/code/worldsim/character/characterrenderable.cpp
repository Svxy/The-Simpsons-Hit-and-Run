#include <worldsim/character/characterrenderable.h>
#include <mission/gameplaymanager.h>
#include <p3d/texture.hpp>
#include <p3d/shader.hpp>
#include <p3d/anim/drawablepose.hpp>
#include <p3d/anim/pose.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/shadow.hpp>
#include <p3d/view.hpp>
#include <pddi/pddi.hpp>
#include <camera/supercammanager.h>
#include <contexts/bootupcontext.h>

// Hack.
//  [6/27/2002]
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>

/*
==============================================================================
CharacterRenderable::CharacterRenderable
==============================================================================
Description:    Comment

Parameters:     ( 
        tDrawablePose* pDrawablePoseHigh,
        tDrawablePose* pDrawablePoseMedium,
        tDrawablePose* pDrawablePoseLow, 
        tPose* pPose )

Return:         CharacterRenderable

=============================================================================
*/
CharacterRenderable::CharacterRenderable( 
        tDrawablePose* pDrawablePoseHigh,
        tDrawablePose* pDrawablePoseMedium,
        tDrawablePose* pDrawablePoseLow)
:
mCurrentLOD( High ),
mbInAnyonesFrustrum( false ),
mpSwatchTexture( NULL ),
mpSwatchShader( NULL ),
mHaveShadowJoints( false ),
mFadeAlpha( 255 ),
mpShockedDrawable( NULL ),
mIsShocked( false ),
mDisplayingSkeletonShock( false )
{
    int i;
    for ( i = 0; i < MAX_LOD; i++ )
    {
        mpDrawableList[ i ] = 0;
    }
    tRefCounted::Assign( mpDrawableList[ High ], pDrawablePoseHigh );
    tRefCounted::Assign( mpDrawableList[ Medium ], pDrawablePoseMedium );
    tRefCounted::Assign( mpDrawableList[ Low ], pDrawablePoseLow );

    if(pDrawablePoseHigh)
    {
        pDrawablePoseHigh->SetPose(NULL);
    }

    if(pDrawablePoseMedium)
    {
        pDrawablePoseMedium->SetPose(NULL);
    }

    if(pDrawablePoseLow)
    {
        pDrawablePoseLow->SetPose(NULL);
    }

    // null these out for now... CharacterManager::SetupCharacter will be 
    // setting these swatch textures...
    for( i=0; i<CharacterRenderable::NUM_CHARACTER_SWATCHES; i++ )
    {
        mpSwatchTextures[i] = NULL;
    }

    mShadowColour.Set( 0, 0, 0 );
}

/*
==============================================================================
CharacterRenderable::~CharacterRenderable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterRenderable

=============================================================================
*/
CharacterRenderable::~CharacterRenderable( void )
{
    int i;
    for ( i = 0; i < MAX_LOD; i++ )
    {
        if ( mpDrawableList[ i ] )
        {
            mpDrawableList[ i ]->Release();
            mpDrawableList[ i ] = 0;
        }
    }

    if( mpSwatchTexture )
    {
        mpSwatchTexture->Release();
        mpSwatchTexture = NULL;
    }

    for( i=0; i<CharacterRenderable::NUM_CHARACTER_SWATCHES; i++ )
    {
        if( mpSwatchTextures[i] != NULL )
        {
            mpSwatchTextures[i]->Release();
            mpSwatchTextures[i] = NULL;
        }
    }
    if( mpSwatchShader )
    {
        mpSwatchShader->Release();
        mpSwatchShader = NULL;
    }

    if ( mpShockedDrawable != NULL )
    {
        mpShockedDrawable->Release();
        mpShockedDrawable = NULL;
    }
}
/*
==============================================================================
CharacterRenderable::Display
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void CharacterRenderable::Display( rmt::Vector iPosn, tPose* pose )
{
BEGIN_PROFILE("CharRender Cull")
    tPointCamera* pCam = (tPointCamera*)GetSuperCamManager()->GetSCC(0)->GetCamera();
    rmt::Vector camPosn;
    pCam->GetPosition( &camPosn );
    iPosn.Sub( iPosn, camPosn );
    float sqrDistFromCam = iPosn.MagnitudeSqr();
END_PROFILE("CharRender Cull")

    float dist = (pCam->GetNearPlane() + 1.5f);
    dist *= dist;

    if ( sqrDistFromCam < dist )
    {
        return;
    }

    if( mbInAnyonesFrustrum )
    {

        if(sqrDistFromCam > 900.0f )
        {
            if(sqrDistFromCam > 6400.0f ) //>80m
            {
                SetLOD(Low);
            }
            else // <= 80m
            {
                SetLOD(Medium);
            }
        }
        else // <= 30m
        {
            SetLOD(High);
        }

    BEGIN_PROFILE("CharRender Display")
        if ( mIsShocked == false )
        {
            DisplayModel( pose );
        }
        else
        {
            DisplayShocked( pose );
        }

    END_PROFILE("CharRender Display")
    }
}
//////////////////////////////////////////////////////////////////////////
int CharacterRenderable::CastsShadow()
{
    return 1;
}

/*
==============================================================================
CharacterRenderable::DisplayShadow
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void CharacterRenderable::DisplayShadow( tPose* pose, const BlobShadowParams* BlobParams )
{
	if( BlobParams )
	{
        BEGIN_PROFILE("Char Blobby Shadow");
        if( !mHaveShadowJoints )
        {
            mHaveShadowJoints = true;
            mShadowJoints[ 0 ] = pose->FindJoint( "Elbow_L" );
            mShadowJoints[ 1 ] = pose->FindJoint( "Ankle_L" );
            mShadowJoints[ 2 ] = pose->FindJoint( "Elbow_R" );
            mShadowJoints[ 3 ] = pose->FindJoint( "Ankle_R" );
        }

        tColour OutsideColour;
        tColour insideColour;

        // Hack for brightly colored shadows for special game mode
        if( ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
        {
            OutsideColour.Set( 0, 0, 0, 0 ); // black outline
            //OutsideColour = mShadowColour;
            insideColour = mShadowColour;
        }
        else
        {
            OutsideColour.Set( 255, 255, 255, 255 );
            const int Inside = 128;
            float shadowAlpha = BlobParams->ShadowAlpha;
            if( mFadeAlpha != 255 )
            {
                shadowAlpha = mFadeAlpha / 255.0f;
            }
            int c = rmt::Clamp( int( Inside + ( 255 - Inside ) * ( 1.0f - shadowAlpha ) ), 0, 255 );
            insideColour.Set( c, c, c, c );
        }


        const float ArticulatedBlobbyShadowLODThreshold = 15.0f;
        const float MinShadowSize = 0.15f;
        const float NonArticulatedScale = 0.2f;
        const int NumShadowJoints = 4;


        const float BlobRadius = 1.0f;
        const float BlobFadeOff = 0.2f;
		const int NumBlobSlices = 16; // Keep the number even or you're in trouble.
        const int HalfCircle = NumBlobSlices >> 1; // We only keep half the circle and then mirror it. Hence the reason to keep the number of points even.
		static float BlobPoints[ HalfCircle ][ 2 ];
        static float FadePoints[ HalfCircle ][ 2 ];
        float pointScales[ NumBlobSlices ]; // How much to move the points to envolope the character.
        bool articulated = false;

        static float DoOnce = true;
        if( DoOnce )
        {
            DoOnce = false;
			const float angleStep = rmt::PI_2 / ((float) NumBlobSlices );
            const float angleHalfStep = angleStep * 0.5f;
			float angle = rmt::PI_2;
			for( int j = 0; j < HalfCircle; ++j )
			{
				float sa, ca;
				rmt::SinCos( angle, &sa, &ca);
				BlobPoints[ j ][ 0 ] = sa * BlobRadius;
				BlobPoints[ j ][ 1 ] = ca * BlobRadius;
                rmt::SinCos( angle - angleHalfStep, &sa, &ca );
   				FadePoints[ j ][ 0 ] = sa * BlobFadeOff;
    			FadePoints[ j ][ 1 ] = ca * BlobFadeOff;
				angle -= angleStep;
			}
        }

		// Translate the shadow towards the camera slightly, instead of moving it off the
		//ground in the direction of the the ground normal. Hopefully this will cause less distortion of the shadow.
		rmt::Vector camPos;
        tCamera* camera = p3d::context->GetView()->GetCamera();
        rAssert( camera );
		camera->GetWorldPosition( &camPos );
		camPos.Sub( BlobParams->GroundPos );
        float camDirDot = camPos.DotProduct( camera->GetCameraToWorldMatrix().Row( 2 ) );
        // Note that the camPos vector is towards the camera so the camDirDot will be negative when the camera is facing
        //the object. So if it's positive then we can assume it's behind the camera.
        if( camDirDot > 0.0f )
        {
            // Please excuse the early return.
            END_PROFILE( "Char Blobby Shadow" );
            return;
        }
        float cameraToShadow = camPos.Magnitude();
		camPos.Normalize();
        camPos.Scale( 0.25f );
		rmt::Matrix transform;
		transform.Identity();
		transform.FillTranslate( BlobParams->GroundPos );
        transform.FillHeading( BlobParams->GroundNormal, BlobParams->ShadowFacing );
		transform.Row( 3 ).Add( camPos );
		p3d::stack->PushMultiply( transform );

        if( ( mHaveShadowJoints ) && ( cameraToShadow < ArticulatedBlobbyShadowLODThreshold ) )
        {
            articulated = true;
            // Find the position of the joints to deform the shadow by the character's movement.
            float jointPos[ NumShadowJoints ][ 2 ];
/*
            Uncomment this if you want to weight the joint position by how close
            they line up to the blob point position.
            float jointNor[ NumShadowJoints ][ 2 ];
*/
            rmt::Vector tempPos;
            rmt::Matrix m = pose->GetJoint( 0 )->worldMatrix;
            m.Invert();
            for( int i = 0; i < NumShadowJoints; ++i )
            {
                m.Transform( mShadowJoints[ i ]->worldMatrix.Row( 3 ), &tempPos );
                jointPos[ i ][ 0 ] = tempPos.x;
                jointPos[ i ][ 1 ] = -tempPos.z;
/*
                Uncomment this if you want to weight the joint positions by how close
                they line up to the blob point position.
                float mag;
                mag = rmt::Sqrt( ( tempPos.x * tempPos.x ) + ( tempPos.z * tempPos.z ) );
                mag = ( mag != 0.0f ) ? 1 / mag : 0.0f;
                jointNor[ i ][ 0 ] = jointPos[ i ][ 0 ] * mag;
                jointNor[ i ][ 1 ] = jointPos[ i ][ 1 ] * mag;
*/
            }

            for( int i = 0; i < HalfCircle; ++i )
            {
                pointScales[ i ] = MinShadowSize;
                pointScales[ i + HalfCircle ] = -MinShadowSize;

                for( int j = 0; j < NumShadowJoints; ++j )
                {
                    float dot;
                    // Project joint position onto blob point position.
                    dot = ( BlobPoints[ i ][ 0 ] * jointPos[ j ][ 0 ] ) + ( BlobPoints[ i ][ 1 ] * jointPos[ j ][ 1 ] );
/*
                    Uncomment this if you want to weight the joint position by how close
                    they line up to the blob point position.
                    dot *= rmt::Abs( ( BlobPoints[ i ][ 0 ] * jointNor[ j ][ 0 ] ) + ( BlobPoints[ i ][ 1 ] * jointNor[ j ][ 1 ] ) );
*/
                    pointScales[ i ] = rmt::Max( pointScales[ i ], dot );
                    pointScales[ i + HalfCircle ] = rmt::Min( pointScales[ i + HalfCircle ], dot );
                }
            }
            // Since we'll be mirroring the circle, we'll conviently keep the second half of the scales array
            //as negative numbers so that it will automatically mirror our points for us.
        }

        pddiPrimStream* blob = 0;
 
        pddiShader* blobShader = BootupContext::GetInstance()->GetSharedShader();
        rAssert( blobShader != NULL );

        if( ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
        {
            blobShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_NONE );
        }
        else
        {
            blobShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_MODULATE );
        }
        blobShader->SetInt( PDDI_SP_ISLIT, 0 );
        blobShader->SetInt( PDDI_SP_ALPHATEST, 0 );
        blobShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_GOURAUD );

        blob  = p3d::pddi->BeginPrims( blobShader, PDDI_PRIM_TRIANGLES, PDDI_V_C, 9 * NumBlobSlices );
        for(int i=0; i < NumBlobSlices; i++)
        {
            float x0, y0, x1, y1, x2, y2, x3, y3;
            int index = i % HalfCircle;
			int nextIndex = ( i + 1 ) % HalfCircle;
            int nextScaleIndex = ( i + 1 ) % NumBlobSlices;
            float fadeScale = ( i < HalfCircle ) ? BlobParams->ShadowScale : -BlobParams->ShadowScale;
            float nextFadeScale = ( nextScaleIndex < HalfCircle ) ? BlobParams->ShadowScale : -BlobParams->ShadowScale;

            // Draw inside.
            blob->Colour( insideColour );
            blob->Coord( 0.0f, 0.0f, 0.0f );

            blob->Colour( insideColour );
            x0 = BlobPoints[ index ][ 0 ];
            x0 *= BlobParams->ShadowScale;
            x0 *= articulated ? pointScales[ i ] : ( index == i ) ? NonArticulatedScale : -NonArticulatedScale;
            y0 = BlobPoints[ index ][ 1 ];
            y0 *= BlobParams->ShadowScale;
            y0 *= articulated ? pointScales[ i ] : ( index == i ) ? NonArticulatedScale : -NonArticulatedScale;
            blob->Coord( x0, y0, 0.0f );

            blob->Colour( insideColour );
            x1 = BlobPoints[ nextIndex ][ 0 ];
            x1 *= BlobParams->ShadowScale;
            x1 *= articulated ? pointScales[ nextScaleIndex ] : ( nextIndex == nextScaleIndex ) ? NonArticulatedScale : -NonArticulatedScale;
            y1 = BlobPoints[ nextIndex ][ 1 ];
            y1 *= BlobParams->ShadowScale;
            y1 *= articulated ? pointScales[ nextScaleIndex ] : ( nextIndex == nextScaleIndex ) ? NonArticulatedScale : -NonArticulatedScale;
            blob->Coord( x1, y1, 0.0f );

            // Draw first part of outside.
            blob->Colour( insideColour );
            blob->Coord( x0, y0, 0.0f );

            blob->Colour( OutsideColour );
            x2 = x1 + ( FadePoints[ nextIndex ][ 0 ] * nextFadeScale );
            y2 = y1 + ( FadePoints[ nextIndex ][ 1 ] * nextFadeScale );
            blob->Coord( x2, y2, 0.0f );

            blob->Colour( insideColour );
            blob->Coord( x1, y1, 0.0f );

            // Draw second part of outside.
            blob->Colour( insideColour );
            blob->Coord( x0, y0, 0.0f );

            blob->Colour( OutsideColour );
            x3 = x0 + ( FadePoints[ index ][ 0 ] * fadeScale );
            y3 = y0 + ( FadePoints[ index ][ 1 ] * fadeScale );
            blob->Coord( x3, y3, 0.0f );

            blob->Colour( OutsideColour );
            blob->Coord( x2, y2, 0.0f );
        }
        p3d::pddi->EndPrims( blob );

        p3d::stack->Pop();
        END_PROFILE( "Char Blobby Shadow" );
	}
}
/*
==============================================================================
CharacterRenderable::GetDrawable
==============================================================================
Description:    Comment

Parameters:     ( )

Return:         tDrawablePose

=============================================================================
*/
tDrawablePose* CharacterRenderable::GetDrawable( ) const
{
    for(int lod = (int)mCurrentLOD; lod >= (int)Low; lod--)
    {
        if(mpDrawableList[ lod ] )
        {
            return mpDrawableList[ lod ];
        }
    }
    return NULL;
}

/*
==============================================================================
CharacterRenderable::GetLOD
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterRenderable

=============================================================================
*/
int CharacterRenderable::GetLOD( void ) const
{
    return mCurrentLOD;
}
/*
==============================================================================
CharacterRenderable::SetLOD
==============================================================================
Description:    Comment

Parameters:     ( CharacterLOD LOD )

Return:         void 

=============================================================================
*/
void CharacterRenderable::SetLOD( int LOD )
{
    mCurrentLOD = (LOD > High) ? LOD : High;
}

void CharacterRenderable::SetSwatch( int index )
{
    rAssert( 0 <= index && index < NUM_CHARACTER_SWATCHES );
    if( mpSwatchTextures[index] != NULL && mpSwatchTexture != mpSwatchTextures[index] )
    {
        // TODO: 
        // Assign here? or just set mpSwatchTexture = mpSwatchTextures[index]?
        tRefCounted::Assign(mpSwatchTexture, mpSwatchTextures[index]);
    }
}

void CharacterRenderable::SetSwatchTexture( int index, tTexture* pTexture )
{
    rAssert( 0 <= index && index < NUM_CHARACTER_SWATCHES );
    tRefCounted::Assign(mpSwatchTextures[index], pTexture);
}

void CharacterRenderable::SetShockEffect( tDrawable* pDrawable )
{
    tRefCounted::Assign(mpShockedDrawable, pDrawable);    
}


void CharacterRenderable::SetSwatchShader( tShader* pShader )
{
    tRefCounted::Assign(mpSwatchShader, pShader);
}

void CharacterRenderable::SetFadeAlpha( int fadeAlpha )
{
    mFadeAlpha = rmt::Clamp( fadeAlpha, 0, 255 );
}

void CharacterRenderable::SetShadowColour( tColour colour )
{
    mShadowColour = colour;
}

void CharacterRenderable::DisplayModel( tPose* pose )
{
    tDrawablePose* draw = GetDrawable();
    if(draw)
    {
        if( mpSwatchShader != NULL )
        {
            if( mpSwatchTexture != NULL )
            {
                mpSwatchShader->SetTexture( PDDI_SP_BASETEX, mpSwatchTexture );
            }
        }
        tShaderIntBroadcast blendAlpha( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
        draw->ProcessShaders( blendAlpha );
        tShaderIntBroadcast emissiveFade( PDDI_SP_EMISSIVEALPHA, mFadeAlpha );
        draw->ProcessShaders( emissiveFade );
        draw->Display( pose );
    }
}


void CharacterRenderable::DisplayShocked( tPose* pose )
{
    // Alternate displaying the skeleton and displaying the model
    if ( mDisplayingSkeletonShock ) 
    {
        if ( mpShockedDrawable != NULL )
        {
            p3d::stack->PushMultiply( pose->GetJoint(0)->worldMatrix );
            mpShockedDrawable->Display();
            p3d::stack->Pop();
        }
        mDisplayingSkeletonShock = false;
    }
    else
    {

        //pose->GetSkeleton();
        //DisplayModel( pose );
        mDisplayingSkeletonShock = true;
    }
}

