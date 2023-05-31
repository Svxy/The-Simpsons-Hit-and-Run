//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// sequencer.cpp
//
// Description: This class queues the actions of an actor.
//              The sequencer places actions into multiple queues,
//              named tracks, which can be grouped into larger composite
//              actions named sequences, which will be executed in turn.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 3) Owner: Bryan Brandt
//-----------------------------------------------------------------------------

#include <raddebug.hpp>
#include <ai\sequencer\sequencer.h>
#include <ai\sequencer\action.h>

static bool s_StopClear = false;

//---------------------------------------------------------------------
// Sequencer class
//---------------------------------------------------------------------

Sequencer::Sequencer():
	m_SequenceCount(0),
	m_State(STATE_NONE),
	m_TimeElapsed(0.0f)
{
}

Sequencer::~Sequencer()
{
	Clear();
}

void Sequencer::ClearSequence(sSequence* seq)
{
	sAction* curAction = seq->actions;

	// iterate over all actions of current sequence
	for (unsigned i = 0; i < seq->actionCount; ++i)
	{
        if ( curAction->action )
        {
            if ( curAction->action->IsRunning() )
            {
                curAction->action->Clear();
            }
            // [Dusit: June 14th, 2003]
            // always gets called to abort safely whether
            // the action's running or not.
            curAction->action->Abort(); 
		}
        tRefCounted::Release( curAction->action );
		curAction->action = 0;

		++curAction;
	}

	seq->actionCount = 0;
}

void Sequencer::Clear()
{
    rAssert(!s_StopClear);
	// empty all sequences
	for (unsigned i = 0; i < m_SequenceCount; ++i)
	{
		ClearSequence(m_Sequences + i);
	}

	m_SequenceCount = 0;
	m_State = STATE_NONE;
	m_TimeElapsed = 0.0f;
}

void Sequencer::BeginSequence()
{
	rAssert(m_State == STATE_NONE);
	rAssert(m_SequenceCount < MAX_SEQUENCES);

	// add new sequence, set current sequence index to it,
	// and reset current track to zero
	m_pCurrentSequence = (m_Sequences + m_SequenceCount);
	++m_SequenceCount;

	// reset current sequence
	m_pCurrentSequence->actionCount = 0;

	// indicate we can now add tracks/timed actions
	m_State = STATE_SEQUENCE;
}

void Sequencer::EndSequence()
{
	rAssert(m_State == STATE_SEQUENCE);
	m_State = STATE_NONE;
}

void Sequencer::AddAction(float t_begin, float t_duration, Action* action)
{
	// we need a sequence to add an action
	rAssert(m_State != STATE_NONE);
	rAssert(action != 0);

	// allocate a new track for the action
	sAction* curAction = (m_pCurrentSequence->actions + m_pCurrentSequence->actionCount);
	++(m_pCurrentSequence->actionCount);

	// action runs from t_begin for t_duration seconds
	curAction->timeBegin = t_begin;
	curAction->timeDuration = t_duration;
	curAction->action = action;
    curAction->action->AddRef( );
}

void Sequencer::AddActionToSequence(float t_begin, float t_duration, Action* action)
{
	// we assume a sequence already exists
	rAssert(m_State == STATE_NONE);
	rAssert(m_SequenceCount > 0);
	rAssert(action != 0);

	// allocate a new track for the action
	sAction* curAction = (m_pCurrentSequence->actions + m_pCurrentSequence->actionCount);
	++(m_pCurrentSequence->actionCount);

	// we need to offset our times from NOW
	curAction->timeBegin = (m_TimeElapsed + t_begin);
	curAction->timeDuration = t_duration;
	curAction->action = action;
    curAction->action->AddRef( );
}

bool Sequencer::IsBusy() const
{
	if (m_SequenceCount <= 0)
		return false;

	unsigned actionIdx = 0;
	const sAction* curAction = m_Sequences->actions;

	// iterate over all tracks of current sequence
	while (actionIdx < m_Sequences->actionCount)
	{
		// is track active?
		if (curAction->action != 0)
			return true;

		++actionIdx;
		++curAction;
	}

	// no tracks had actions in queue
	return false;
}

void Sequencer::WakeUp(float time)
{
	if (m_SequenceCount <= 0)
		return;

// CLOSER??? m_TimeElapsed += SIMULATION_TIME;
    m_TimeElapsed += time;

	unsigned actionIdx = 0;
	sAction* curAction = m_Sequences->actions;

	// iterate over all tracks of current sequence
	while (actionIdx < m_Sequences->actionCount)
	{
		// is track active?
		if (curAction->action != 0)
		{
			// check to see if action should be running yet
			if (curAction->timeBegin <= m_TimeElapsed)
			{
				Action* action = curAction->action;

				// Is the current action sleeping?
				if (action->IsSleeping())
				{
					// Wake-up the action.
					action->WakeUp(time);
					// Start the action only if wakeup didn't kill it
					if (!action->IsDone())
					{
						action->Run();
					}
				}
			}
		}

		++actionIdx;
		++curAction;
	}
}

void Sequencer::DoSimulation(float time)
{
	if (m_SequenceCount <= 0)
		return;

	unsigned actionIdx = 0;
	sAction* curAction = m_Sequences->actions;

	// iterate over all tracks of current sequence
	while (actionIdx < m_Sequences->actionCount)
	{
		// is track active?
		if (curAction->action != 0)
		{
			// check to see if action should be running yet
			if (curAction->timeBegin <= m_TimeElapsed)
			{
				Action* action = curAction->action;

				// Is the current action running? Yes, then do a simulation.
				if (action->IsRunning())
				{
					action->DoSimulation(time);
				}

				// if action is non-indefinite, age it
				if (curAction->timeDuration >= 0.0f)
				{
					// age the action
					curAction->timeDuration -= time;

					// if the action time has elapsed, finish it
					if (curAction->timeDuration <= 0.0f)
					{
						action->Done();
					}
				}
			}
		}

		++actionIdx;
		++curAction;
	}
}

void Sequencer::Update(float time)
{
	if (m_SequenceCount <= 0)
		return;

	unsigned masterCount = 0;

	unsigned actionIdx = 0;
	sAction* curAction = m_Sequences->actions;

	// iterate over all tracks
	while (actionIdx < m_Sequences->actionCount)
	{
		Action* action = curAction->action;

		// does track have an action?
		if (action != 0)
		{
			// Is the current action running? Yes, then get the new status.
			if (action->IsRunning())
			{
                s_StopClear = true;
				action->Update(time);
                s_StopClear = false;
			}

			// Is the current action done or failed?
			// Yes, then clear action
			if (action->IsDone())
			{
				action->Clear();
				action->Release( );
				curAction->action = 0;
			}
			else
			{
				// accumulate master action count
				if (!action->IsSlave())
				{
					++masterCount;
				}
			}
		}

		++actionIdx;
		++curAction;
	}

	if (masterCount <= 0)
	{
		// we have no master actions, so move on to
		// the next sequence
		ClearSequence(m_Sequences);

		for (unsigned i = 1; i < m_SequenceCount; ++i)
		{
			m_Sequences[i-1] = m_Sequences[i];
		}

        if(m_SequenceCount)
        {
    		--(m_SequenceCount);
        }

		// reset time origin for next sequence
		m_TimeElapsed = 0.0f;
	}
}

// End of file.
