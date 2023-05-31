#include <sound/soundrenderer/soundnucleus.hpp>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <radmusic/radmusic.hpp>
#include <memory/srrmemory.h>

namespace Sound
{

const Encoding gPcmEncoding     = { IRadSoundHalAudioFormat::PCM, 1, 1 };
const Encoding gPcmBEncoding    = { IRadSoundHalAudioFormat::PCM_BIGENDIAN, 1, 1 };
const Encoding gVagEncoding     = { IRadSoundHalAudioFormat::VAG, 2, 7 };
const Encoding gGcAdpcmEncoding = { IRadSoundHalAudioFormat::GCNADPCM, 2, 7 };
const Encoding gRadAdpcmEncoding = { IRadSoundHalAudioFormat::RadicalAdpcm, 5, 16 };
const Encoding gXAdpcmEncoding = { IRadSoundHalAudioFormat::XBOXADPCM, 36, 128 };

const unsigned int NUM_AUX_SENDS = 1;

const unsigned int MUSIC_NUM_STREAM_PLAYERS = 4;
const unsigned int MUSIC_NUM_CLIP_PLAYERS = 2;
const unsigned int MUSIC_NUM_CHANNELS = 2;
const unsigned int MUSIC_SAMPLING_RATE = 24000;

const unsigned int TOTAL_PS2_FREE_UNCOMPRESSED_CLIP_BYTES = ( 1624 * 1024 * 7 ) / 2;

#if defined RAD_GAMECUBE

    const int ARAM_SILENT_BUFFER_SIZE = 1280;
    const int ARAM_USER_START = 0x4000;
    const int ARAM_RESERVED_BINK_MEMORY = 0x25800;
    const int GAMECUBE_SOUND_MEMORY_AVAILABLE =
        ( 1024 * 1024 * 10 ) - ARAM_SILENT_BUFFER_SIZE - ARAM_USER_START - ARAM_RESERVED_BINK_MEMORY;

    const int PLAYBACK_RATE = 32000;
    
    AudioFormat gCompressedStreamAudioFormat =   { 1, & gGcAdpcmEncoding, 24000 };
    AudioFormat gUnCompressedStreamAudioFormat = { 1, & gPcmBEncoding, 24000 };
    AudioFormat gClipFileAudioFormat =     { 1, & gPcmBEncoding, 24000 };
    AudioFormat gMusicAudioFormat =        { 2, & gPcmBEncoding, 24000 };
        
    const unsigned int STREAM_BUFFER_SIZE_MS = 6000;      
    const bool STREAM_USE_BUFFERED_DATA_SOURCES = false;
    const unsigned int STREAM_BUFFERED_DATA_SOURCE_SIZE_MS = 0;   
    const radMemorySpace STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Local;
    
    const unsigned int CLIP_BUFFERED_DATA_SOURCE_SIZE_MS = 0;
    const radMemorySpace CLIP_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Local; 
    
#elif defined RAD_XBOX || defined RAD_WIN32
    
    const int PLAYBACK_RATE = 0;

#ifdef PAL
    AudioFormat gCompressedStreamAudioFormat =   { 1, & gXAdpcmEncoding, 24000 };
#else
    AudioFormat gCompressedStreamAudioFormat =   { 1, & gPcmEncoding, 24000 };
#endif
    AudioFormat gUnCompressedStreamAudioFormat = { 1, & gPcmEncoding, 24000 };
    AudioFormat gClipFileAudioFormat =     { 1, & gPcmEncoding, 24000 };
    AudioFormat gMusicAudioFormat =        { 2, & gPcmEncoding, 24000 };    
    
    const unsigned int STREAM_BUFFER_SIZE_MS = 6000;    
    const bool STREAM_USE_BUFFERED_DATA_SOURCES = false;
    const unsigned int STREAM_BUFFERED_DATA_SOURCE_SIZE_MS = 0;
    const radMemorySpace STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Local;  
    
    const unsigned int CLIP_BUFFERED_DATA_SOURCE_SIZE_MS = 0;
    const radMemorySpace CLIP_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Local; 

             
#elif defined RAD_PS2

    const int PLAYBACK_RATE = 0;
    
    AudioFormat gCompressedStreamAudioFormat =   { 1, & gVagEncoding, 24000 };
    AudioFormat gUnCompressedStreamAudioFormat = { 1, & gVagEncoding, 24000 };
    AudioFormat gClipFileAudioFormat =     { 1, & gVagEncoding, 24000 };
    AudioFormat gMusicAudioFormat =        { 2, & gVagEncoding, 24000 };    
        
    const unsigned int STREAM_BUFFER_SIZE_MS = 1000;
    const bool STREAM_USE_BUFFERED_DATA_SOURCES = true;
    const unsigned int STREAM_BUFFERED_DATA_SOURCE_SIZE_MS = 4100;
    const radMemorySpace STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Iop; 

    const unsigned int CLIP_BUFFERED_DATA_SOURCE_SIZE_MS = 5000;
    const radMemorySpace CLIP_BUFFERED_DATA_SOURCE_MEMORY_SPACE = radMemorySpace_Ee;  
              
#endif   

enum ClipLoadState
{
    ClipLoadState_Idle,
    ClipLoadState_InitFile,
    ClipLoadState_FillingBuffer,
    ClipLoadState_LoadingClip,
    ClipLoadState_Done
};
    
    
struct ClipLoadInfo
{
    IRadSoundRsdFileDataSource * pFds;
    IRadSoundClip * pClip;
    IRadSoundBufferedDataSource * pBds;
    ClipLoadState state;
    bool looping;
} gClipLoadInfo;

StreamerResources gStreamers[ SOUND_NUM_STREAM_PLAYERS ] =
{
    { NULL, NULL, NULL, & gCompressedStreamAudioFormat,   false, 0 },
    { NULL, NULL, NULL, & gCompressedStreamAudioFormat,   false, 0  },
    { NULL, NULL, NULL, & gCompressedStreamAudioFormat,   false, 0  },
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
#ifdef RAD_XBOX
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  },
#endif
    { NULL, NULL, NULL, & gUnCompressedStreamAudioFormat, false, 0  }
};                                  


void CreateAudioFormat( AudioFormat * pAf, radMemoryAllocator alloc )
{
    pAf->m_pAudioFormat = ::radSoundHalAudioFormatCreate( alloc );
    pAf->m_pAudioFormat->AddRef( );
        
    pAf->m_pAudioFormat->Initialize(
        pAf->m_pEncoding->m_Encoding,
        NULL,
        pAf->m_SamplingRate,
        pAf->m_Channels,
        16 );
}

void DestroyAudioFormat( AudioFormat * pAf )
{
    pAf->m_pAudioFormat->Release( );
    pAf->m_pAudioFormat = NULL;
}

void CreateStreamerResources( StreamerResources * pSi, radMemoryAllocator alloc )
{
    pSi->m_pStreamPlayer = ::radSoundStreamPlayerCreate( alloc );
    pSi->m_pStreamPlayer->AddRef( );

    pSi->m_pStreamPlayer->Initialize(
        pSi->m_pAudioFormat->m_pAudioFormat,
        STREAM_BUFFER_SIZE_MS,
        IRadSoundHalAudioFormat::Milliseconds,
        ::radSoundHalSystemGet( )->GetRootMemoryRegion( ),
        "Sound Stream Player" );
    
    pSi->m_pStitchedDataSource = ::radSoundStitchedDataSourceCreate( alloc );
    pSi->m_pStitchedDataSource->AddRef( );
    pSi->m_pStitchedDataSource->InitializeFromAudioFormat( pSi->m_pAudioFormat->m_pAudioFormat );

    if ( STREAM_USE_BUFFERED_DATA_SOURCES && ( false == CommandLineOptions::Get( CLO_FIREWIRE ) ) )
    {                                
        pSi->m_pBufferedDataSource = radSoundBufferedDataSourceCreate( alloc );
        pSi->m_pBufferedDataSource->AddRef( );

        pSi->m_pBufferedDataSource->Initialize(
            STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE,
            radMemorySpaceGetAllocator( STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE, RADMEMORY_ALLOC_DEFAULT ), // This is IOP Memory
            STREAM_BUFFERED_DATA_SOURCE_SIZE_MS,
            IRadSoundHalAudioFormat::Milliseconds,
            pSi->m_pAudioFormat->m_pAudioFormat,
            "DaSound Buffered DataSource" );
            
    }
    else
    {
        pSi->m_pBufferedDataSource = NULL;
    }
}

void DestroyStreamerResources( StreamerResources* pSi )
{
    // Its a stream player
    pSi->m_pStreamPlayer->Stop( );
    pSi->m_pStreamPlayer->SetDataSource( NULL );

    if( pSi->m_pBufferedDataSource )
    {
        pSi->m_pBufferedDataSource->Release( );
        pSi->m_pBufferedDataSource = NULL;
    }
    
    // Release the player
    pSi->m_pStreamPlayer->Release( );
    pSi->m_pStreamPlayer = NULL;
    

    pSi->m_pStitchedDataSource->Release( );
    pSi->m_pStitchedDataSource = NULL;
}

unsigned int CalculateStreamerSize( unsigned int ms, AudioFormat * pAf )
{
	unsigned int sizeInFrames = pAf->m_pAudioFormat->MillisecondsToFrames( ms );

    unsigned int optimalFrameMultiple =
        pAf->m_pAudioFormat->BytesToFrames( radSoundHalDataSourceReadMultipleGet( ) );

    // Our buffer must be at least as big as two optimal reads.

    sizeInFrames = radMemoryRoundUp( sizeInFrames, optimalFrameMultiple * 2 );

    sizeInFrames = ::radSoundHalBufferCalculateMemorySize( IRadSoundHalAudioFormat::Frames,
        sizeInFrames, IRadSoundHalAudioFormat::Frames, pAf->m_pAudioFormat );

	unsigned int sizeInBytes = pAf->m_pAudioFormat->FramesToBytes( sizeInFrames );
	
	return sizeInBytes;
}

void SoundNucleusInitialize( radMemoryAllocator alloc )
{
    
#if defined( RAD_PS2 ) || defined( RAD_GAMECUBE ) || defined( RAD_WIN32 )
    ::radSoundHalSystemInitialize( alloc );
#else
    ::radSoundHalSystemInitialize( GMA_XBOX_SOUND_MEMORY );
#endif

    CreateAudioFormat( & gCompressedStreamAudioFormat, alloc );
    CreateAudioFormat( & gUnCompressedStreamAudioFormat, alloc );
    CreateAudioFormat( & gClipFileAudioFormat, alloc );
    CreateAudioFormat( & gMusicAudioFormat, alloc );
    

    
    //
    // ESAN TODO: Investigate the magic number 150 below...
    //
    
    unsigned int totalStreamSoundMemoryNeeded = 0;
    unsigned int totalStreamBufferMemoryNeeded = 0;
    unsigned int totalClipBufferMemoryNeeded = 0;

    if ( CLIP_BUFFERED_DATA_SOURCE_SIZE_MS > 0 )
    {
        gClipLoadInfo.pBds = radSoundBufferedDataSourceCreate( GMA_AUDIO_PERSISTENT );
        gClipLoadInfo.pBds->AddRef( );
        gClipLoadInfo.pBds->Initialize(
            CLIP_BUFFERED_DATA_SOURCE_MEMORY_SPACE,
            radMemorySpaceGetAllocator(
                CLIP_BUFFERED_DATA_SOURCE_MEMORY_SPACE,
                GMA_AUDIO_PERSISTENT ),
            CLIP_BUFFERED_DATA_SOURCE_SIZE_MS,
            IRadSoundHalAudioFormat::Milliseconds,
            SoundNucleusGetClipFileAudioFormat( ),
            "Clip Bds" );
            
        gClipLoadInfo.pBds->SetLowWaterMark( 1.0f );            
            
        totalClipBufferMemoryNeeded =
            SoundNucleusGetClipFileAudioFormat( )->MillisecondsToBytes(
                CLIP_BUFFERED_DATA_SOURCE_SIZE_MS );               
    }
    
    
    for( unsigned int i = 0; i < SOUND_NUM_STREAM_PLAYERS; i ++ )
    {
        StreamerResources* pSi = gStreamers + i;
        
        unsigned int streamSoundMemoryNeeded = CalculateStreamerSize(
            STREAM_BUFFER_SIZE_MS,
            pSi->m_pAudioFormat );

        totalStreamSoundMemoryNeeded += streamSoundMemoryNeeded;
            
        unsigned int streamBufferMemoryNeeded = CalculateStreamerSize(
            STREAM_BUFFERED_DATA_SOURCE_SIZE_MS,
            pSi->m_pAudioFormat );
                                             
        totalStreamBufferMemoryNeeded += streamBufferMemoryNeeded;
                
        rTunePrintf( "AUDIO: Predicting SOUND streamer will allocate: Sound: [0x%x] Buffer:[0x%x]\n",
            streamSoundMemoryNeeded,
            streamBufferMemoryNeeded );
    }
    
    for( unsigned int i = 0; i < MUSIC_NUM_STREAM_PLAYERS; i ++ )
    {
        unsigned int streamSoundMemoryNeeded = CalculateStreamerSize(
            STREAM_BUFFER_SIZE_MS,
            & gMusicAudioFormat );

        totalStreamSoundMemoryNeeded += streamSoundMemoryNeeded;
                    
        unsigned int streamBufferMemoryNeeded = CalculateStreamerSize(
            STREAM_BUFFERED_DATA_SOURCE_SIZE_MS,
            & gMusicAudioFormat );

        totalStreamBufferMemoryNeeded += streamBufferMemoryNeeded;
                        
        rTunePrintf( "AUDIO: Predicting MUSIC streamer will allocate: Sound: [0x%x] Buffer:[0x%x]\n",
            streamSoundMemoryNeeded,
            streamBufferMemoryNeeded );                              
            
    }

    unsigned int totalClipMemoryNeeded =
        ( TOTAL_PS2_FREE_UNCOMPRESSED_CLIP_BYTES * gClipFileAudioFormat.m_pEncoding->m_CompressionNumerator ) /
            gClipFileAudioFormat.m_pEncoding->m_CompressionDenominator;
    
    rTunePrintf(
        "AUDIO: Sound Memory totals: Stream: [0x%x] Clip: [0x%x], Total: [0x%x]\n",
        totalStreamSoundMemoryNeeded,
        totalClipMemoryNeeded,
        totalStreamSoundMemoryNeeded + totalClipMemoryNeeded );

    #ifdef RAD_GAMECUBE
        rAssert(
            ( totalStreamSoundMemoryNeeded + totalClipMemoryNeeded ) <=
                GAMECUBE_SOUND_MEMORY_AVAILABLE );
    #endif

    rTunePrintf(
        "AUDIO: Sound Buffered Stream Memory Stream: [0x%x], Clip: [0x%x] Total:\n",
        totalStreamBufferMemoryNeeded,
        totalClipBufferMemoryNeeded,
        totalStreamBufferMemoryNeeded + totalClipBufferMemoryNeeded );

    IRadSoundHalSystem::SystemDescription desc;
    
    desc.m_MaxRootAllocations  = 170;
    desc.m_NumAuxSends         = NUM_AUX_SENDS;
#ifdef RAD_WIN32    
    desc.m_SamplingRate        = 24000;
#endif  
    
#ifndef RAD_PS2
    desc.m_ReservedSoundMemory = totalStreamSoundMemoryNeeded + totalClipMemoryNeeded;
#endif

#ifdef RAD_GAMECUBE
    desc.m_EffectsAllocator = GMA_AUDIO_PERSISTENT;
#endif

    ::radSoundHalSystemGet( )->Initialize( desc );    
    //::radSoundHalSystemGet( )->SetOutputMode( radSoundOutputMode_Surround );  
                     
    for( unsigned int i = 0; i < SOUND_NUM_STREAM_PLAYERS; i ++ )
    {
        gStreamers[ i ].index = i;
        CreateStreamerResources( gStreamers + i, alloc ); 
    }

    radmusic::stream_graph_description sgDesc[ MUSIC_NUM_STREAM_PLAYERS ];
            
    for( unsigned int i = 0; i < MUSIC_NUM_STREAM_PLAYERS; i ++ )
    {
        sgDesc[ i ].buffered_data_source_size_in_ms = STREAM_BUFFERED_DATA_SOURCE_SIZE_MS;
        sgDesc[ i ].buffered_data_source_space      = STREAM_BUFFERED_DATA_SOURCE_MEMORY_SPACE;     
        sgDesc[ i ].channels                        = MUSIC_NUM_CHANNELS;
        sgDesc[ i ].sampling_rate                   = MUSIC_SAMPLING_RATE;
        sgDesc[ i ].stream_buffer_size_in_ms        = STREAM_BUFFER_SIZE_MS;
        sgDesc[ i ].use_buffered_data_source        = CommandLineOptions::Get( CLO_FIREWIRE ) ? false : STREAM_USE_BUFFERED_DATA_SOURCES;
    }
        
    radmusic::initialize( sgDesc, MUSIC_NUM_STREAM_PLAYERS, MUSIC_NUM_CLIP_PLAYERS, GMA_MUSIC );
    radmusic::register_radload_loaders( );
    
}

void SoundNucleusTerminate( void )
{
    rAssert( ClipLoadState_Idle == gClipLoadInfo.state );
    
    if ( gClipLoadInfo.pBds != NULL )
    {
        gClipLoadInfo.pBds->Release( );
    }
    
    for( unsigned int i = 0; i < SOUND_NUM_STREAM_PLAYERS; i ++ )
    {
        DestroyStreamerResources( gStreamers + i );
    }
    
    DestroyAudioFormat( & gCompressedStreamAudioFormat );
    DestroyAudioFormat( & gUnCompressedStreamAudioFormat );
    DestroyAudioFormat( & gClipFileAudioFormat );
    DestroyAudioFormat( & gMusicAudioFormat );

    radmusic::terminate( );

    // Shutdown our related systems
    ::radSoundHalSystemTerminate( );        
    
}

IRadSoundHalAudioFormat * SoundNucleusGetStreamFileAudioFormat( void )
{
#if defined( RAD_GAMECUBE ) || ( defined( RAD_XBOX ) && defined( PAL ) )
        return NULL;
    #else
        return gUnCompressedStreamAudioFormat.m_pAudioFormat;
    #endif

}

IRadSoundHalAudioFormat * SoundNucleusGetClipFileAudioFormat( void )
{
    return gClipFileAudioFormat.m_pAudioFormat;
}

StreamerResources* SoundNucleusCaptureStreamerResources( IRadSoundHalAudioFormat * pAf )
{
    for( unsigned int i = 0; i < SOUND_NUM_STREAM_PLAYERS; i ++ )
    {
        StreamerResources * pSi =
            gStreamers + i;
         
        if ( ! pSi->m_IsCaptured )
        {
            if ( pSi->m_pAudioFormat->m_pAudioFormat->Matches( pAf ) )
            {
                pSi->m_IsCaptured = true;
                return pSi;
            }
        }
    }
    
    rTuneAssertMsg( false, "Out of streamers of desired format" );
    
    return NULL;
}

void SoundNucleusUnCaptureStreamerResources( StreamerResources * pSi )
{
    rAssert( gStreamers[ pSi->index ].m_IsCaptured == true );
    gStreamers[ pSi->index ].m_IsCaptured = false;
}

void SoundNucleusLoadClip( const char * pFileName, bool looping )
{
    rAssert( ClipLoadState_Idle == gClipLoadInfo.state );
        
    gClipLoadInfo.pFds = radSoundRsdFileDataSourceCreate( GMA_AUDIO_PERSISTENT );
    gClipLoadInfo.pFds->AddRef( );     
    gClipLoadInfo.looping = looping;
             

    // Initialize the file data source iwth our file
    //
    gClipLoadInfo.pFds->InitializeFromFileName(
        pFileName,
        false,
        0,
        IRadSoundHalAudioFormat::Frames,
        SoundNucleusGetClipFileAudioFormat( ) );
        
    gClipLoadInfo.state = ClipLoadState_InitFile;
}

bool SoundNucleusIsClipLoaded( void )
{
    rAssert( ClipLoadState_Idle != gClipLoadInfo.state );
 
    return ClipLoadState_Done == gClipLoadInfo.state;      
}

void SoundNucleusFinishClipLoad( IRadSoundClip ** ppClip )
{
    rAssert( ClipLoadState_Done == gClipLoadInfo.state );
    
    *ppClip = gClipLoadInfo.pClip;
    gClipLoadInfo.pClip = NULL;
    
    gClipLoadInfo.pFds->Release( );
    gClipLoadInfo.pFds = NULL;
    
    if ( NULL != gClipLoadInfo.pBds )
    {
        gClipLoadInfo.pBds->SetInputDataSource( 0 );
    }
    
    gClipLoadInfo.state = ClipLoadState_Idle;
}

void SoundNucleusCancelClipLoad( void )
{
    rAssert( ClipLoadState_Idle != gClipLoadInfo.state );
    
    if( NULL != gClipLoadInfo.pClip )
    {
        gClipLoadInfo.pClip->Release( );
        gClipLoadInfo.pClip = NULL;
    }

    if( NULL != gClipLoadInfo.pFds )
    {
        gClipLoadInfo.pFds->Release( );
        gClipLoadInfo.pFds = NULL;
    }
    
    if ( NULL != gClipLoadInfo.pBds )
    {
        gClipLoadInfo.pBds->SetInputDataSource( 0 );
    }
    
    gClipLoadInfo.state = ClipLoadState_Idle;
      
}


void SoundNucleusServiceClipLoad( void )
{
    ClipLoadState oldState;
    
    do
    {
        oldState = gClipLoadInfo.state;
        
        switch ( gClipLoadInfo.state )
        {
            case ClipLoadState_Idle:
            {
                break;
            }
            case ClipLoadState_InitFile:
            {
                if ( IRadSoundHalDataSource::Initialized == gClipLoadInfo.pFds->GetState( ) )
                {
                    if ( gClipLoadInfo.pBds != NULL )
                    {
                        unsigned int ms =
                            gClipLoadInfo.pBds->GetFormat( )->FramesToMilliseconds(
                                gClipLoadInfo.pFds->GetRemainingFrames( ) );
                        
                        rTuneAssert( ms < CLIP_BUFFERED_DATA_SOURCE_SIZE_MS );                        
                        
                        gClipLoadInfo.pBds->SetInputDataSource( gClipLoadInfo.pFds );
                        
                        gClipLoadInfo.state = ClipLoadState_FillingBuffer;
                    }
                    else
                    {
                        gClipLoadInfo.pClip = radSoundClipCreate( GMA_AUDIO_PERSISTENT );
                        gClipLoadInfo.pClip->AddRef( );
                        gClipLoadInfo.pClip->Initialize(
                            gClipLoadInfo.pFds,
                            radSoundHalSystemGet( )->GetRootMemoryRegion( ),
                            gClipLoadInfo.looping,       
                            "Clip" );
                            
                        gClipLoadInfo.state = ClipLoadState_LoadingClip;
                    }
                }
                
                break;
            }
            case ClipLoadState_FillingBuffer:
            {
                if ( gClipLoadInfo.pBds->IsBufferFull( ) )
                {
                    gClipLoadInfo.pClip = radSoundClipCreate( GMA_AUDIO_PERSISTENT );
                    gClipLoadInfo.pClip->AddRef( );
                    gClipLoadInfo.pClip->Initialize(
                        gClipLoadInfo.pBds,
                        radSoundHalSystemGet( )->GetRootMemoryRegion( ),
                        gClipLoadInfo.looping,       
                        "Clip" );
                        
                    gClipLoadInfo.state = ClipLoadState_LoadingClip;
                }
                
                break;
            }
            case ClipLoadState_LoadingClip:
            {
                if ( IRadSoundClip::Initialized == gClipLoadInfo.pClip->GetState( ) )
                {
                    gClipLoadInfo.state = ClipLoadState_Done;
                }
                
                break;
            }
            case ClipLoadState_Done:
            {
                break;
            }
        }
     }
     while( oldState != gClipLoadInfo.state );
}

}