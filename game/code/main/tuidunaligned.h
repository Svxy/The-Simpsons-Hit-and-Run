//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        game.h
//
// Description: The game loop
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef TUIDUNALIGNED_H
#define TUIDUNALIGNED_H

//========================================
// Forward References
//========================================

//
// This is a 64 bit number
//
#ifdef RAD_PS2
    typedef unsigned long radInt64;
#elif defined( RAD_WIN32 ) || defined ( RAD_XBOX )
    typedef unsigned _int64 radInt64;
#elif defined(RAD_GAMECUBE)
    typedef unsigned long long radInt64;
#else
    #error 'FTech requires definition of RAD_GAMECUBE, RAD_PS2, RAD_XBOX, or RAD_WIN32'
#endif


//=============================================================================
//
// Synopsis:    tUidUnaligned
//
//=============================================================================
class tUidUnaligned
{
public:
    tUidUnaligned();
    tUidUnaligned( const tUidUnaligned& right );
    tUidUnaligned( const radInt64 right )
    {
        const unsigned int* r = reinterpret_cast< const unsigned int* >( &right );
        u0 = r[ 0 ];
        u1 = r[ 1 ];
    }
    operator radInt64()
    {
        radInt64 returnMe;
        unsigned int* r = reinterpret_cast< unsigned int* >( &returnMe );
        r[ 0 ] = u0;
        r[ 1 ] = u1;
        return returnMe;
    }

    bool           operator !=( const tUidUnaligned  right ) const;
    bool           operator ==( const tUidUnaligned  right ) const;
    bool           operator  <( const tUidUnaligned  right ) const;
    tUidUnaligned& operator=( const tUidUnaligned&  right )
    {
        u0 = right.u0;
        u1 = right.u1;
        return *this;
    }
    tUidUnaligned  operator  ^( const tUidUnaligned  right ) const;
    tUidUnaligned  operator  &( const tUidUnaligned  right ) const;
    tUidUnaligned  operator *=( const radInt64       right );
    tUidUnaligned  operator >>( const int bits             ) const;
    
protected:
private:
    unsigned int u0;
    unsigned int u1;
};

#endif // TUIDUNALIGNED_H

