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


#ifndef __SCREENSHOT_H__
#define __SCREENSHOT_H__

#include "dolphin.h"


#ifdef __cplusplus
extern "C" {
#endif

// access to the memory allocator of choice
typedef void*(*SCREENSHOTAllocator)   ( u32 size      );
typedef void (*SCREENSHOTDeallocator) ( void* block   );


void SCREENSHOTService( void* bufferXFB, 
						SCREENSHOTAllocator allocator, 
						SCREENSHOTDeallocator deallocator );


#ifdef __cplusplus
}
#endif


#endif	// __SCREENSHOT_H__