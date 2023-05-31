
//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleairender.h
//
// Description: Blahblahblah
//
// History:     04/02/2003 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================
#include <raddebugwatch.hpp>

#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <ai/vehicle/vehicleairender.h>
#include <ai/vehicle/vehicleai.h>
#include <ai/vehicle/trafficai.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <roads/roadmanager.h>

#include <ai/vehicle/vehicleai.h>

#include <p3d/debugdraw.hpp>

#include <memory/srrmemory.h>

static int rendercounter = 1;
VehicleAIRender* VehicleAIRender::spInstance = NULL;

VehicleAIRender* VehicleAIRender::GetVehicleAIRender()
{
    if( spInstance == NULL )
    {
        spInstance = new VehicleAIRender();
    }
    return spInstance;
}

void VehicleAIRender::Destroy()
{
    if( spInstance != NULL )
    {
        delete spInstance;
        spInstance = NULL;
    }
}


VehicleAIRender::VehicleAIRender( )
{
    rAssert( MAX_REGISTRATIONS <= DListArray::MAX_ELEMS );

    mAIShader = new tShader("simple");
    mAIShader->AddRef();

    mFreeSlots.Clear();
    mRegistered.Clear();

    for( int i=0; i<MAX_REGISTRATIONS; i++ )
    {
        mAIs[i].ai = NULL;
        mAIs[i].index = mFreeSlots.AddLast( &(mAIs[i]) );
        mAIs[i].show = false;

        rAssert( mAIs[i].index != -1 );
    }

    radDbgWatchAddFunction( "Drop Path Point", &DropPathPoint, NULL, "Vehicle AI Debug\\Path Finding");
}

VehicleAIRender::~VehicleAIRender()
{

    for( int i=0; i<MAX_REGISTRATIONS; i++ )
    {
        if( mAIs[i].ai != NULL )
        {
            UnregisterAI( mAIs[i].index );
        }
    }
   if( mAIShader != NULL )
    {
        mAIShader->ReleaseVerified();
        mAIShader = NULL;
    }

    radDbgWatchDelete( &DropPathPoint );
}

int VehicleAIRender::RegisterAI( void* ai, int type )
{
    if( ai == NULL )
    {
        return -1;
    }

    // make sure it's not already in the list
    for( int i=0; i<MAX_REGISTRATIONS; i++ )
    {
        if( mAIs[i].ai == ai )
        {
            return mAIs[i].index;
        }
    }

    // grab a free AI slot & set its values
    RegisteredAI* ra = (RegisteredAI*) mFreeSlots.GetLast();
    if( ra == NULL )
    {
        return -1;
    }
    ra->ai = ai;
    ra->show = false;
    ra->type = type;

    // remove from free list (should succeed cuz we just grabbed
    // it from the free list!)
    bool succeeded = mFreeSlots.Remove( ra->index );
    rAssert( succeeded );

    // add to registered list (should succeed because its 
    // existence in the free list implies that the registered list
    // isn't full.
    ra->index = mRegistered.AddLast( ra );
    rAssert( 0 <= ra->index && ra->index < MAX_REGISTRATIONS );

    // add the debug watch
    rendercounter = rendercounter + 1;
    char name[64];
    if( ra->type == TYPE_TRAFFIC_AI )
    {
        sprintf( name, "Vehicle AI Debug\\traffic (%d)", rendercounter );
    }
    else if( ra->type == TYPE_VEHICLE_AI )
    {
        sprintf( name, "Vehicle AI Debug\\%s (%d)", ((VehicleAI*)(ra->ai))->GetVehicle()->GetNameDangerous(), rendercounter);
    }
    else
    {
        rAssert( false );
    }
    radDbgWatchAddBoolean( &(ra->show), "Display", name );


    return ra->index;
}

void VehicleAIRender::UnregisterAI( int handle )
{
    rAssert( 0 <= handle && handle < MAX_REGISTRATIONS );
    if( handle < 0 || handle >= MAX_REGISTRATIONS )
    {
        return;
    }

    RegisteredAI* ra = (RegisteredAI*) mRegistered.GetDataAt(handle);
    if( ra == NULL )
    {
        return;
    }

    ra->ai = NULL;
    radDbgWatchDelete( &(ra->show) );
    ra->show = false;

    bool succeeded = mRegistered.Remove( ra->index );
    rAssert( succeeded );

    ra->index = mFreeSlots.AddLast( ra );
    rAssert( ra->index != -1 );

}

static bool sWhichPathEndpoint = 0;
static rmt::Vector sPathEndPoints[2];
static unsigned sNumPathPoints = 0;
static rmt::Vector sPathPoints[64];

void VehicleAIRender::Display()
{
    tColour blue, red, green, black, magenta, cyan;
    blue.Set( 0, 0, 255 );
    red.Set( 255, 0, 0 );
    green.Set( 0, 255, 0 );
    black.Set( 0, 0, 0 );
    cyan.Set( 0, 255, 255 );
    magenta.Set( 255, 0, 255 );

    pddiPrimStream* stream = NULL;

    int i = mRegistered.GetHead();
    while( 0 <= i && i < DListArray::MAX_ELEMS )
    {
        RegisteredAI* ra = (RegisteredAI*) mRegistered.GetDataAt(i);
        rAssert( ra != NULL );

        if( !ra->show )
        {
            i = mRegistered.GetNextOf(i);
            continue;
        }
        if( ra->ai == NULL )
        {
            i = mRegistered.GetNextOf(i);
            continue;
        }

        if( ra->type == TYPE_VEHICLE_AI )
        {
            VehicleAI* ai = (VehicleAI*) ra->ai;
            // get vehicle position.
            rmt::Vector pos;
            ai->GetPosition( &pos );

            rmt::Vector dest, nextDest;
            ai->GetDestination( dest );
            ai->GetNextDestination( nextDest );
            /*
            dest.y = pos.y;
            nextDest.y = pos.y;
            */

            // draw lines to first and next destinations
            stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
            if( stream )
            {
                stream->Vertex( ((pddiVector*)(&(pos))),  blue );
                stream->Vertex( ((pddiVector*)(&(dest))), blue );  
            }
            p3d::pddi->EndPrims( stream );

            stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
            if( stream )
            {
                stream->Vertex( ((pddiVector*)(&(pos))),      red );
                stream->Vertex( ((pddiVector*)(&(nextDest))), red );  
            }
            p3d::pddi->EndPrims( stream );


            RoadManager::PathElement lastElem;
            RoadSegment* lastSeg;
            float lastSegT;
            float lastRoadT;

            ai->GetLastPathInfo( lastElem, lastSeg, lastSegT, lastRoadT );

            rmt::Vector lastElemPos;

            if( lastElem.elem )
            {
                if( lastElem.type == RoadManager::ET_INTERSECTION )
                {
                    ((Intersection*)lastElem.elem)->GetLocation( lastElemPos );

#ifndef RAD_RELEASE
                    P3DDrawSphere(1.0f, lastElemPos, *mAIShader, magenta);
#endif
                }
                else
                {
                    Road* lastRoad = (Road*) lastElem.elem;

                    rmt::Vector vec0, vec1, vec2, vec3, start, end;
                    lastSeg->GetCorner(0, vec0);
                    lastSeg->GetCorner(1, vec1);
                    lastSeg->GetCorner(2, vec2);
                    lastSeg->GetCorner(3, vec3);

                    start = (vec0+vec3)*0.5f;
                    end = (vec1+vec2)*0.5f;

                    rmt::Vector segDir = end - start;

                    lastElemPos = start + segDir * lastSegT;


                    tColour useThisColor = (lastRoad->GetShortCut())? cyan : magenta;
                    stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C, 4 );
                    if( stream )
                    {
                        stream->Vertex( ((pddiVector*)(&(vec0))),  useThisColor );
                        stream->Vertex( ((pddiVector*)(&(vec1))), useThisColor );  
                        stream->Vertex( ((pddiVector*)(&(vec2))), useThisColor );  
                        stream->Vertex( ((pddiVector*)(&(vec3))), useThisColor );  
                    }
                    p3d::pddi->EndPrims( stream );
                }
            }

            // draw line from us to last element
            stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
            if( stream )
            {
                stream->Vertex( ((pddiVector*)(&(pos))),    black );
                stream->Vertex( ((pddiVector*)(&(lastElemPos))), black );  
            }
            p3d::pddi->EndPrims( stream );



            // display the segments array!
            int numSegs = ai->mNumSegments;

            if( numSegs > 0 )
            {
                VehicleAI::Segment* segs = ai->mSegments;
                stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C, numSegs+1 );
                if( stream )
                {
                    stream->Vertex( ((pddiVector*)(&(segs[0].mStart))),  green );
                    for( int j=0; j<numSegs; j++ )
                    {
                        stream->Vertex( ((pddiVector*)(&(segs[j].mEnd))), green );  
                    }
                }
                p3d::pddi->EndPrims( stream );
            }
                        


            // output states by projecting text from x-y coordinates onto screen coordinates
            char speed[128];
            char params[128];
            char state[64];
            char shortcut[128];
            sprintf( speed, "%.1f (%.1f) km/h", ai->GetVehicle()->GetSpeedKmh(), ai->GetDesiredSpeedKmh() );
            sprintf( params, "g : %.1f  b : %.1f  e : %.1f", ai->GetGas(), ai->GetBrake(), ai->GetHandBrake() );
            sprintf( state, "state : %d", ai->GetState() );
            sprintf( shortcut, "SC MIN : %d  MAX : %d  MOD : %d", 
                ai->GetMinShortcutSkill(), ai->GetMaxShortcutSkill(), ai->GetShortcutSkillMod() );

            rmt::Vector pos2 = ai->GetVehicle()->GetPosition();
            pos2.y += 3;

            p3d::context->WorldToDevice(pos2, &pos2);

            p3d::pddi->DrawString(speed, (int)pos2.x+1, (int)pos2.y+1, tColour(0,0,0)); 
            p3d::pddi->DrawString(speed, (int)pos2.x, (int)pos2.y, tColour(255,255,255)); 
            p3d::pddi->DrawString(params, (int)pos2.x+1, (int)pos2.y+16, tColour(0,0,0)); 
            p3d::pddi->DrawString(params, (int)pos2.x, (int)pos2.y+15, tColour(255,255,255)); 
            p3d::pddi->DrawString(state, (int)pos2.x+1, (int)pos2.y+31, tColour(0,0,0)); 
            p3d::pddi->DrawString(state, (int)pos2.x, (int)pos2.y+30, tColour(255,255,255)); 
            p3d::pddi->DrawString(shortcut, (int)pos2.x+1, (int)pos2.y+46, tColour(0,0,0)); 
            p3d::pddi->DrawString(shortcut, (int)pos2.x, (int)pos2.y+45, tColour(255,255,255)); 

        }
        else if( ra->type == TYPE_TRAFFIC_AI )
        {
            TrafficAI* ai = (TrafficAI*) ra->ai;
            if( ai->GetVehicle() == NULL )
            {
                i = mRegistered.GetNextOf(i);
                continue;
            }

            if( !ai->mIsActive )
            {
                i = mRegistered.GetNextOf(i);
                continue;
            }

            rmt::Vector aiPos;
            ai->GetVehicle()->GetPosition( &aiPos );

            // draw the lookahead position
            stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
            if( stream )
            {
                stream->Vertex( ((pddiVector*)(&(aiPos))),  blue );
                stream->Vertex( ((pddiVector*)(&(ai->mLookAheadPt))), blue );
            }
            p3d::pddi->EndPrims( stream );


            TrafficLocomotion* traffLoco = ai->GetVehicle()->mTrafficLocomotion;
            rAssert( traffLoco );

            // draw the segment position
            rmt::Vector segmentPos( 0.0f, 0.0f, 0.0f );
            rmt::Vector segmentDir( 0.0f, 1.0f, 0.0f );
            if( traffLoco->IsInIntersection() || 
                ai->GetState() == TrafficAI::SWERVING ||
                ai->GetState() == TrafficAI::LANE_CHANGING )
            {
                rmt::Vector* ways;
                int nWays;
                int currWay;

                traffLoco->GetSplineCurve( ways, nWays, currWay );

                if( currWay < (nWays-1) )
                {
                    segmentDir.Sub( ways[ currWay+1 ], ways[ currWay ] );
                    segmentPos = ways[ currWay ] + segmentDir * traffLoco->mCurrPathLocation;
                }
            }
            else
            {
                ai->GetSegment()->GetLaneLocation( ai->GetLanePosition(), 
                    ai->GetLaneIndex(), segmentPos, segmentDir );
            }
            segmentPos.y += 0.5f;
            #ifndef RAD_RELEASE
                P3DDrawSphere(0.5f, segmentPos, *mAIShader, tColour(0,255,255));
            #endif

            if( ai->GetState() == TrafficAI::LANE_CHANGING ||
                ai->GetState() == TrafficAI::SPLINING )
            {
                // if lane changing, render the lane change path
                TrafficLocomotion* tl = ai->GetVehicle()->mTrafficLocomotion;
                rAssert( tl );

                rmt::Vector* ways;
                int nWays, currWay;
                tl->GetSplineCurve( ways, nWays, currWay );

                if( nWays > 0 )
                {
                    stream = p3d::pddi->BeginPrims( mAIShader->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C, nWays );
                    if( stream )
                    {
                        for( int j=0; j<nWays; j++ )
                        {
                            if( j == currWay || ((j-1) == currWay) )
                            {
                                stream->Vertex( ((pddiVector*)(&(ways[j]))),  red );
                            }
                            else
                            {
                                stream->Vertex( ((pddiVector*)(&(ways[j]))),  green );
                            }
                        }
                    }
                    p3d::pddi->EndPrims( stream );
                }
            }

            // output states by projecting text from x-y coordinates onto screen coordinates
            char speed[128];
            char state[64];
            sprintf( speed, "%.1f (%.1f) km/h", ai->GetVehicle()->GetSpeedKmh(), ai->GetAISpeed()*3.6f );
            sprintf( state, "state : %d", ai->GetState() );

            rmt::Vector pos2 = ai->GetVehicle()->GetPosition();
            pos2.y += 3;

            p3d::context->WorldToDevice(pos2, &pos2);

            p3d::pddi->DrawString(speed, (int)pos2.x+1, (int)pos2.y+1, tColour(0,0,0)); 
            p3d::pddi->DrawString(speed, (int)pos2.x, (int)pos2.y, tColour(255,255,255)); 
            p3d::pddi->DrawString(state, (int)pos2.x+1, (int)pos2.y+31, tColour(0,0,0)); 
            p3d::pddi->DrawString(state, (int)pos2.x, (int)pos2.y+30, tColour(255,255,255)); 

        }
        i = mRegistered.GetNextOf(i);
    }

    #ifndef RAD_RELEASE
        if(sNumPathPoints)
        {
            for(unsigned i = 0; i < sNumPathPoints - 1; i++)
            {
                P3DDrawSphere(1.0f, sPathPoints[i], *mAIShader, tColour(0,255,255));
                P3DDrawCylinder( 0.3f, sPathPoints[i], sPathPoints[i+1], *mAIShader, tColour(255,0,255));
            }
            P3DDrawSphere(1.0f, sPathPoints[sNumPathPoints - 1], *mAIShader, tColour(0,255,255));
        }
    #endif
}

void VehicleAIRender::DropPathPoint(void*)
{
    /*
    GetCharacterManager()->GetCharacter(0)->GetPosition(sPathPoints[sNumPathPoints++]);
    */

    /*
    GetCharacterManager()->GetCharacter(0)->GetPosition(sPathEndPoints[sWhichPathEndpoint]);

    if(sWhichPathEndpoint == 0)
    {
        sNumPathPoints = 0;
        sWhichPathEndpoint = 1;
    }
    else
    {
        sWhichPathEndpoint = 0;
        sNumPathPoints = 2;
        sPathPoints[0] = sPathEndPoints[0]; 
        sPathPoints[1] = sPathEndPoints[1]; 
        sPathPoints[0].y += 3.0f;
        sPathPoints[1].y += 3.0f;
    }*/

    GetCharacterManager()->GetCharacter(0)->GetPosition(sPathEndPoints[sWhichPathEndpoint]);

    if(sWhichPathEndpoint == 0)
    {
        sNumPathPoints = 0;
        sWhichPathEndpoint = 1;
    }
    else
    {
        sWhichPathEndpoint = 0;


        RoadManager::PathElement sourceElem; 
        RoadManager::PathElement targetElem;
        float sourceT = 0.0f;
        float targetT = 0.0f;
        float sourceRoadT = 0.0f;
        float targetRoadT = 0.0f;
        RoadSegment* sourceSeg = NULL;
        RoadSegment* targetSeg = NULL;

        SwapArray<RoadManager::PathElement> path;
        path.Reserve(64);

        HeapMgr()->PushHeap( GMA_TEMP );
        path.Allocate();
        HeapMgr()->PopHeap( GMA_TEMP );

        bool ok = true;
        ok &= VehicleAI::FindClosestPathElement( sPathEndPoints[0], sourceElem, sourceSeg, sourceT, sourceRoadT, true );
        ok &= VehicleAI::FindClosestPathElement( sPathEndPoints[1], targetElem, targetSeg, targetT, targetRoadT, true );

        assert(ok);

        RoadManager::GetInstance()->FindPathElementsBetween(
            false,
            sourceElem, sourceRoadT, sPathEndPoints[0], 
            targetElem, targetRoadT, sPathEndPoints[1],
            path);

        for(int i = 0; i < path.mUseSize; i++)
        {
            switch(path[i].type)
            {
                case RoadManager::ET_INTERSECTION :
                    {
                        ((Intersection*)(path[i].elem))->GetLocation(sPathPoints[sNumPathPoints]);
                        sPathPoints[sNumPathPoints++].y += 3.0f;
                    }
                    break;
                case RoadManager::ET_NORMALROAD :
                    {
                    }
                    break;
            }
        }
        
    }


}
