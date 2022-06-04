/**
 *    Copyright (c) 2003, 2006 Bryan Bullard.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without 
 * restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, subject to the following 
 * conditions:
 *
 *   The above copyright notice and this permission notice 
 *   shall be included in all copies or substantial portions 
 *   of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
 * OTHER DEALINGS IN THE SOFTWARE.
 *
**/

#ifndef _TOTEM_DISPATCH_H
#define _TOTEM_DISPATCH_H


#include <totem/base.h>
#include <totem/linklist.h>
#include <totem/ia32/dispatch.h>


typedef ULONG_T DISPATCH_LEVEL_T;

struct
{
	DISPATCH_LEVEL_T	level;
	ULONG_T			irq;
	ULONG_T			sharing;
	DLINK_LIST_T		vector_list;
	BOOLEAN_T		enabled;
	BOOLEAN_T		share;
	
#ifndef TOTEM_NSTATS	
	ULONG_T			counter;
#endif
} DISPATCH_VECTOR_MAP_ENTRY_T;

extern volatile DISPATCH_VECTOR_MAP_ENTRY_T GlobalDispatchVectorMap[NR_DISPATCH_VECTORS];


typedef BOOLEAN_T (* DISPATCH_HANDLER_T )( ULONG_T vector_nr, VOID* param, DISPATCH_FRAME_T* frame );  

typedef struct
{
	DISPATCH_HANDLER_T	handler;
	VOID*			param;
	ULONG_T			vector_nr;
	DLINK_NODE_T		node;
	TAG_T			tag;
} DISPATCH_VECTOR_T;

VOID RaiseDispatchLevel( DISPATCH_LEVEL_T level, DISPATCH_LEVEL_T* prev ); 
VOID RestoreDispatchLevel( DISPATCH_LEVEL_T* level );
DISPATCH_LEVEL_T GetDispatchLevel( VOID );


STATUS_T RequestDispatchVector( ULONG_T vector_nr, DISPATCH_HANDLER_T handler, VOID* param, BOOLEAN_T share, volatile DISPATCH_VECTOR_T* intrvector );
STATUS_T ReleaseDispatchVector( volatile DISPATCH_VECTOR_T* vector );


#endif /* _TOTEM_DISPATCH_H */
