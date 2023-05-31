//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        selectabledialoglist.h
//
// Description: Typedef for STL list of selectable dialogs.  Removed from
//              DialogList to avoid unnecessary dependency.
//
// History:     9/25/2002 + Created -- Darren
//
//=============================================================================

#ifndef SELECTABLEDIALOGLIST_H
#define SELECTABLEDIALOGLIST_H

class SelectableDialog;

#include <list>
#include <memory/stlallocators.h>

typedef std::list< SelectableDialog*, s2alloc<SelectableDialog*> > SelectableDialogList;

#endif // SELECTABLEDIALOGLIST_H

