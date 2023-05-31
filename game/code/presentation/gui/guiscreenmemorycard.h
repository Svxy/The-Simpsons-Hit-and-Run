//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMemoryCard
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMEMORYCARD_H
#define GUISCREENMEMORYCARD_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guiscreenmessage.h>
#include <data/memcard/memorycardmanager.h>

#include <radfile.hpp>

//===========================================================================
// Forward References
//===========================================================================

#ifdef RAD_XBOX
    const unsigned int NUM_BYTES_PER_BLOCK = 16 * 1024;
#endif

class CGuiMenu;
struct IRadDrive;

namespace Scrooby
{
    class Group;
}

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLoadSave : public IMemoryCardFormatCallback
{
public:
    CGuiScreenLoadSave( Scrooby::Screen* pScreen );
    virtual ~CGuiScreenLoadSave();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    static bool s_forceGotoMemoryCardScreen;
	// Implements IMemoryCardFormatCallback
	//
	virtual void OnFormatOperationComplete(radFileError err);

protected:
	enum ScreenOperation
    {
        SCREEN_OP_IDLE,
		LOAD,
		SAVE,
		FORMAT,
        DELETE_GAME,

        NUM_SCREEN_OPS
	};

    void UpdateCurrentMemoryDevice();
    virtual void GotoMemoryCardScreen( bool isFromPrompt = false ) = 0;
    virtual void HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response );
	
	void FormatCurrentDrive();

    int m_currentSlot;
    radFileError m_lastError;

    Scrooby::Text* m_currentMemoryDevice;
    int m_currentDriveIndex;

	unsigned int m_minimumFormatTime;
	unsigned int m_elapsedFormatTime;
	radFileError m_formatResult;
	bool         m_formatState;
	bool		 m_formatDone;


	ScreenOperation m_operation;


};

class CGuiScreenMemoryCard : public CGuiScreen
{
public:
    CGuiScreenMemoryCard( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMemoryCard();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    static int s_currentMemoryCardSlot;

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateDeviceList( bool forceUpdate = false );
    void UpdateFreeSpace( unsigned int currentDriveIndex );

    Scrooby::Group* m_layerSelectMemoryDevice;
    Scrooby::Group* m_layerNoMemoryDevice;

#ifdef RAD_XBOX
    Scrooby::Text* m_numFreeBlocks;
    IRadDrive *m_driveMountedFlag[radFileDriveMax];
#endif
    Scrooby::Text* m_memStatusText;

    CGuiMenu* m_pMenu;

    IRadDrive* m_availableDrives[ radFileDriveMax ];
    IRadDrive::MediaInfo* m_mediaInfos[ radFileDriveMax ];
    int m_numAttachedDevices;


};

const int NUM_RADFILE_ERROR_MESSAGES = DataCorrupt; // should be last enum
const int NUM_MEDIA_INFO_STATES = IRadDrive::MediaInfo::MediaDamaged; // should be last enum


const int MAX_MEM_CARD_NAME  = 16;	// max display for memory card 	

enum eErrorMessageMode
{
    ERROR_DURING_CHECKING,
    ERROR_DURING_LOADING,
    ERROR_DURING_SAVING,

    NUM_ERROR_MESSAGE_MODES
};

inline int GetErrorMessageIndex( radFileError errorCode, eErrorMessageMode mode )
{
    const int NUM_PLATFORMS = 3;

    // radfile error messages are offset by media info states
    //
    int messageIndex = NUM_MEDIA_INFO_STATES;

    switch( mode )
    {
        case ERROR_DURING_CHECKING:
        {
            messageIndex += (errorCode - 1);

            break;
        }
        case ERROR_DURING_LOADING:
        {
            messageIndex += (errorCode - 1) + NUM_RADFILE_ERROR_MESSAGES;

            break;
        }
        case ERROR_DURING_SAVING:
        {
            messageIndex += (errorCode - 1) + NUM_RADFILE_ERROR_MESSAGES * 2;

            break;
        }
        default:
        {
            rAssertMsg( false, "ERROR: *** Invalid error message mode!" );
            break;
        }
    }

    // return platform-specific message index
    //
    return( messageIndex * NUM_PLATFORMS + PLATFORM_TEXT_INDEX );
};

inline int GetErrorMessageIndex( IRadDrive::MediaInfo::MediaState mediaState )
{
    const int NUM_PLATFORMS = 3;

    int messageIndex = mediaState - 1;

    // return platform-specific message index
    //
    return( messageIndex * NUM_PLATFORMS + PLATFORM_TEXT_INDEX );
};

#endif // GUISCREENMEMORYCARD_H
