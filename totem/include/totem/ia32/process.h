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

#ifndef _TOTEM_IA32_PROCESS_H
#define _TOTEM_IA32_PROCESS_H

#include <totem/arch.h>
#include <totem/base.h>

#define INITAILIZE_PROCESS_STACK(process_pointer,process_entry,process_param,process_exit); \
{ \
	UINT32_T* stack_frame = (UINT32_T*)(((PTRDIFF_T)process_pointer + PROCESS_GRANULARITY) - 1); \
	process_pointer->stack_origin = (VOID*)stack_frame; \
	*stack_frame--; \
	*stack_frame-- = (UINT32_T)process_param; \
	*stack_frame-- = (UINT32_T)process_exit; \
	*stack_frame-- = 0x202;	/* eflags IF=1 IOPL=0 */ \
	*stack_frame-- = IA32_EXECUTIVE_CODE_SEL; /* cs */ \
	*stack_frame-- = (UINT32_T)process_entry; /* eip */ \
	*stack_frame-- = 0; /* used by dispatcher (error code) */ \
	*stack_frame-- = 0; /* used by dispatcher (vector #) */ \
	*stack_frame-- = 0; /* used by dispatcher ( new dispatch level ) */ \
	*stack_frame-- = 0; /* used by dispatcher ( ring0 ss:esp ) */ \
	*stack_frame-- = 0; /* used by dispatcher ( ring0 ss:esp ) */ \
	*stack_frame-- = 0; /* eax */ \
	*stack_frame-- = 0; /* ecx */ \
	*stack_frame-- = 0; /* edx */ \
	*stack_frame-- = 0; /* ebx */ \
	*stack_frame-- = 0; /* ebp */ \
	*stack_frame-- = 0; /* esi */ \
	*stack_frame-- = 0; /* edi */ \
	*stack_frame-- = IA32_EXECUTIVE_DATA_SEL; /* ds */ \
	*stack_frame-- = IA32_EXECUTIVE_DATA_SEL; /* es */ \
	*stack_frame-- = 0; /* fs */ \
	*stack_frame = 0; /* gs */ \
	process_pointer->stack = (VOID*)stack_frame; \
}
 
#endif /* _TOTEM_IA32_PROCESS_H */

