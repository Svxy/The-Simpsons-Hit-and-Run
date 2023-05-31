//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:     atcloader.h
//
// Description: Loads the ATC, creates the AttributreTable, and sets the ATCManager's mp_ATCTable.
//
// History:     03/08/2002 + Created -- Chuck Chow
//
//=============================================================================

#ifndef ATCLOADER_H
#define ATCLOADER_H

//========================================
// Nested Includes
//========================================
#include <constants/srrchunks.h>
#include <p3d/loadmanager.hpp>


//========================================
// Forward References
//========================================



//=============================================================================
//
// Synopsis:Blah Blah 
//
//=============================================================================

class ATCLoader: public tSimpleChunkHandler
{
public:
    ATCLoader();
	virtual ~ATCLoader();

private:


    //Prevent wasteful constructor creation.
	ATCLoader( const ATCLoader&  atcloader);
	ATCLoader& operator=( const ATCLoader& atc );

    // P3D chunk loader.
    virtual tEntity* LoadObject(tChunkFile* f, tEntityStore* store);

};


#endif //ATCLOADER_H
