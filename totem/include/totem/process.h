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

#ifndef _TOTEM_PROCESS_H
#define _TOTEM_PROCESS_H

#include <totem/base.h>
#include <totem/linklist.h>
#include <totem/memory.h>

#include <totem/ia32/process.h>

#define RUNQUEUE_NR			8

#define RUNQUEUE_LOWEST_PRIORITY	0
#define RUNQUEUE_HIGHEST_PRIORITY	(RUNQUEUE_NR-1)

extern volatile ULONG_T GlobalClockTickCounter;

#define PROCESS_STATE_RUNNING		0
#define PROCESS_STATE_RUNNABLE		1
#define PROCESS_STATE_BLOCKED		2
#define PROCESS_STATE_EXPIRED		3

#define PROCESS_PRIORITY_LOWEST		RUNQUEUE_LOWEST_PRIORITY
#define PROCESS_PRIORITY_HIGHEST	RUNQUEUE_HIGHEST_PRIORITY

#define PROCESS_PRIORITY_IDLE		PROCESS_PRIORITY_LOWEST
#define PROCESS_PRIORITY_LOW		(PROCESS_PRIORITY_LOWEST+1)
#define PROCESS_PRIORITY_NORMAL		(PROCESS_PRIORITY_LOWEST+2)
#define PROCESS_PRIORITY_HIGH		(PROCESS_PRIORITY_LOWEST+3)
#define PROCESS_PRIORITY_REAL		PROCESS_PRIORITY_HIGHEST

/* Don't change this without changing assembler code */
typedef struct
{
	ULONG_T		state;			/* TIMER_LEVEL */
	ULONG_T		priority;		/* TIMER_LEVEL */
	ULONG_T		reserved;		
	ULONG_T		quantum;		/* TIMER_LEVEL */
	ULONG_T		quantum_left;		/* TIMER_LEVEL */
	ULONG_T		counter;		/* TIMER_LEVEL */
	ULONG_T		timer;			/* TIMER_LEVEL */
		
	VOID*		stack;			/* ATOMIC_LEVEL */
	VOID*		stack_origin;		/* SYNCH_LEVEL */

	VOID*		heap;			/* SYNCH_LEVEL */
	VOID*		heap_origin;		/* SYNCH_LEVEL */

	ULONG_T		error_code;		/* SYNCH_LEVEL */

	DLINK_NODE_T	run_queue_node;		/* TIMER_LEVEL */
	
	TAG_T		tag;			/* SYNCH_LEVEL */

} PROCESS_T;

#define PROCESS_GRANULARITY		(PAGE_SIZE*2)
#define PROCESS_XSTACK_SIZE		(PROCESS_GRANULARITY-sizeof(PROCESS_T))

#define PROCESS_POINTER_TO_ID(p)	(ULONG_T)(p)
#define PROCESS_ID_TO_POINTER(id)	(PROCESS_T*)(id)

typedef LONG_T (* PROCESS_ENTRY_HANDLER_T )( VOID* param );
typedef VOID (* PROCESS_EXIT_HANDLER_T )();

STATUS_T CreateProcess( ULONG_T state, ULONG_T priority, ULONG_T mode, PROCESS_ENTRY_HANDLER_T entry, VOID* param, PROCESS_EXIT_HANDLER_T exit, ULONG_T* id );
STATUS_T TerminateProcess( ULONG_T id, ULONG_T exit_code );

extern volatile PROCESS_T* GlobalCurrentProcessPointer;




#endif /* _TOTEM_PROCESS_H */
