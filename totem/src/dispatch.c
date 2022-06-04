/**
 *    Copyright (c) 2003, 2006 Bryan Bullard.  All rights reserved.
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

#include <totem/arch.h>
#include <totem/atomic.h>
#include <totem/base.h>
#include <totem/stats.h>
#include <totem/status.h>
#include <totem/linklist.h>
#include <totem/bytecopy.h>
#include <totem/assert.h>
#include <totem/portio.h>
#include <totem/dispatch.h>
#include <totem/process.h>

/* #include <totem/display.h> */

extern VOID SetDispatchLevel( DISPATCH_LEVEL_T level );

DISPATCH_VECTOR_MAP_ENTRY_T GlobalDispatchVectorMap[NR_DISPATCH_VECTORS];

STATUS_T
RequestDispatchVector(
	IN ULONG_T vector_nr, 
	IN DISPATCH_HANDLER_T handler,
	IN VOID* param,
	IN BOOLEAN_T share,
	OUT volatile DISPATCH_VECTOR_T* vector
	)
{
	/* ATOMIC_LEVEL */
	
	if (
	( vector_nr >= NR_SYSTEM_VECTORS ) ||
	( handler == NULL ) ||
	( vector == NULL )
		) return STATUS_BAD_PARAM;
		
	if ( GlobalVectorMap[vector_nr].sharing != VECTOR_MAP_SHARE_AVAIL )
		return STATUS_NOT_AVAIL;
	
	if (
	( share == FALSE ) &&
	( GlobalVectorMap[vector_nr].vector_list.depth > 0 )
		) return STATUS_NOT_AVAIL;
	
	if ( share == FALSE )
		GlobalVectorMap[vector_nr].sharing = VECTOR_MAP_SHARE_LOCKED;
		
	DLinkPushHead(
		(DLINK_LIST_T*)&GlobalVectorMap[vector_nr].vector_list,
		(DLINK_NODE_T*)&vector->node,
		NULL
		);
		
	vector->node.data = (VOID*)vector;	
	vector->vector_nr = vector_nr;
	vector->handler = handler;
	vector->param = param;
	vector->tag = MAKE_TAG('V','E','C','T');
	
	return STATUS_SUCCESS;
}

STATUS_T
ReleaseDispatchVector(
	IN OUT volatile DISPATCH_VECTOR_T* vector 
	)
{	
	/* ATOMIC_LEVEL */

	if ( vector == NULL )
		return STATUS_BAD_PARAM;

	if ( 
	( vector->vector_nr >= NR_SYSTEM_VECTORS ) ||
	( vector->tag != MAKE_TAG('V','E','C','T') )
		) return STATUS_BAD_PARAM;

	DLinkRemove(
		(DLINK_LIST_T*)&GlobalVectorMap[vector->vector_nr].vector_list,
		(DLINK_NODE_T*)&vector->node,
		NULL
		);

	ZeroBytes(
		(BYTE_T*)vector,
		sizeof(DISPATCH_VECTOR_T)
		);
	
	GlobalVectorMap[vector->vector_nr].sharing = VECTOR_MAP_SHARE_AVAIL;

	return STATUS_SUCCESS;		
}

VOID 
RaiseDispatchLevel(
	IN DISPATCH_LEVEL_T level, 
	OUT DISPATCH_LEVEL_T* prev
	)
{
	ASSERT( *level != INVALID_LEVEL );
	ASSERT( level >= GetDispatchLevel() );
	
	/* Hopefully this is okay */
	if( level == ( *prev = GetDispatchLevel() ) ) 
		return;
		
	SetDispatchLevel(level);
}
	
VOID 
RestoreDispatchLevel(
	IN DISPATCH_LEVEL_T* level 
	)
{
	ASSERT( *level != INVALID_LEVEL );
	ASSERT( *level <= GetDispatchLevel() );
	
	if( *level == GetDispatchLevel() )
		return;
	
	SetDispatchLevel(*level);
}

VOID
dispatch_(
	IN OUT DISPATCH_FRAME_T frame
	)
{

	
	DISPLAY(DDEBUG, 
		"Entered Dispatcher prev_level=%d gs=%d fs=%d es=%d ds=%d edi=%d esi=%d ebp=%d ebx=%d edx=%d ecx=%d eax=%d esp0=0x%p ss0=%d level=%d, vector=%d, error=%d eip=0x%p, cs=%d, eflags=%d esp3=0x%p, ss3=%d\n",
		frame.prev_level,
		frame.gs,
		frame.fs,
		frame.es,
		frame.ds,
		frame.edi,
		frame.esi,
		frame.ebp,
		frame.ebx,
		frame.edx,
		frame.ecx,
		frame.eax,
		frame.esp0,
		frame.ss0,
		frame.signal_level,
		frame.vector_nr,
		frame.error_code,
		frame.eip,
		frame.cs,
		frame.eflags,
		frame.esp3,
		frame.ss3
		);
	
	ia32_cli_();
	ia32_hlt_();

	ASSERT( GetDispatchLevel() != INVALID_LEVEL );
	ASSERT( GetDispatchLevel() >= frame.prev_level );
	ASSERT( frame.vector_nr < NR_SYSTEM_VECTORS );
	
#ifndef TOTEM_NSTATS	
	++(GlobalSystemStatistics.vectors[frame.vector_nr]);
#endif	
	
	DLINK_NODE_T* pnode = GlobalVectorMap[frame.vector_nr].vector_list.head;
	DISPATCH_VECTOR_T* pvector = NULL;
	
	while (pnode != NULL)
	{
		pvector = pnode->data;
		
		ASSERT(pvector->handler);
		

		if ( pvector->handler(
			(ULONG_T)frame.vector_nr,
			pvector->param,
			(DISPATCH_FRAME_T*)&frame
			) == TRUE
			) break;
		
		pnode = pnode->next;
	}
/*	
	DISPLAY(DDEBUG,"Left Dispatcher\n");
*/
}
