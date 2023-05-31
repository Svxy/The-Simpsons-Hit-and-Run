#ifndef BUTTON_H_
#define BUTTON_H_

// An input point, either physical (if storeig in a UserController), or logical (if stored in a Mappable
// (even joysticks are treated as "buttons" by the game)
class Button
{
public:
    Button( void );
    ~Button( void );

    // test the properties of the button
	inline float GetValue( void ) const;
    inline void  SetValue( float fValue );
	inline bool  IsDown( void ) const;
	inline bool  IsUp( void ) const;
	
    // returns number of input ticks since the value of this button changed
	inline unsigned int TimeSinceChange( void ) const; 
    
    // force the change flag to be set without resetting the value
    inline void ForceChange(void);
    
    inline static void Tick(int timeinms) { mTickCount += timeinms; }

protected:
    float mfValue;
    unsigned int mTickCountAtChange; 

    // counter for current number of input ticks (for tracking 
    static unsigned int mTickCount;
};

// Inline member functions
float Button::GetValue( void ) const
{
	return mfValue;
}

void Button::ForceChange(void)
{
    mTickCountAtChange = mTickCount;
}

void Button::SetValue( float fValue )
{
    mTickCountAtChange = mTickCount;
    mfValue = fValue;
}

bool Button::IsDown( void ) const
{
	return mfValue != 0.0f;
}

bool Button::IsUp( void ) const
{
	return mfValue == 0.0f;
}

unsigned int Button::TimeSinceChange( void ) const
{
	return mTickCount - mTickCountAtChange;
}

// nbrooke 15/10/02  :I devirtualized the Button class (there was a lot of hierarchy 
// and virtual functions considering there was only one class avtually used my any other code) 
// but didn't feel like changing a bunch of stuff that was using IButton, thus the typedef
typedef Button IButton;

#endif