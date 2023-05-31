#ifndef _SEQUENCER_SEQUENCER_HPP
#define _SEQUENCER_SEQUENCER_HPP

//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// sequencer.hpp
//
// Description: This class queues the actions of an actor.
//              The sequencer places actions into multiple queues,
//              named tracks, which can be grouped into larger composite
//              actions named sequences, which will be executed in turn.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 2) Owner: Bryan Brandt
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
// Structure and class
//---------------------------------------------------------------------

class Action;

class Sequencer
{
public:

	enum { MAX_SEQUENCES = 16 };
	enum { MAX_ACTIONS = 6 };

	Sequencer();
	virtual ~Sequencer();

	bool IsBusy() const;
	void Clear();

	void BeginSequence();
	void EndSequence();

	// Dependent actions; calling these methods to add actions
	// will queue each subsequent action up to occur after the
	// previous action completes.
	void AddAction(Action* action)
		{ AddAction(0.0f, -1.0f, action); }

	// Set t_duration <= 0.0f to make action indefinite.
	void AddAction(float t_duration, Action* action)
		{ AddAction(0.0f, t_duration, action); }

	// Explicitly timed actions; these actions have an explicit
	// start time independent of other actions' completion.
	// Set t_duration <= 0.0f to make action indefinite.
	void AddAction(float t_begin, float t_duration, Action* action);

	// As above, but these can be called at any time, assuming
	// there is a sequence currently running.
	// This allows adding actions to the currently running sequencer
	// on the fly.  As such, it makes the most sense for these actions
	// to be slaves.
	void AddActionToSequence(Action* action)
		{ AddActionToSequence(0.0f, -1.0f, action); }
	void AddActionToSequence(float t_duration, Action* action)
		{ AddActionToSequence(0.0f, t_duration, action); }
	void AddActionToSequence(float t_begin, float t_duration, Action* action);

	// main entries
	void WakeUp(float time);
	void DoSimulation(float time);
	void Update(float time);

	// get time elapsed
	float GetTimeElapsed() const
		{ return m_TimeElapsed; }

private:

	// sAction - contains timing information
	//           and action itself
	struct sAction
	{
		float timeBegin;
		float timeDuration;
		Action* action;
	};

	// sSequence - stores multiple parallel actions
	struct sSequence
	{
		unsigned actionCount;
		sAction actions[MAX_ACTIONS];
	};

	// queue of sequences, executed in order
	unsigned m_SequenceCount;
	sSequence m_Sequences[MAX_SEQUENCES];

	// edit state - reference when adding sequences/tracks/actions
	enum State
	{
		STATE_NONE,
		STATE_SEQUENCE
	};

	State m_State;
	sSequence* m_pCurrentSequence;

	// amount of time passed for current sequence
	float m_TimeElapsed;

	void ClearSequence(sSequence* seq);
};


#endif // HC_SEQUENCER_SEQUENCER_HPP
