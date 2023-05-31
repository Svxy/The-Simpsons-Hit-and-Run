//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficvehicle.h
//
// Description: Blahblahblah
//
// History:     02/01/2003 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef TRAFFICMODELGROUP_H
#define TRAFFICMODELGROUP_H

#include <string.h>
#include <raddebug.hpp>

//******************************************************************************
//
// TRAFFIC MODEL 
//
//******************************************************************************

class TrafficModel
{
public:
    enum 
    {
        MAX_STRING_LEN = 64
    };

    // TRACKING
    int mNumInstances;

    // DATA
    char mModelName[MAX_STRING_LEN+1];
    int mMaxInstances;

public:
    TrafficModel();
    void ClearData();
    void Init( const char* name, int num );

private:

};

inline void TrafficModel::ClearData()
{
    mModelName[0] = '\0';
    mMaxInstances = 0;
    mNumInstances = 0;
    // TODO:
    // We may not need to clear mNumInstances if we're going to initialize traffic vehicles
    // on the fly. Imagine loading a new model group that shares this same model as
    // the previous group. We don't want to lose the count. 
}

inline TrafficModel::TrafficModel()
{
    ClearData();
}

inline void TrafficModel::Init( const char* name, int num )
{
    rTuneAssert( name != NULL );
    rTuneAssert( num > 0 );

    int nameLen = strlen( name );

    rTuneAssert( nameLen <= TrafficModel::MAX_STRING_LEN );

    strncpy( mModelName, name, TrafficModel::MAX_STRING_LEN );
    mModelName[ nameLen ] = '\0';
    mMaxInstances = num;
}

//******************************************************************************
//
// TRAFFIC MODEL GROUP
//
//******************************************************************************

class TrafficModelGroup
{
public:
    TrafficModelGroup();
    void ClearGroup();
    void AddTrafficModel( const char* name, int num );
    TrafficModel* GetTrafficModel( int i );
    int GetNumModels();

private:
    enum 
    {
        MAX_TRAFFIC_MODELS = 5
    };
    TrafficModel mTrafficModels[MAX_TRAFFIC_MODELS];
    int mNumModels;
};

inline TrafficModelGroup::TrafficModelGroup()
{
    mNumModels = 0;
}
inline void TrafficModelGroup::ClearGroup()
{
    for( int i=0; i<mNumModels; i++ )
    {
        mTrafficModels[i].ClearData();
    }
    mNumModels = 0;
}
inline void TrafficModelGroup::AddTrafficModel( const char* name, int num )
{
    rTuneAssert( name != NULL );
    rTuneAssert( strlen(name) <= TrafficModel::MAX_STRING_LEN );
    rTuneAssert( num > 0 );

    bool found = false;
    for( int i=0; i<mNumModels; i++ )
    {
        if( strcmp( mTrafficModels[i].mModelName, name )==0 )
        {
            found = true;
            mTrafficModels[i].mNumInstances = num;
        }
    }

    if( !found )
    {
        mTrafficModels[mNumModels].Init( name, num );
        mNumModels++;
    }
}
inline TrafficModel* TrafficModelGroup::GetTrafficModel( int i )
{
    rTuneAssert( 0 <= i && i < TrafficModelGroup::MAX_TRAFFIC_MODELS );
    return &mTrafficModels[i];
}
inline int TrafficModelGroup::GetNumModels()
{
    return mNumModels;
}
#endif