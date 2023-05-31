
#ifndef SOUNDNUCLEUS_HPP
#define SOUNDNUCLEUS_HPP

#include <radmemory.hpp>
#include <radsound.hpp>

namespace Sound
{
#ifdef RAD_XBOX
    const unsigned int SOUND_NUM_STREAM_PLAYERS = 10;
#else
    const unsigned int SOUND_NUM_STREAM_PLAYERS = 9;
#endif
    const unsigned int SOUND_NUM_CLIP_PLAYERS = 64;

struct Encoding
{
    IRadSoundHalAudioFormat::Encoding m_Encoding;
    unsigned int m_CompressionNumerator;
    unsigned int m_CompressionDenominator;
};

struct AudioFormat
{
    unsigned int                      m_Channels;
    const Encoding *                  m_pEncoding;
    unsigned int                      m_SamplingRate;
    IRadSoundHalAudioFormat *         m_pAudioFormat;
};

struct StreamerResources
{
    IRadSoundStreamPlayer       * m_pStreamPlayer;
    IRadSoundBufferedDataSource * m_pBufferedDataSource;
    IRadSoundStitchedDataSource * m_pStitchedDataSource;
    AudioFormat                 * m_pAudioFormat;
    bool                          m_IsCaptured;
    unsigned int index;
};

void SoundNucleusInitialize( radMemoryAllocator alloc );
void SoundNucleusTerminate( void );

IRadSoundHalAudioFormat * SoundNucleusGetStreamFileAudioFormat( void );
IRadSoundHalAudioFormat * SoundNucleusGetClipFileAudioFormat( void );
 
StreamerResources * SoundNucleusCaptureStreamerResources( IRadSoundHalAudioFormat * pAf );
void                SoundNucleusUnCaptureStreamerResources( StreamerResources * pSi );

void SoundNucleusLoadClip( const char * pFileName, bool looping );
bool SoundNucleusIsClipLoaded( void );
void SoundNucleusCancelClipLoad( void );
void SoundNucleusFinishClipLoad( IRadSoundClip ** ppClip );
void SoundNucleusServiceClipLoad( void );

}

#endif











