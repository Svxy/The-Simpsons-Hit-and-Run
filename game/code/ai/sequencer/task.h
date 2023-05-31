#ifndef _SEQUENCER_TASK_HPP
#define _SEQUENCER_TASK_HPP
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// task.hpp
//
// Description: A Sequencer unit of scheduling having a notion of execution
//              states.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 2) Owner: Laurent Ancessi
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------

// CLOSER #include "architecture/ctype.hpp"

//---------------------------------------------------------------------
// Constant
//---------------------------------------------------------------------
#include <p3d/refcounted.hpp>

enum Status
{
	DONE,
	FAILED,
	RUNNING,
	SLEEPING
};

//---------------------------------------------------------------------
// Structure and class
//---------------------------------------------------------------------

class Task
:
public tRefCounted
{
	public:
		Task(void);
		virtual ~Task(void);
		Status GetStatus(void);
		void SetStatus(Status status);
		bool IsSleeping(void);
		void Run(void);
		bool IsRunning(void);
		void Done(void);
		bool IsDone(void);
	private:
		Status m_status;
};

#endif // HC_SEQUENCER_TASK_HPP
