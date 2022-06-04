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
#include <totem/assert.h>
#include <totem/arch.h>
#include <totem/base.h>
#include <totem/bytecopy.h>
#include <totem/dispatch.h>
#include <totem/linklist.h>
#include <totem/memory.h>
#include <totem/stats.h>
#include <totem/status.h>

#include <totem/process.h>


static volatile DLINK_LIST_T process_run_queue_[RUNQUEUE_NR];

static volatile DLINK_LIST_T process_timers_;

static volatile DISPATCH_VECTOR_T timer_vector_;

static volatile PROCESS_T idle_process_;

volatile PROCESS_T* GlobalCurrentProcessPointer = NULL;
#define current GlobalCurrentProcessPointer

volatile ULONG_T GlobalClockTickCounter = 0;


static
BOOLEAN_T 
timer_interrupt_handler_(
	IN ULONG_T vector_nr, 
	IN VOID* param, 
	IN OUT DISPATCH_FRAME_T* frame
	)
{	
	GlobalClockTickCounter++;
	
	(current->counter)++;
	
	if ( current->quantum_left > 0 )
		(current->quantum_left)--;
	
	/* DISPLAY( DNOTICE, "current->quantum_left=%d\n", current->quantum_left ); */
	
	if (frame->prev_level == NORMAL_LEVEL)
	{
		DLINK_NODE_T* node = NULL;
		LONG_T runqueue_nr;
	
		if (
		( current->state == PROCESS_STATE_RUNNING ) &&
		( current->quantum_left > 0 )
			) return TRUE;
	
		if ( current->state == PROCESS_STATE_RUNNING )
			current->state = PROCESS_STATE_RUNNABLE;
	
		DLinkPushTail(
			(DLINK_LIST_T*)&(process_run_queue_[current->priority]),
			(DLINK_NODE_T*)&(current->run_queue_node),
			NULL
			);
	
		for(
		runqueue_nr = RUNQUEUE_HIGHEST_PRIORITY; 
		runqueue_nr >= RUNQUEUE_LOWEST_PRIORITY; 
		runqueue_nr--
		)
		{
			node = process_run_queue_[runqueue_nr].head;
				
			if ( node == NULL )
				continue;
				
			do
			{
				PROCESS_T* process = (PROCESS_T*)node->data;
						
				ASSERT( process->priority == runqueue_nr );
						
				if( process->state == PROCESS_STATE_RUNNABLE )
				{
					DLinkRemove(
						(DLINK_LIST_T*)&(process_run_queue_[runqueue_nr]), 
						node, 
						NULL
						);

					current = (PROCESS_T*)node->data;
						
					current->quantum_left = current->quantum;
					current->state = PROCESS_STATE_RUNNING;
							
#ifndef TOTEM_NSTATS	
					++(GlobalSystemStatistics.context_switches);
#endif
					/* DISPLAY( DNOTICE, "Switched to process %p\n", current ); */
					return TRUE;
				}
			}
			while ( ( node = node->next ) );
		}
		Abort(0);	
	}
	
	return TRUE;		
}

VOID
init_runqueue_()
{
	LONG_T runqueue_nr;

	GlobalClockTickCounter = 0;
	
	for(
	runqueue_nr = RUNQUEUE_HIGHEST_PRIORITY; 
	runqueue_nr >= RUNQUEUE_LOWEST_PRIORITY; 
	runqueue_nr--
		)
		InitializeDLinkList( 
			(DLINK_LIST_T*)&(process_run_queue_[runqueue_nr]) 
			);
	
	if ( RequestDispatchVector(
		IA32_IRQ_00, 
		timer_interrupt_handler_, 
		NULL,
		TRUE, 
		&timer_vector_
		) != STATUS_SUCCESS 
		)
	{
		DISPLAY( 
			DCRITICAL, 
			"\nABORT: Could not acquire timer vector.\n"
			);

		Abort(0);
	}
	
	/*
	 * Initialize idle process
	 */
	
	/* Note that we are using the inital stack for the idle process */

	idle_process_.quantum = 10;
	idle_process_.priority = PROCESS_PRIORITY_LOWEST;
	idle_process_.state = PROCESS_STATE_RUNNING;
	idle_process_.timer = 0;
	idle_process_.tag = MAKE_TAG( 'P','R','O','C' );
	
	DLinkPushTail(
		(DLINK_LIST_T*)&(process_run_queue_[PROCESS_PRIORITY_LOWEST]),
		(DLINK_NODE_T*)&(idle_process_.run_queue_node),
		NULL
		);
	
	idle_process_.run_queue_node.data = (VOID*)&idle_process_;
	
	GlobalCurrentProcessPointer = &idle_process_;
}

static volatile char process_cache[0x8000];
static volatile char* process_pointer = process_cache;

STATUS_T
CreateProcess(
	IN ULONG_T state,
	IN ULONG_T priority,
	IN ULONG_T reserved,
	IN PROCESS_ENTRY_HANDLER_T entry,
	IN VOID* param,
	IN PROCESS_EXIT_HANDLER_T exit,
	OUT ULONG_T* id
	)
{
	PROCESS_T* process = NULL;
	DISPATCH_LEVEL_T level = INVALID_LEVEL;
	
	ASSERT( ((PTRDIFF_T)entry) > ((PTRDIFF_T)VIRTUAL_NULL_GUARD_TOP) );
	ASSERT( priority <= PROCESS_PRIORITY_HIGHEST );
	ASSERT( priority >= PROCESS_PRIORITY_LOWEST );
	ASSERT( ((PTRDIFF_T)exit) > ((PTRDIFF_T)VIRTUAL_NULL_GUARD_TOP) );
	ASSERT( (VOID*)id > VIRTUAL_NULL_GUARD_TOP );
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	
	/*
	DISPLAY(
		DDEBUG,
		"CreateProcess state=%d,pri=%d,resv=%d,entry=%p,param=%d,exit=%p\n",
		state,
		priority,
		reserved,
		entry,
		param,
		exit
		);
	*/	
	
	process = (PROCESS_T*)process_pointer;
	
	process_pointer = (char*)((PTRDIFF_T)process_pointer + PROCESS_GRANULARITY );
	
	if ( process == NULL )
		return STATUS_NOT_AVAIL;
	
	INITAILIZE_PROCESS_STACK(process,entry,param,exit);
	
	process->quantum = 10;
	process->priority = priority;
	process->state = state;
	process->timer = 0;
	process->tag = MAKE_TAG( 'P','R','O','C' );
	
	*id = (ULONG_T)process;
	
	RaiseDispatchLevel( TIMER_LEVEL, &level );
	
	DLinkPushTail(
		(DLINK_LIST_T*)&(process_run_queue_[priority]),
		(DLINK_NODE_T*)&(process->run_queue_node),
		NULL
		);
	
	process->run_queue_node.data = (VOID*)process;

	RestoreDispatchLevel( &level );

#ifndef TOTEM_NSTATS	
	++(GlobalSystemStatistics.process_count);
#endif

	/*
	DISPLAY(
		DDEBUG,
		"CreateProcess id=%p\n",
		*id
		);
	*/
	
	return STATUS_SUCCESS;
}

STATUS_T
TerminateProcess(
	IN ULONG_T id,
	IN ULONG_T exit_code
	)
{
	DISPATCH_LEVEL_T level = INVALID_LEVEL;
	PROCESS_T* process = PROCESS_ID_TO_POINTER( id );
	
	if ( process == NULL )
		return STATUS_BAD_PARAM;
		
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	
	RaiseDispatchLevel( TIMER_LEVEL, &level );
	
	process->state = PROCESS_STATE_EXPIRED;
	
	RestoreDispatchLevel( &level );
	
	RaiseDispatchLevel( SYNCH_LEVEL, &level );
	
	process->error_code = exit_code;
	
	RestoreDispatchLevel( &level );
	
	return STATUS_SUCCESS;
}


STATUS_T
SetTimer(
	IN ULONG_T id,
	IN ULONG_T clock_ticks
	)
{
	return STATUS_SUCCESS;
}


STATUS_T
CancelTimer(
	IN ULONG_T id
	)
{
	return STATUS_SUCCESS;
}
