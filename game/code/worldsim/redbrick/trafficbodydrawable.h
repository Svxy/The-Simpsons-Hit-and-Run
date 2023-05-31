#ifndef TRAFFICBODYDRAWABLE_H
#define TRAFFICBODYDRAWABLE_H

#include <p3d/drawable.hpp>

class tShader;

class TrafficBodyDrawable :
    public tDrawable
{
public:
    TrafficBodyDrawable();
    ~TrafficBodyDrawable();

    void SetBodyPropDrawable( tDrawable* drawable );
    void SetBodyShader( tShader* shader );
    void SetDesiredColour( pddiColour colour )
    {
        mDesiredColour = colour;
    }
    pddiColour GetDesiredColour() const { return mDesiredColour; }

    int mFadeAlpha;
    bool mFading;

public:
    ///////////////////////////////////////////////////
    // Implementing tDrawable
    virtual void Display();
    virtual void ProcessShaders(ShaderCallback&);

private:
    tDrawable* mBodyPropDrawable;
    tShader* mBodyShader;
    pddiColour mDesiredColour;

};

#endif 