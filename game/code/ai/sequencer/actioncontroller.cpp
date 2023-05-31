//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// actioncontroller.cpp
//
// Description: This class contains two sequencers, and handles
//              retrieval and swapping of these sequencers transparently.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 1) Owner: Bryan Brandt
//-----------------------------------------------------------------------------

#include <ai\sequencer\actioncontroller.h>

//---------------------------------------------------------------------
// ActionController class
//---------------------------------------------------------------------

ActionController::ActionController():
	m_pCurrentSequencer(m_Sequencers + 1),
	m_pNextSequencer(m_Sequencers)
{
}

ActionController::~ActionController()
{
}

void ActionController::Clear()
{
	m_Sequencers[0].Clear();
	m_Sequencers[1].Clear();
}

void ActionController::WakeUp(float time)
{
    if (!m_pCurrentSequencer->IsBusy() && m_pNextSequencer->IsBusy())
	{
		// swap sequencers
		m_pNextSequencer = m_pCurrentSequencer;

		if (m_pNextSequencer == m_Sequencers)
		{
			m_pCurrentSequencer = (m_Sequencers + 1);
		}
		else
		{
			m_pCurrentSequencer = m_Sequencers;
		}

		// clear old sequencer so it's ready for new input
		m_pNextSequencer->Clear();
	}

	m_pCurrentSequencer->WakeUp(time);
}

void ActionController::DoSimulation(float time)
{
	m_pCurrentSequencer->DoSimulation(time);
}

void ActionController::Update(float time)
{
	m_pCurrentSequencer->Update(time);
}

// End of file.
