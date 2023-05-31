//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PS2Platform   
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//
//===========================================================================

//========================================
// System Includes
//========================================
// Libraries
#include <string.h>
#include <stdio.h>
// Foundation
#include <radtime.hpp>
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
// Pure3D
#include <pddi/pddi.hpp>
#include <p3d/p3dtypes.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <debug/profiler.h>
#include <memory/srrmemory.h>


#ifdef PROFILER_ENABLED

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
Profiler* Profiler::spInstance = NULL;

int Profiler::sRed = 0;
int Profiler::sGreen = 0;
int Profiler::sBlue = 0;
int Profiler::sPage = 0;
int Profiler::sLeftOffset = 0;
int Profiler::sTopOffset = 0;
bool Profiler::sDisplay = false;
bool Profiler::sDumpToOutput = false;
bool Profiler::sEnableCollection = false;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Profiler::CreateInstance
//==============================================================================
//
// Description: Creates the Profiler.
//
// Parameters:	None.
//
// Return:      Pointer to the Profiler.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Profiler* Profiler::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "Profiler" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_DEBUG) Profiler;
    rAssert( spInstance );
MEMTRACK_POP_GROUP( "Profiler" );
    
    return spInstance;
}

//==============================================================================
// Profiler::GetInstance
//==============================================================================
//
// Description: - Access point for the Profiler singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the Profiler.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Profiler* Profiler::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// Profiler::DestroyInstance
//==============================================================================
//
// Description: Destroy the Profiler.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void Profiler::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_DEBUG, spInstance );
    spInstance = NULL;
}



//==============================================================================
// Profiler::Init
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::Init()
{
}



//==============================================================================
// Profiler::AllocSample
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return: the next available sample or NULL     
//
//==============================================================================
Profiler::ProfileSample *Profiler::AllocSample(void)
{
    if (mNextSampleAllocIndex >= NUM_SAMPLES) return NULL;

    return &mSamples[mNextSampleAllocIndex++];
}            

//==============================================================================
// Profiler::AllocHistory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return: the next available history or NULL     
//
//==============================================================================
Profiler::ProfileSampleHistory *Profiler::AllocHistory(void)
{
    if (mNextHistoryAllocIndex >= NUM_SAMPLES) return NULL;

    return &mHistory[mNextHistoryAllocIndex++];
}            

//==============================================================================
// Profiler::BeginFrame
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::BeginFrame()
{
    int i;
    for( i = 0; i < NUM_SAMPLES; ++i ) 
    {
        mSamples[i].bValid = false;
    }

    mOpenStackTop = -1;
    mNextSampleAllocIndex = 0;

    mStartProfile = ((float)radTimeGetMicroseconds()) / 1000.0F;

    mOpenSampleStore->RemoveAll();

    //
    // Only collect profile data if we're actually going to show it on the screen.
    // Check it here so that we don't turn it on/off in mid-sample.
    //
    sEnableCollection = sDisplay;
}


//==============================================================================
// Profiler::EndFrame
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::EndFrame()
{
    unsigned int i = 0;

    if( !sEnableCollection )
    {
        return;
    }

    mEndProfile = ((float)radTimeGetMicroseconds()) / 1000.0F;

    while( i < mNextSampleAllocIndex)
    {
        if (!mSamples[i].bValid) continue;
        float sampleTime, percentTime;
            
        rAssertMsg(!mSamples[i].isOpen, "ProfileEnd() called without a ProfileBegin()" );

        sampleTime = mSamples[i].fAccumulator - mSamples[i].fChildrenSampleTime;

        float profileTime = mEndProfile - mStartProfile;

        if( profileTime < 0.001F )
        {
            percentTime = 0.0f;
        }
        else
        {
            percentTime = ( sampleTime / profileTime ) * 100.0f;
            rAssert( percentTime <= 100.0f );
        }

        //
        // Add new measurement into the history and get the ave, min, and max
        //
//        this->StoreProfileHistory( mSamples[i].szName, 
//                                   percentTime, 
//                                   mEndProfile - mStartProfile, 
//                                   mSamples[i].uiSampleTime,    // single sample
//                                   mSamples[i].uiAccumulator ); // total sample
        this->StoreProfileHistory( mSamples[i].uid, 
                                   percentTime, 
                                   profileTime, 
                                   mSamples[i].fSampleTime,    // single sample
                                   mSamples[i].fAccumulator ); // total sample
        i++;
    }
}


//==============================================================================
// Profiler::BeginProfile
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::BeginProfile( const char* name )
{
    int i;
    tUID nameUID;

    if( !sEnableCollection )
    {
        return;
    }

    i = 0;
    nameUID = tEntity::MakeUID( name );

    // If the sample is alread open this frame it will be in the store
    ProfileSample *sample = mOpenSampleStore->Find(nameUID);

    // Sample isn't in the store.  Alloc a new one, set it up, and store it
    if (sample == NULL)
    {
        sample = AllocSample();
        rAssertMsg(sample != NULL, "Exceeded Maximum Available Profile Samples." );
        
        sample->bValid              = true;
        sample->uid                 = nameUID;
        sample->iProfileInstances   = 0;
        sample->fAccumulator        = 0.0F;
        sample->fChildrenSampleTime = 0.0F;
        sample->fSampleTime         = 0.0F;
        sample->fTotalTime          = 0.0F;
        sample->iNumParents         = 0;
        sample->isOpen              = false;
            
        strncpy(sample->szName, name, 255);

        mOpenSampleStore->Store(nameUID, sample);
    }

    rAssertMsg(!sample->isOpen, "Profiler section error: Started a second time without closing.");

    sample->iProfileInstances++;
    sample->isOpen      = true;
    sample->fStartTime = ((float)radTimeGetMicroseconds()) / 1000.0F;

    // Ahh, pointer math.
    unsigned sampleIndex = sample - mSamples;

    // Add this sample to the open stack
    rAssertMsg(mOpenStackTop < (MAX_PROFILER_DEPTH - 1), "Profiler error: Too many levels deep.");
    ++mOpenStackTop;
    mOpenStack[mOpenStackTop] = sampleIndex;
}


//==============================================================================
// Profiler::EndProfile
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//  
//==============================================================================
void Profiler::EndProfile( const char* name )
{
    tUID nameUID;

    if( !sEnableCollection )
    {
        return;
    }

    nameUID = tEntity::MakeUID( name );
 
    ProfileSample *sample = mOpenSampleStore->Find(nameUID);
    rAssertMsg(sample != NULL, "Profiler error: Ending a section that isn't open.");
    rAssertMsg(sample->isOpen, "Profiler error: Ending a section that isn't open.");

    float endTime = ((float)radTimeGetMicroseconds()) / 1000.0F;
    float duration = endTime - sample->fStartTime;
 
    sample->isOpen        = false;
    sample->iNumParents   = mOpenStackTop;
    sample->fAccumulator += duration;
    sample->fSampleTime   = duration;

    // Pop the stack
    mOpenStackTop--;

    // Only update the parent if there is one
    if (mOpenStackTop > 0)
    {
        unsigned int parentIndex = mOpenStack[mOpenStackTop];
        mSamples[parentIndex].fChildrenSampleTime += duration;
    }
}



//==============================================================================
// Profiler::Render
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::Render(void)
{
    const int LEFT = 10;
    const int TOP = 45;
    
    if( !sDisplay )
    {
        return;
    }

    //int i = mScrollCount * NUM_VISIBLE_LINES;
    unsigned int i = sPage * NUM_VISIBLE_LINES;

    if( sDumpToOutput )
    {
        i = 0;
    }

    float startTime = ((float)radTimeGetMicroseconds()) / 1000.0F;

    BEGIN_PROFILE("* Render Profile *")
    
    //
    // Display header info.
    //
    char fps[256];    
    sprintf(fps,"Game Time: %3.1f(fps) : %3.1f(ms)  Adjusted Time: %3.1f(fps) : %3.1f(ms)", 1/(mFrameRate/1000), mFrameRate, 1/(mFrameRateAdjusted/1000), mFrameRateAdjusted);

//    const int SHADOW_OFFSET = 1;
//    tColour SHADOW_COLOUR(0,0,0);
    tColour stringColour = tColour(sRed, sGreen, sBlue);
/*    
    p3d::pddi->DrawString( fps, 
                           LEFT + sLeftOffset + SHADOW_OFFSET, 
                           TOP + sTopOffset + SHADOW_OFFSET, 
                           SHADOW_COLOUR );
    
    p3d::pddi->DrawString( "-------------------------------------------------------------------",
                           LEFT + sLeftOffset + SHADOW_OFFSET,
                           20 + TOP + sTopOffset + SHADOW_OFFSET,
                           SHADOW_COLOUR);

    p3d::pddi->DrawString( "Ave(%)\t| Single\t| #\t| Total\t| Profile Name\n", 
                           LEFT + sLeftOffset + SHADOW_OFFSET, 
                           40 + TOP + sTopOffset + SHADOW_OFFSET,
                           SHADOW_COLOUR );
    
    p3d::pddi->DrawString( "-------------------------------------------------------------------",
                           LEFT + sLeftOffset + SHADOW_OFFSET, 
                           60 + TOP + sTopOffset + SHADOW_OFFSET,
                           SHADOW_COLOUR);
*/
    
    
    p3d::pddi->DrawString( fps,
                           LEFT + sLeftOffset, 
                           TOP + sTopOffset,
                           stringColour);
    
    p3d::pddi->DrawString( "-------------------------------------------------------------------",
                           LEFT + sLeftOffset, 
                           20 + TOP + sTopOffset,
                           stringColour );

    p3d::pddi->DrawString( "Ave(%)\t| Single\t| #\t| Total\t| Profile Name\n", 
                           LEFT + sLeftOffset, 
                           40 + TOP + sTopOffset,
                           stringColour );

    p3d::pddi->DrawString( "-------------------------------------------------------------------",
                           LEFT + sLeftOffset, 
                           60 + TOP + sTopOffset,
                           stringColour );

    if( sDumpToOutput )
    {
        rTuneString( "-------------------------------------------------------------------------------\n" );    
        rTuneString( "Ave(%)\t| Single\t| #\t| Total\t| Profile Name\n" ); 
        rTuneString( "-------------------------------------------------------------------------------\n" );    
    }

    while( (i < mNextSampleAllocIndex) && mSamples[i].bValid ) 
    {		
        unsigned int indent = 0;
        float aveTime, minTime, maxTime, sampleAve, totalTime;
        char line[256], name[256], indentedName[256];
        char ave[32], min[32], max[32], num[32], sample[32], total[32]; 

//        this->GetProfileHistory( mSamples[i].szName, &aveTime, &minTime, &maxTime, &sampleAve, &totalTime);
        this->GetProfileHistory( mSamples[i].uid, &aveTime, &minTime, &maxTime, &sampleAve, &totalTime);


        // Format the data
        sprintf( ave,   "%3.2f", aveTime );
        sprintf( min,   "%3.2f", minTime );
        sprintf( max,   "%3.2f", maxTime );
        sprintf( sample,"%3.2f", sampleAve );
        sprintf( num,   "%3d",   mSamples[i].iProfileInstances );
        sprintf( total, "%3.2f", totalTime );


        strcpy( indentedName, mSamples[i].szName );
      
        
        for( indent=0; indent<mSamples[i].iNumParents; indent++ ) 
        {
            sprintf( name, "   %s", indentedName );
            strcpy( indentedName, name );
        }
        sprintf(line,"%5s\t| %5s\t|%2s\t| %5s\t| %s ", ave, sample, num, total, indentedName);

//        p3d::pddi->DrawString( line,
//                               LEFT + sLeftOffset + SHADOW_OFFSET, 
//                               80 + TOP + ((i % NUM_VISIBLE_LINES)*20) + sTopOffset + SHADOW_OFFSET, 
//                               SHADOW_COLOUR);
        
        p3d::pddi->DrawString( line,
                               LEFT + sLeftOffset, 
                               80 + TOP + ((i % NUM_VISIBLE_LINES)*20) + sTopOffset, 
                               stringColour);

        if( sDumpToOutput )
        {
            rTunePrintf( "%s\n", line );
        }

        i++;

        if( i >= (unsigned int) (( sPage + 1 ) * NUM_VISIBLE_LINES ))
        {
            break;
        }
    }

    END_PROFILE("* Render Profile *")

    mDisplayTime = (((float)radTimeGetMicroseconds()) / 1000.0F) - startTime;

    sDumpToOutput = false;
}


//==============================================================================
// Profiler::Next
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::NextPage()
{
    ++sPage;

    if( sPage == NUM_VISIBLE_PANES )
    {
        sPage = 0;
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// Profiler::Profiler
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
Profiler::Profiler() :
    mOpenStackTop( -1 ),
    mStartProfile( 0.0F ),
    mEndProfile( 0.0F ),
    mFrameRate( 0.0f ),
    mFrameRateAdjusted( 0.0f ),
    mDisplayTime( 0.0F ),
    mNextHistoryAllocIndex( 0 )
{
    int i;
    for( i = 0; i < NUM_SAMPLES; ++i ) 
    {
      mSamples[i].bValid = false;
      mSamples[i].iNumParents = 0;
    }

MEMTRACK_PUSH_GROUP( "Profiler" );

    HeapMgr()->PushHeap( GMA_DEBUG );

    mOpenSampleStore  = new HashTable<ProfileSample>(NUM_SAMPLES * 2, 100, NUM_SAMPLES * 2);
    mOpenHistoryStore = new HashTable<ProfileSampleHistory>(NUM_SAMPLES * 2, 100, NUM_SAMPLES * 2);

    mOpenSampleStore->AddRef();
    mOpenHistoryStore->AddRef();

    HeapMgr()->PopHeap( GMA_DEBUG );

MEMTRACK_POP_GROUP( "Profiler" );

    radDbgWatchAddBoolean( &sDisplay, "Display", "Profiler", 0, 0 );
    radDbgWatchAddInt( &sPage, "Select Page", "Profiler", 0, 0, 0, NUM_VISIBLE_PANES );    
    radDbgWatchAddInt( &sLeftOffset, "Left Position", "Profiler", 0, 0, -1000, 1000 );
    radDbgWatchAddInt( &sTopOffset, "Top Position", "Profiler", 0, 0, -1000, 1000 );
    radDbgWatchAddInt( &sRed, "Text Colour - Red", "Profiler", 0, 0, 0, 255 );
    radDbgWatchAddInt( &sGreen, "Text Colour - Green", "Profiler", 0, 0, 0, 255 );
    radDbgWatchAddInt( &sBlue, "Text Colour - Blue", "Profiler", 0, 0, 0, 255 );
    radDbgWatchAddBoolean( &sDumpToOutput, "Dump to Output Window", "Profiler", 0, 0 );
}


//==============================================================================
// Profiler::~Profiler
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
Profiler::~Profiler()
{
}


//==============================================================================
// Profiler::StoreProfileHistory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::StoreProfileHistory
( 
//    char* name, 
    tUID nameUID, 
    float percent, 
    float elapsedTime, 
    float sampleTime, 
    float totalTime
)
{
    float oldRatio;
    float newRatio = 0.8f * (elapsedTime / 1000.0f);

    if( newRatio > 1.0f ) 
    {
        newRatio = 1.0f;
    }
    oldRatio = 1.0f - newRatio;

    mFrameRate = (mFrameRate * oldRatio) + (elapsedTime * newRatio);

    // TODO: I had to put this in because there are cases where elapsedTime
    //       is 0 and mDisplayTime isn't... How can that happen?
    //
    float adjustedTime = 0.0F;

    if( elapsedTime >= mDisplayTime )
    {
        adjustedTime = elapsedTime - mDisplayTime;
    }

    mFrameRateAdjusted = (mFrameRateAdjusted*oldRatio) + (adjustedTime * newRatio);

    ProfileSampleHistory *history = mOpenHistoryStore->Find(nameUID);
    if (history == NULL)
    {
        history = AllocHistory();
        rAssertMsg(history != NULL, "Too many profiler histories.");

        history->uid    = nameUID; 
        history->fAve   = percent;
        history->fMin   = percent;    
        history->fMax   = percent;    
        history->fSampleAve = sampleTime;
        history->fSampleTotal = totalTime;

        mOpenHistoryStore->Store(nameUID, history);
    }
    else
    {
        history->fAve         = (history->fAve * oldRatio)         + (percent * newRatio);
        history->fSampleAve   = (history->fSampleAve * oldRatio)   + (sampleTime * newRatio);
        history->fSampleTotal = (history->fSampleTotal * oldRatio) + (totalTime * newRatio);
     
        if( percent < history->fMin ) history->fMin = percent;
        else                          history->fMin = (history->fMin * oldRatio) + (percent * newRatio);

        if( history->fMin < 0.0f ) history->fMin = 0.0f;

        if( percent > history->fMax ) history->fMax = percent;
        else                          history->fMax = (history->fMax * oldRatio) + (percent * newRatio);
    }

}


//==============================================================================
// Profiler::GetProfileHistory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Profiler::GetProfileHistory
( 
//    char* name, 
    tUID nameUID, 
    float* ave, 
    float* min, 
    float* max, 
    float* sample, 
    float* total 
)
{
    *ave = *min = *max = *sample = 0.0f;

    ProfileSampleHistory *history = mOpenHistoryStore->Find(nameUID);
    //rAssertMsg(history != NULL, "Get Profile history Error:  Never been stored!");

    if (history != NULL)
    {
        *ave    = history->fAve;
        *min    = history->fMin;
        *max    = history->fMax;
        *sample = history->fSampleAve;
        *total  = history->fSampleTotal;
    }
}


#endif // PROFILER_ENABLED