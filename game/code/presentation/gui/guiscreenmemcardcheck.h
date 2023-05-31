//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMemCardCheck
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/23      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMEMCARDCHECK_H
#define GUISCREENMEMCARDCHECK_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <data/memcard/memorycardmanager.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMemCardCheck : public CGuiScreen,
                               public IMemoryCardCheckCallback,
							   public IMemoryCardFormatCallback
{
public:
    CGuiScreenMemCardCheck( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMemCardCheck();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual void OnMemoryCardCheckDone( radFileError errorCode,
                                        IRadDrive::MediaInfo::MediaState mediaState,
                                        int driveIndex,
                                        int mostRecentSaveGameDriveIndex,
                                        int mostRecentSaveGameSlot );

	// Implements IMemoryCardFormatCallback
	//
	virtual void OnFormatOperationComplete(radFileError err);

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void OnContinue();

    Scrooby::Text* m_messageText;

	unsigned int m_minimumFormatTime;
	unsigned int m_elapsedFormatTime;
	radFileError m_formatResult;
    bool         m_formatState : 1;
    bool		 m_formatDone : 1;
    bool         m_StatusPromptShown : 1;
    bool         m_isAutoLoadPending : 1;

};

#endif // GUISCREENMEMCARDCHECK_H
