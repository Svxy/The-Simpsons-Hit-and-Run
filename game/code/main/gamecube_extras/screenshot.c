/*---------------------------------------------------------------------------*
  Project:  onetri
  File:     screenshot.h

  Copyright 1998, 1999, 2000 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  Created 05-25-01 by Steve Rabin
  
 *---------------------------------------------------------------------------*/

#include <main/gamecube_extras/screenshot.h>
#include <dolphin/hio.h>
#include <string.h>


#define WIDTH_SCREENSHOT 640
#define HEIGHT_SCREENSHOT 480
#define SIZE_SCREENSHOT_RGB WIDTH_SCREENSHOT*HEIGHT_SCREENSHOT*3
#define SIZE_SCREENSHOT_YUV WIDTH_SCREENSHOT*HEIGHT_SCREENSHOT*2

#define XFB_BASE 1
#define XFB_RANGE 6
#define EFB_BASE 11
#define EFB_RANGE 9
#define PING_BASE 20
#define PING_RANGE 9
#define MINIMIZE_BUFFER_TRUE 35
#define MINIMIZE_BUFFER_FALSE 36


typedef enum  { 
				GRAB_NOT_TRANSFERRING, 
				GRAB_TRANSFERRING 
} GRAB_STATUS;


//Globals
BOOL g_connected = FALSE;
BOOL g_ping_received = FALSE;
u32 g_received_signal = 0;
BOOL g_mail_waiting = FALSE;
BOOL g_minimize_buffer = FALSE;
s32 g_usb_channel = -1;
u8* g_data;


static BOOL HostIOEnumCallback( s32 chan )
{
	g_usb_channel = chan;
	return( FALSE );
}

static void HostIOCallback( void )
{	
	//Mail is waiting for us - don't look at it yet
	g_mail_waiting = TRUE;
}



static void CopyoutEFB( u8* image_buffer, u32 width, u32 height )
{
	u16 i, j;
	u32 color;
	
	for( j=0; j<height; j++ ) {
		for( i=0; i<width; i++ ) {
			GXPeekARGB( i, j, &color );
			image_buffer[2] = (u8)(color & 0x000000FF);
			image_buffer[1] = (u8)((color & 0x0000FF00) >> 8);
			image_buffer[0] = (u8)((color & 0x00FF0000) >> 16);
			image_buffer += 3;
		}
	}
}



static void CopyoutPortionEFB( u8* image_buffer, u32 width, u32 startByte, u32 totalBytes )
{
	u32 color;
	
	u16 x = (u16)((u32)(startByte/3) % width);
	u16 y = (u16)((u32)(startByte/3) / width);
	u32 component = startByte%3;
	
	u32 count = 0;
	while( count < totalBytes )
	{
		GXPeekARGB( x, y, &color );
		
		if( component == 0 ) {
			count++;
			image_buffer[0] = (u8)((color & 0x00FF0000) >> 16);
		}
		if( component <= 1 && count < totalBytes ) {
			count++;
			image_buffer[1 - component] = (u8)((color & 0x0000FF00) >> 8);
		}
		if( component <= 2 && count < totalBytes ) {
			count++;
			image_buffer[2 - component] = (u8)(color & 0x000000FF);
		}
		image_buffer += 3 - component;
		component = 0;
		
		x++;
		if( x >= width ) {
			x = 0;
			y++;
		}
	}
	

}



static void TakeScreenshotEFB( SCREENSHOTAllocator allocator )
{
	if( g_data == 0 ) {
		g_data = (u8*)allocator( SIZE_SCREENSHOT_RGB );
	}
	CopyoutEFB( g_data, WIDTH_SCREENSHOT, HEIGHT_SCREENSHOT );
	DCFlushRange( &g_data[0], SIZE_SCREENSHOT_RGB );
	OSReport( "SCREENSHOT: Saved off screenshot to local memory.\n" );
}



static void TakeScreenshotXFB( void* bufferXFB, SCREENSHOTAllocator allocator )
{
	void* buffer = bufferXFB;
	if( g_data == 0 ) {
		g_data = (u8*)allocator( SIZE_SCREENSHOT_YUV );
	}
	memcpy( g_data, buffer, SIZE_SCREENSHOT_YUV );
	DCFlushRange( &g_data[0], SIZE_SCREENSHOT_YUV );
	OSReport( "SCREENSHOT: Saved off screenshot to local memory.\n" );
}



static void WriteScreenshotPortionEFBtoUSB( u32 chunk, SCREENSHOTAllocator allocator )
{
	u32 i;
	u32 size1K = 1024;
	u32 size100K = size1K*100;
	if( g_data == 0 ) {
		g_data = (u8*)allocator( size1K );
	}
	for( i=0; i<100; i++ )
	{
		CopyoutPortionEFB( g_data, WIDTH_SCREENSHOT, (chunk*size100K)+(i*size1K), size1K );
		DCFlushRange( &g_data[0], size1K );
		while( !HIOWrite( 0x00000500 + (i*size1K), g_data, (s32)size1K ) ) {
			//Spin until communication is successful
		}
	}
}



static GRAB_STATUS GrabChunk( u32 chunk, 
							  void* bufferXFB,
							  SCREENSHOTAllocator allocator, 
							  SCREENSHOTDeallocator deallocator )
{
	if( chunk >= XFB_BASE && chunk < XFB_BASE + XFB_RANGE )
	{
		void* buffer = 0;
		
		if( chunk == XFB_BASE )
		{	//save off XFB screenshot
			if( !g_minimize_buffer ) {
				TakeScreenshotXFB( bufferXFB, allocator );
			}
		}
			
		//transfer chunk
		if( g_minimize_buffer ) {	
			u8* fb = (u8*)bufferXFB;
			buffer = (void*)( &fb[100*1024*(chunk - XFB_BASE)] );
		}
		else {
			buffer = (void*)( &g_data[100*1024*(chunk - XFB_BASE)] );
		}
		
		while( !HIOWrite( 0x00000500, buffer, 100*1024 ) ) {
			//Spin until communication is successful
		}
		OSReport( "SCREENSHOT: Wrote chunk #%d.\n", chunk );
				
		//communicate that the chunk is ready to be read
		while( !HIOWriteMailbox( chunk ) ) {
			//Spin until communication is successful
		}
		OSReport( "SCREENSHOT: Notify of write chunk #%d put in mailbox.\n", g_received_signal );
	}
	else if( chunk >= EFB_BASE && chunk < EFB_BASE + EFB_RANGE )
	{
		void* buffer;
		
		if( chunk == EFB_BASE )
		{	//save off EFB screenshot
			if( !g_minimize_buffer ) {
				TakeScreenshotEFB( allocator );
			}
		}

		if( g_minimize_buffer )
		{
			WriteScreenshotPortionEFBtoUSB( chunk - EFB_BASE, allocator );
		}
		else
		{	//transfer chunk
			buffer = (void*)( &g_data[100*1024*(chunk - EFB_BASE)] );
			while( !HIOWrite( 0x00000500, buffer, 100*1024 ) ) {
				//Spin until communication is successful
			}
		}
		OSReport( "SCREENSHOT: Wrote chunk #%d.\n", chunk );
		
		//communicate that the chunk is ready to be read
		while( !HIOWriteMailbox( chunk ) ) {
			//Spin until communication is successful
		}
		OSReport( "SCREENSHOT: Notify of write chunk #%d put in mailbox.\n", g_received_signal );
		
	}
	
	if( chunk == XFB_BASE + XFB_RANGE - 1 || 
		chunk == EFB_BASE + EFB_RANGE - 1 )
	{	//free screenshot buffer space
		if( g_data ) {
			deallocator( g_data );
			g_data = 0;
		}
		return( GRAB_NOT_TRANSFERRING );
	}
	else {
		return( GRAB_TRANSFERRING );
	}

}



static void CheckMail( void* bufferXFB,
					   SCREENSHOTAllocator allocator, 
					   SCREENSHOTDeallocator deallocator )
{
	u32 temp = 0;
	u32 escapeCount = 0;
	GRAB_STATUS status = GRAB_TRANSFERRING;
	while( status == GRAB_TRANSFERRING )
	{
		escapeCount++;
		if( g_mail_waiting )
		{
			if( HIOReadMailbox( &temp ) ) {
				g_mail_waiting = FALSE;
				status = GRAB_NOT_TRANSFERRING;
				if( temp >= PING_BASE && temp < PING_BASE + PING_RANGE )
				{	//ping message received
					while( !HIOWriteMailbox( temp ) ) {
						//Spin until communication is successful
					}
					OSReport( "SCREENSHOT: Sent ping back.\n" );
				}
				else if( (temp >= XFB_BASE && temp < XFB_BASE + XFB_RANGE) || 
						 (temp >= EFB_BASE && temp < EFB_BASE + EFB_RANGE) )
				{	//request for screen capture
					escapeCount = 0;
					g_received_signal = temp;
					OSReport( "SCREENSHOT: Grab screenshot request.\n" );
					status = GrabChunk( g_received_signal, bufferXFB, allocator, deallocator );
				}
				else if( temp == MINIMIZE_BUFFER_TRUE )
				{	//request to use minimal memory (100K), however game pauses
					g_minimize_buffer = TRUE;
				}
				else if( temp == MINIMIZE_BUFFER_FALSE )
				{	//request to not use minimal memory and keep game running
					g_minimize_buffer = FALSE;
				}
			}
		}
		if( !g_minimize_buffer )
		{	//Escape loop since were not pausing the game to transfer
			return;
		}
		if( g_minimize_buffer && escapeCount > 150000000 )
		{	//The PC screenshot app has died - escape
			if( g_data ) {
				deallocator( g_data );
				g_data = 0;
			}
			return;
		}
	}

}



static BOOL ConnectToUSB( void )
{
	if( !g_connected )
	{
		if( HIOEnumDevices( HostIOEnumCallback ) ) {
			if( g_usb_channel >= 0 ) {
				if( HIOInit( g_usb_channel, HostIOCallback ) ) {
					u32 temp = 0;
					HIOReadMailbox( &temp );
					g_connected = TRUE;
					OSReport( "SCREENSHOT: USB connected\n" );
				}
			}
		}
	}
	
	return( g_connected );
}



void SCREENSHOTService( void* bufferXFB,
						SCREENSHOTAllocator allocator, 
						SCREENSHOTDeallocator deallocator )
{
	if( !g_connected ) {
		ConnectToUSB();
	}
	else if( g_mail_waiting ) {
		CheckMail( bufferXFB, allocator, deallocator );
	}

}
