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
#include <totem/base.h>
#include <totem/bytecopy.h>
#include <totem/dispatch.h>
#include <totem/display.h>
#include <totem/memory.h>
#include <totem/status.h>


#define SYSCALL_NR_NO_OP	0
#define SYSCALL_TABLE_SIZE	1


static volatile DISPATCH_VECTOR_T syscall00_vector_;
static volatile DISPATCH_VECTOR_T syscall02_vector_;

STATUS_T
system_no_op_()
{
	DISPLAY( DDEBUG, "SystemNoOp called.\n" );
	
	return STATUS_SUCCESS;	
}

static
BOOLEAN_T
syscall00_handler_(
	IN ULONG_T vector_nr, 
	IN VOID* param, 
	IN OUT DISPATCH_FRAME_T* frame
	)
{
	/* It this is a request for the scheduler.
	 * The scheduler will be invoked implicitly
	 * when the dispatch level returns to normal.
	 */
	return TRUE;		
}

static
BOOLEAN_T
syscall02_handler_(
	IN ULONG_T vector_nr, 
	IN VOID* param, 
	IN OUT DISPATCH_FRAME_T* frame
	)
{
	system_no_op_();

	return TRUE;		
}

VOID
init_system_calls_()
{
	
	if ( RequestDispatchVector(
		SYSCALL_VECTOR_00, 
		syscall00_handler_, 
		NULL,
		TRUE, 
		&syscall00_vector_
		) != STATUS_SUCCESS 
		)
	{
		DISPLAY( 
			DCRITICAL, 
			"\nABORT: Could not acquire system call vector 0.\n"
			);
	}

	if ( RequestDispatchVector(
		SYSCALL_VECTOR_02, 
		syscall02_handler_, 
		NULL,
		TRUE, 
		&syscall02_vector_
		) != STATUS_SUCCESS 
		)
	{
		DISPLAY( 
			DCRITICAL, 
			"\nABORT: Could not acquire system call vector 2.\n"
			);
	}
}
