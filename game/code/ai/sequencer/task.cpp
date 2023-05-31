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


#include <ai\sequencer\task.h>

//---------------------------------------------------------------------
// Main functions
//---------------------------------------------------------------------

Task::Task(void)
{
	// Reset the class
	m_status = SLEEPING;
}

Task::~Task(void)
{
}

Status Task::GetStatus(void)
{
	// Return the status of the task
	return m_status;
}

void Task::SetStatus(Status status)
{
	// Set the status of the task
	m_status = status;
}

bool Task::IsSleeping(void)
{
	// Is the status set to SLEEPING
	return m_status == SLEEPING;
}

void Task::Run(void)
{
	// Set the status to RUNNING
	m_status = RUNNING;
}

bool Task::IsRunning(void)
{
	// Is the status set to RUNNING
	return m_status == RUNNING;
}

void Task::Done(void)
{
	// Set the status to DONE
	m_status = DONE;
}

bool Task::IsDone(void)
{
	// Is the status set to DONE
	return m_status == DONE;
}

// End of file.
