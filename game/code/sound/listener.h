//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        listener.h
//
// Description: Declaration of Listener class, which tracks the camera and
//              updates the RadSound listener position/orientation to match
//
// History:     02/08/2002 + Created -- Darren
//
//=============================================================================

#ifndef LISTENER_H
#define LISTENER_H

//========================================
// Nested Includes
//========================================
#include <contexts/contextenum.h>
#include <sound/soundrenderer/soundrenderingmanager.h>

//========================================
// Forward References
//========================================

struct IRadSoundHalListener;
class tVectorCamera;

//=============================================================================
//
// Synopsis:    Listener
//
//=============================================================================

class Listener
{
    public:
        Listener();
        virtual ~Listener();

        void Initialize( Sound::daSoundRenderingManager& renderMgr );
        void Update( ContextEnum context );

    private:
        //Prevent wasteful constructor creation.
        Listener( const Listener& original );
        Listener& operator=( const Listener& rhs );

        IRadSoundHalListener* m_theListener;

        tVectorCamera* m_feCamera;
};


#endif // LISTENER_H

