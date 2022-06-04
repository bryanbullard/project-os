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
#include <totem/status.h>
#include <totem/dispatch.h>
#include <totem/abort.h>
#include <totem/display.h>


#define CONTEXT GlobalAbortContext


static volatile DISPATCH_VECTOR_T de_;	/* Divide By Zero */
static volatile DISPATCH_VECTOR_T bp_;	/* Breakpoint */
static volatile DISPATCH_VECTOR_T ud_;	/* Undefined Opcode */
static volatile DISPATCH_VECTOR_T gp_;	/* General Protection Fault */




static
BOOLEAN_T
fault_vector_handler_(
	IN ULONG_T vector_nr, 
	IN VOID* param, 
	IN OUT DISPATCH_FRAME_T* frame
	)
{
	
	CONTEXT.cpu = 0;
	CONTEXT.dispatch_level = frame->prev_level;
	CONTEXT.pid = 0;
	CONTEXT.cr4 = 0;
	CONTEXT.cr3 = 0;
	CONTEXT.cr2 = 0;
	CONTEXT.cr0 = 0;
	CONTEXT.gs = frame->gs;
	CONTEXT.fs = frame->fs;
	CONTEXT.es = frame->es;
	CONTEXT.ds = frame->ds;
	CONTEXT.edi = frame->edi;
	CONTEXT.esi = frame->esi;
	CONTEXT.ebp = frame->ebp;
	CONTEXT.ebx = frame->ebx;
	CONTEXT.edx = frame->edx;
	CONTEXT.ecx = frame->ecx;
	CONTEXT.eax = frame->eax;
	CONTEXT.esp = frame->esp0;
	CONTEXT.ss = frame->ss0;
	CONTEXT.eip = frame->eip;
	CONTEXT.cs = frame->cs;
	CONTEXT.eflags = frame->eflags;
	
	switch ( vector_nr )
	{
		case IA32_DE_00:
			DISPLAY( 
				DCRITICAL,
				"\nA divide by zero error occured at address 0x%p.\nError code: %p\n",
				frame->eip,
				frame->error_code
				);
			AbortWithContext( NULL );
			return TRUE;
			
		case IA32_BP_03:
			/* This is a trap not a fault, so fix EIP for size of "int3" */
			CONTEXT.eip = (CONTEXT.eip - 1);
			DISPLAY(
				DCRITICAL,
				"\nA breakpoint was encountered at address %p but no debugger is installed.\n"
				"The system will now abort.\n",
				CONTEXT.eip
				);
			AbortWithContext( NULL );
			return TRUE;
			
		case IA32_UD_06:
			DISPLAY(
				DCRITICAL,
				"\nA invalid opcode was encountered at address 0x%p.\nError code: %p\n",
				frame->eip,
				frame->error_code
				);
			AbortWithContext( NULL );
			return TRUE;
			
		case IA32_GP_13:
			DISPLAY(
				DCRITICAL,
				"\nA general protection fault occured at address 0x%p.\nError code: %p\n",
				frame->eip,
				frame->error_code
				);
			AbortWithContext( NULL );
			return TRUE;
			
		default:
			break;
	}
	return FALSE;
}


VOID
init_fault_handlers_()
{
	STATUS_T status;
	
	status = RequestDispatchVector(
			IA32_DE_00, 
			fault_vector_handler_, 
			NULL, 
			FALSE, 
			&de_
			);
				
	if ( status != STATUS_SUCCESS )
		DISPLAY( 
			DCRITICAL, 
			"\nCould not acquire divide error vector.  These exceptions will not be reported.\n"
			);

	status = RequestDispatchVector(
			IA32_BP_03, 
			fault_vector_handler_, 
			NULL, 
			TRUE, 
			&bp_
			);
				
	if ( status != STATUS_SUCCESS )
		DISPLAY(
			DCRITICAL,
			"\nCould not acquire breakpoint vector.  These exceptions will not be reported.\n"
			);
	
	status = RequestDispatchVector(
			IA32_UD_06, 
			fault_vector_handler_, 
			NULL, 
			FALSE, 
			&ud_
			);
				
	if ( status != STATUS_SUCCESS )
		DISPLAY(
			DCRITICAL,
			"\nCould not acquire undefined opcode error vector.  These exceptions will not be reported.\n"
			);
		
	status = RequestDispatchVector(
			IA32_GP_13, 
			fault_vector_handler_, 
			NULL, 
			FALSE, 
			&gp_
			);
				
	if ( status != STATUS_SUCCESS )
		DISPLAY(
			DCRITICAL,
			"\nCould not acquire general protection fault vector.  These exceptions will not be reported.\n"
			);
	
	
}
