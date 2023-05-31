/******************************************************************************
    File:		SteeringWheel.h
    Desc:		Interface for the SteeringWheel class.

    Date:		May 16, 2003
    History:
*****************************************************************************/
#ifndef  STEERINGWHEEL_H
#define  STEERINGWHEEL_H

#include <input/RealController.h>

class SteeringWheel : public RealController
{
public: 
	SteeringWheel();
	virtual ~SteeringWheel();

    virtual void Init( IRadController* pController );
    virtual int GetDICode( int inputpoint ) const;
    bool IsPedalInverted() const { return m_bPedalInverted; }

private:
    virtual void MapInputToDICode();
    int GetIndependentDICode( int diCode ) const;

private:
    int* m_InputToDICode;
    bool m_bPedalInverted;
};
#endif
