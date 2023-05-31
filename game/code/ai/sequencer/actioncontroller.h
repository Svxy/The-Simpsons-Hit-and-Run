#ifndef _SEQUENCER_ACTIONCONTROLLER_HPP
#define _SEQUENCER_ACTIONCONTROLLER_HPP
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// actioncontroller.hpp
//
// Description: This class contains two sequencers, and handles
//              retrieval and swapping of these sequencers transparently.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 2) Owner: Bryan Brandt
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------

// CLOSER #include "architecture/ctype.hpp"
#include <ai\sequencer\sequencer.h>

//---------------------------------------------------------------------
// Structure and class
//---------------------------------------------------------------------

class ActionController
{
public:

	ActionController();
	virtual ~ActionController();

	Sequencer* GetCurrentSequencer() const
		{ return m_pCurrentSequencer; }
	Sequencer* GetNextSequencer() const
		{ return m_pNextSequencer; }

	// clears ALL sequencers
	void Clear();

	bool IsBusy() const
		{ return (m_pCurrentSequencer->IsBusy() || m_pNextSequencer->IsBusy()); }

	// main entries
	void WakeUp(float time);
	void DoSimulation(float time);
	void Update(float time);

    template <class T> T* SequenceSingleAction(T* t)
    {
        Sequencer* pSeq = GetNextSequencer();
        pSeq->BeginSequence();
        pSeq->AddAction(t);
        pSeq->EndSequence();
        return t;
    }

private:

	Sequencer* m_pCurrentSequencer;
	Sequencer* m_pNextSequencer;
	Sequencer m_Sequencers[2];
};

#endif // HC_SEQUENCER_ACTIONCONTROLLER_HPP
