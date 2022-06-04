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

#include <totem/base.h>
#include <totem/bytecopy.h>
#include <totem/display.h>
#include <totem/arch.h>
#include <totem/abort.h>


volatile 
ABORT_CONTEXT_T GlobalAbortContext = { 0 };

volatile
static
CHAR_T* level_desc_array_[] = 
{
	"INVALID_LEVEL", /* 0 Invalid */
	"NORMAL_LEVEL",	/* 1 Normal */
	"SYNCH_LEVEL", /* 2 Synch */
	"DEVICE_SIGNAL_LEVEL_00/IRQ07", /* 3 IRQ07 */
	"DEVICE_SIGNAL_LEVEL_01/IRQ06", /* 4 IRQ06 */
	"DEVICE_SIGNAL_LEVEL_02/IRQ05", /* 5 IRQ05 */
	"DEVICE_SIGNAL_LEVEL_03/IRQ04", /* 6 IRQ04 */
	"DEVICE_SIGNAL_LEVEL_04/IRQ03", /* 7 IRQ03 */
	"DEVICE_SIGNAL_LEVEL_05/IRQ15", /* 8 IRQ15 */
	"DEVICE_SIGNAL_LEVEL_06/IRQ14", /* 9 IRQ14 */
	"DEVICE_SIGNAL_LEVEL_07/IRQ13", /* 10 IRQ13 */
	"DEVICE_SIGNAL_LEVEL_08/IRQ12", /* 11 IRQ12 */
	"DEVICE_SIGNAL_LEVEL_09/IRQ11", /* 12 IRQ11 */
	"DEVICE_SIGNAL_LEVEL_10/IRQ10", /* 13 IRQ10 */
	"DEVICE_SIGNAL_LEVEL_11/IRQ09", /* 14 IRQ09 */
	"DEVICE_SIGNAL_LEVEL_12/IRQ02", /* 15 IRQ08 or IRQ02 */
	"DEVICE_SIGNAL_LEVEL_13/IRQ01", /* 16 IRQ01 */
	"TIMER_LEVEL/DEVICE_SIGNAL_LEVEL_14/IRQ00", /* 17 IRQ00 */
	"PROCESSOR_SYNCH_LEVEL/IPI", /* 18 Inter-processor interrupts */
	"ATOMIC_LEVEL" /* 19 Non-preemptable execution */
};


VOID
AbortWithContext(
	IN ABORT_CONTEXT_T* context
	)
{	
	CHAR_T* level_desc = NULL;
	/*UINT32* frame_pointer = (UINT32*)GlobalAbortContextData.ebp;*/
	
	if ( context != NULL )
	{
		CopyBytes(
			(BYTE_T*)&GlobalAbortContext,
			(BYTE_T*)context,
			sizeof(ABORT_CONTEXT_T)
			);
	}
	
	if ( 
	GlobalAbortContext.dispatch_level <=  
	(sizeof(level_desc_array_)/sizeof(CHAR_T*)) 
	) level_desc = 
		(CHAR_T*)level_desc_array_[GlobalAbortContext.dispatch_level];

	DISPLAY(
		DCRITICAL,
		"\nAn abort was signaled on CPU %x at address %p:%p.\n"
		"Stop code: %d (0x%x)\n"
		"Dispatch level: %s(%d)\nProcess: %d\n\n",
		GlobalAbortContext.cpu,
		GlobalAbortContext.cs,
		GlobalAbortContext.eip,
		GlobalAbortContext.stop_code,
		GlobalAbortContext.stop_code,
		level_desc,
		GlobalAbortContext.dispatch_level,
		GlobalAbortContext.pid
		);
		
	DISPLAY(
		DCRITICAL,
		"EAX: %p     ECX: %p     EDX: %p     EBX: %p\n"
		"ESP: %p     ESI: %p     EDI: %p     EBP: %p\n",
		GlobalAbortContext.eax,
		GlobalAbortContext.ecx,
		GlobalAbortContext.edx,
		GlobalAbortContext.ebx,
		GlobalAbortContext.esp,
		GlobalAbortContext.esi,
		GlobalAbortContext.edi,
		GlobalAbortContext.ebp
		);
		
	DISPLAY(
		DCRITICAL,
		" CS: %p      SS: %p      DS: %p      ES: %p\n"
		" FS: %p      GS: %p     CR0: %p     CR2: %p\n",
		GlobalAbortContext.cs,
		GlobalAbortContext.ss,
		GlobalAbortContext.ds,
		GlobalAbortContext.es,
		GlobalAbortContext.fs,
		GlobalAbortContext.gs,
		GlobalAbortContext.cr0,
		GlobalAbortContext.cr2
		);
		
	DISPLAY(
		DCRITICAL,
		"CR3: %p     CR4: %p     EIP: %p  EFLAGS: %p\n\n",
		GlobalAbortContext.cr3,
		GlobalAbortContext.cr4,
		GlobalAbortContext.eip,
		GlobalAbortContext.eflags
		);
	
	DISPLAY(
		DCRITICAL,
		"Call Trace:\n"
		);
	/*
	{
		ULONG i = 0;
		
		while ( frame_pointer != NULL )
		{	
			DISPLAY(
				DCRITICAL,
				"%p\n",
				((*frame_pointer)+4)
				);
				
			frame_pointer = (UINT32*)*frame_pointer;
		}
	}
	*/
	ia32_cli_();
	ia32_hlt_();
}
