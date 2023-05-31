//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogqueuetype.h
//
// Description: Typedef for STL list of dialog queue elements.  Removed from
//              DialogPriorityQueue to avoid unnecessary dependency.
//
// History:     9/25/2002 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGQUEUETYPE_H
#define DIALOGQUEUETYPE_H

class DialogQueueElement;

#include <list>
#include <memory/stlallocators.h>

typedef std::list< DialogQueueElement*, s2alloc<DialogQueueElement*> > DialogQueueType;

#endif // DIALOGQUEUETYPE_H

