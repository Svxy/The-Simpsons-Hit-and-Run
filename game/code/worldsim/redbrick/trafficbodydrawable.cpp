#include <worldsim/redbrick/trafficbodydrawable.h>
#include <p3d/shader.hpp>
#include <debug/profiler.h>

TrafficBodyDrawable::TrafficBodyDrawable()
{
    mBodyPropDrawable = NULL;
    mBodyShader = NULL;
    mDesiredColour.Set( 255, 255, 255, 255 );
    mFading = false;
}

TrafficBodyDrawable::~TrafficBodyDrawable()
{
    if( mBodyPropDrawable != NULL )
    {
        mBodyPropDrawable->Release();//delete mBodyPropDrawable;
        mBodyPropDrawable = NULL;
    }
    if( mBodyShader != NULL )
    {
        mBodyShader->Release();
        mBodyShader = NULL;
    }
}
void TrafficBodyDrawable::SetBodyPropDrawable( tDrawable* drawable )
{
    tRefCounted::Assign( mBodyPropDrawable, drawable );
}
void TrafficBodyDrawable::SetBodyShader( tShader* shader )
{
    tRefCounted::Assign( mBodyShader, shader );
}

///////////////////////////////////////////////////
// Implementing tDrawable
void TrafficBodyDrawable::Display()
{
    BEGIN_PROFILE("TrafficBodyDrawable::Display")
    rAssert( mBodyPropDrawable != NULL );
    if( mBodyPropDrawable != NULL )
    {
        if( mBodyShader != NULL )
        {
            // display with desired colour first, then we'll go over it with a gloss
            // put the old settings back
            if(!mFading)
            {
                mBodyShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_NONE );
            }
            else
            {
                mBodyShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
            }
            mBodyShader->SetColour( PDDI_SP_DIFFUSE, mDesiredColour );
            mBodyShader->SetInt( PDDI_SP_EMISSIVEALPHA, mFadeAlpha );
            mBodyPropDrawable->Display();

            pddiColour white( 255,255,255,255 );
            mBodyShader->SetColour( PDDI_SP_DIFFUSE, white );
            mBodyShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
            mBodyShader->SetInt( PDDI_SP_EMISSIVEALPHA, mFadeAlpha );
            mBodyShader->SetInt( PDDI_SP_ALPHATEST, 1 );
            mBodyShader->SetFloat( PDDI_SP_ALPHACOMPARE_THRESHOLD, (250.0f * (float(mFadeAlpha) / 255.0f)) / 255.0f );
            mBodyPropDrawable->Display();


            mBodyShader->SetInt( PDDI_SP_ALPHATEST, 0 );
        }
        else
        {
            mBodyPropDrawable->Display();
        }
    }
    END_PROFILE("TrafficBodyDrawable::Display")
}

void TrafficBodyDrawable::ProcessShaders(ShaderCallback& callback)
{
    rAssert( mBodyPropDrawable != NULL );
    mBodyPropDrawable->ProcessShaders(callback);
}
