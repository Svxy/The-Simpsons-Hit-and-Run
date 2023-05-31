/*===========================================================================
   geometryvehicle.h

   created Dec 7, 2001
   by Greg Mayer

   Copyright (c) 2001 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _GEOMETRYVEHICLE_HPP
#define _GEOMETRYVEHICLE_HPP

class tPose;
class tGeometry;
class tTexture;
class tShader;
class tPoseAnimationController;
class tParticleSystem;
class tAnimation;
class Vehicle;
class TrafficBodyDrawable;
class tBillboardQuadGroup;
class tFrameController;
class SkidmarkGenerator;
class tMultiController;
class StatePropCollectible;

#include <p3d/anim/compositedrawable.hpp>
#include <render/particles/vehicleparticleemitter.h>
#include <worldsim/SkidMarks/SkidMarkGenerator.h>
#include <constants/particleenum.h>
#include <vector>
#include <memory/stlallocators.h>

#define MAX_REFRACTION_SHADERS 16

struct BlobShadowParams;

struct HeadLightParams
{
    HeadLightParams( const rmt::Vector& Pos, const rmt::Vector& Normal, const rmt::Vector& Forward, const rmt::Vector& Up ) :
        GroundPos( Pos ), GroundNormal( Normal ), VehicleHeading( Forward ), VehicleUp( Up ) {};
    const rmt::Vector& GroundPos;
    const rmt::Vector& GroundNormal;
    const rmt::Vector& VehicleHeading;
    const rmt::Vector& VehicleUp;
    float LightReach; // A value of 1 is the light right at the car (i.e. car on the ground), a zero is light max reach from the car (i.e. car high in the air)
};

enum FCTrigger
{
    eNone,
    eBackfire
};

class GeometryVehicle
{
public:

    GeometryVehicle();
    ~GeometryVehicle();

    bool Init( const char* name, Vehicle* owner, int i);

    void Display();
    //void DisplaySkids();
    void UpdateSkids();
    void SetSkidValues(int wheel, float intensity, rmt::Vector& normal, eTerrainType terrainType );


    void Update(float dt);

    tPose* GetP3DPose();
    
    int  CastsShadow();
    void DisplayShadow( BlobShadowParams* BlobParams = 0 );

    // The additive lights. Headlights, etc.
    void DisplayLights( const HeadLightParams& LightParams );

    void ShowBrakeLights();
    void HideBrakeLights();  

    void ShowReverseLights();
    void HideReverseLights();

    void SetTrafficBodyColour( pddiColour colour );
    void SetShadowAdjustments( float Adjustments[ 4 ][ 2 ] );
    void SetShininess( unsigned char EnvRef ) { m_EnvRef = EnvRef; }

    void SetFadeAlpha( int fade );

    bool IsTrafficModel();

    void FadeRoof( bool fade );

    void EnableLights( bool enable );
    void SetLightsOffDueToDamage(bool lightsOffDueToDamage) {mLightsOffDueToDamage = lightsOffDueToDamage;}
    // Get the vehicle body colour, currently only valid for traffic vehicles
    tColour GetVehicleColour()const;
    bool HasVehicleColour()const { return (mTrafficBodyDrawable != NULL); }

    // Structure that holds data on how to manipulate frame controllers
    // that are tied to vehicle specific things like gas and velocity
    // The animation frame of such an object is determined by
    // frame = clamp( gasBias * currGas * brakeBias * brake + speedBias * currSpeed )
    // controller->SetFrame( frame );
    // note that brake values act as reverse gas and are in the range [0,1] like gas is
   

    struct VehicleFrameController
    {
        const char* name;
        tFrameController* frameController;
        bool frameSetter;
        FCTrigger trigger;
        float gasBias;
        float brakeBias;
        float speedBias;
        float timeBias;
    };    

    float GetHeadlightScale();
    void SetHeadlightScale( float scale );
    
    // Attach a tDrawable collectible to the vehicle
    // returns true if attached or false if not ( false happens
    // when a collectible is already attached to the vehicle)
    bool AttachCollectible( StatePropCollectible* );
    StatePropCollectible* GetAttachedCollectible();
    void DetachCollectible( const rmt::Vector& velocity, bool explode = true );

private:

    // TODO - keep like this?
    friend class Vehicle;
    
    Vehicle* mVehicleOwner;

    bool GetArt( const char* name);    // a bit obscure, but use return value to say true for localized damage textures and shaders present
    void FindAndTurnOffWheels();
    void FindAndTurnOffFrontWheelsOnly();   // for rocket car
    void FindBrakeLightBillboardJoints();
    void FindHeadLightBillboardJoints();
    void FindGhostGlowBillboards( const char* model );
    void FindRoofGeometry( const char* model );
    void FindNukeGlowBillboards( const char* model );

    void SetCollectibleHardpointPosition( const rmt::Vector& position );
    void SetCollectibleHardpointTransform( float rotx ,float roty ,float rotz, const rmt::Vector& position );

    tCompositeDrawable* mCompositeDrawable;

    // some debug shit to make the car change colour when you bottom out
    tGeometry* mChassisGeometry;
       
   
    tShader* mRefractionShader[ MAX_REFRACTION_SHADERS ];
   
    //-------------------------------------------------------
    // damage texture stuff
    // 
    // get specific test working then make a general solution
    //
    //-------------------------------------------------------

    // need pointers to the tShaders so we can swap the texture they use
    // need pointers to the textures to swap in
    // need pointers to the textures to swap back?

    // we should find these for type 1 and 2 damage


    tShader* mHoodShader;
    tShader* mTrunkShader;
    tShader* mDoorPShader;
    tShader* mDoorDShader;

    tTexture* mHoodTextureDam;
    tTexture* mTrunkTextureDam;
    tTexture* mDoorPTextureDam;
    tTexture* mDoorDTextureDam;

    tTexture* mHoodTextureNorm;
    tTexture* mTrunkTextureNorm;
    tTexture* mDoorPTextureNorm;
    tTexture* mDoorDTextureNorm;

    bool FindDamageShadersAndTextures( const char* name);  // only returns true if localized damage textures and shaders are present
    //void TriggerDamage();

    void DamageTextureDoorD(bool on);
    void DamageTextureDoorP(bool on);
    void DamageTextureHood(bool on);
    void DamageTextureTrunk(bool on);

    void HideFlappingPiece(int jointindex, bool doit);
    
    void InitParticles();

    void SetEngineSmoke(ParticleEnum::ParticleID pid);
    void SetWheelSmoke( int wheel, ParticleEnum::ParticleID pid, float bias);
    
    tShader* mChassisShader;
    tTexture* mChassisTextureNorm;
    tTexture* mChassisTextureDam;
    void DamageTextureChassis(bool on);

   
    VehicleParticleEmitter* mParticleEmitter;

    ParticleAttributes mEngineParticleAttr;   // eNull
    ParticleAttributes mLeftWheelParticleAttr;
    ParticleAttributes mRightWheelParticleAttr;
    ParticleAttributes mTailPipeParticleAttr;   // eNull

    //eEngineSmokeLight,
    //eEngineSmokeHeavy,
    
    // Particle system with variable emission rate depending on speed
    tParticleSystem* mVariableEmissionParticleSystem; 


    SkidMarkGenerator* mSkidMarkGenerator;
    void InitSkidMarks();

    // ?
 //   tPoseAnimationController* mAnimController;
   
	//tPoseAnimation* mAnim;


    //tPoseAnimationController* mAnimController;
    tAnimation* mAnim;


    float mAnimRevPerSecondBase;

    float mRevMult;

	// Special particle effects (only present on a few vehicles, like Frink or 
	// the zombie car)
	ParticleEnum::ParticleID mSpecialEffect;

    // Cache the last position of the vehicle for
    //   spherical environment map rotation based on distance travelled
    //
    rmt::Vector mLastPosition;
    float mCurEnvMapRotation;
    
    int mBrakeLightJoints[4];
    int mReverseLightJoints[4];

    TrafficBodyDrawable* mTrafficBodyDrawable;
    TrafficBodyDrawable* mTrafficDoorDrawable;

    float mShadowPointAdjustments[ 4 ][ 2 ]; // Nudge around the shadow points. See notes in the shadow display.

    int mFadeAlpha;

    enum {
        NUM_BRAKELIGHT_BBQS = 4,
#ifdef RAD_WIN32 
        NUM_FRINKARC_BBQS = 3,
#endif
        NUM_GHOSTGLOW_BBQGS = 6,
        NUM_GHOSTGLOW_BBQS = 6,
        NUM_NUKEGLOW_BBQGS = 1,
        NUM_NUKEGLOW_BBQS = 3
    };

#ifdef RAD_WIN32
    tBillboardQuadGroup* mFrinkArc;
    tColour mOriginalFrinkArcColour[ NUM_FRINKARC_BBQS   ];
#endif

    tBillboardQuadGroup* mBrakeLights[NUM_BRAKELIGHT_BBQS];
    // This relies on each quadgroup containing only one quad
    // & will store color of that quad
    tColour mOriginalBrakeLightColours[NUM_BRAKELIGHT_BBQS]; 

    bool mUsingTrafficModel;

    bool mHasGhostGlow;
    tBillboardQuadGroup* mGhostGlows[NUM_GHOSTGLOW_BBQGS];
    tColour mOriginalGhostGlowColours[NUM_GHOSTGLOW_BBQS];

    bool mHasNukeGlow;
    tBillboardQuadGroup* mNukeGlows[NUM_NUKEGLOW_BBQGS];
    tColour mOriginalNukeGlowColours[NUM_NUKEGLOW_BBQS];


    bool mBrakeLightsOn;
    float mBrakeLightScale;
    float mHeadLightScale;
    bool mEnableLights;
    bool mLightsOffDueToDamage;

    tCompositeDrawable::DrawablePropElement* mRoofOpacShape;
    tCompositeDrawable::DrawablePropElement* mRoofAlphaShape;
    tShader* mRoofShader;
    int mRoofAlpha;
    int mRoofTargetAlpha;


    std::vector< VehicleFrameController, s2alloc< VehicleFrameController > > mFrameControllers;
    void FindAnimationControllers( const char* multicontrollername );
    bool GetSpecialController( tUID name, VehicleFrameController* outSpecialController );
    void AdvanceAnimationControllers( float deltaTime );

    // A collectible that can be attached to the car
    // Used for l7m5 mission
    StatePropCollectible* m_Collectible;
    rmt::Matrix m_CollectibleTransform;

    unsigned char m_EnvRef;
};

#endif  // _GEOMETRYVEHICLE_HPP