//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SkidmarkManager
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <worldsim/SkidMarks/skidmarkmanager.h>
#include <memory/srrmemory.h>
#include <worldsim/SkidMarks/skidmark.h>
#include <radtime.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float ALPHA_FADE_PER_MILLISECOND = 0.001f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

SkidmarkManager* SkidmarkManager::spInstance = NULL;

//===========================================================================
// Member Functions
//===========================================================================

SkidmarkManager::ManagedSkidmark::ManagedSkidmark():
age( 0 ),
inUse( false )
{
    skidmark = new Skidmark();
    skidmark->AddRef();
}
 
SkidmarkManager::ManagedSkidmark::~ManagedSkidmark()
{
    skidmark->RemoveFromDSG();
    skidmark->Release();
}


SkidmarkManager::SkidmarkManager():
m_SkidMarks( NULL ),
m_UseTimedFadeouts( false )
{

}

SkidmarkManager::~SkidmarkManager()
{
    Destroy();
}



SkidmarkManager* SkidmarkManager::CreateInstance()
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    rAssert( spInstance == NULL );
    spInstance = new SkidmarkManager();
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    return spInstance;
}

SkidmarkManager* SkidmarkManager::GetInstance()
{
    return spInstance;
}

void SkidmarkManager::DestroyInstance()
{
    delete spInstance;
}

void SkidmarkManager::Update( unsigned int timeInMS )
{    

    const unsigned int TIME_TO_WAIT_BEFORE_FADE = 5000;
    unsigned int currentTime = radTimeGetMilliseconds();
    if ( m_UseTimedFadeouts )
    {

        float deltaAlpha = static_cast< float >( timeInMS ) * ALPHA_FADE_PER_MILLISECOND;
        // Iterate through all skidmarks, fading out the old ones that aren't in use   
        for ( int i = 0 ; i < m_NumSkidMarks ; i++ )
        {
            if ( m_SkidMarks[i]->age + TIME_TO_WAIT_BEFORE_FADE < currentTime )
            {
                if ( m_SkidMarks[i]->skidmark->IsInDSG() && m_SkidMarks[i]->inUse == false )
                {
                    m_SkidMarks[i]->skidmark->FadeOut( deltaAlpha );     
                }   
            }
        } 
    }
}

Skidmark* SkidmarkManager::GetUnusedSkidmark()
{

    Skidmark* availSkidmark = NULL;
    for ( int i = 0 ; i < m_NumSkidMarks ; i++ )
    {
        if ( m_SkidMarks[ i ]->inUse == false && m_SkidMarks[ i ]->skidmark->IsVisible() == false )
        {
            availSkidmark = m_SkidMarks[i]->skidmark;
            availSkidmark->ClearVertices();
            m_SkidMarks[ i ]->inUse = true;

            break;
        }
    }
    return availSkidmark;
}

void SkidmarkManager::ReturnUsedSkidmark( Skidmark* skidmark )
{    
    rAssert(m_SkidMarks);
    if(!m_SkidMarks)
    {
        return;
    }

    for ( int i = 0 ; i < m_NumSkidMarks ; i++ ) 
    {
        if ( m_SkidMarks[ i ]->skidmark == skidmark )
        {
            rAssert( m_SkidMarks[i]->inUse == true );
            m_SkidMarks[ i ]->inUse = false;
            m_SkidMarks[ i ]->age = radTimeGetMilliseconds();
            break;
        }
    }
}

void SkidmarkManager::Init( int numSkids )
{    

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    rAssert( m_SkidMarks == NULL );
    m_NumSkidMarks = numSkids;
    rAssert( m_SkidMarks == NULL );
    m_SkidMarks = new ManagedSkidmark*[ numSkids ];

    for ( int i = 0 ; i < m_NumSkidMarks ; i++)
    {
        m_SkidMarks[i] = new ManagedSkidmark();
    }    
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

void SkidmarkManager::Destroy()
{
    
    if ( m_SkidMarks != NULL )
    {
        for ( int i = 0 ; i < m_NumSkidMarks ; i++)
        {
            delete m_SkidMarks[i];
            m_SkidMarks[i] = NULL;
        }
        delete [] m_SkidMarks;
        m_SkidMarks = NULL;
    }

}

