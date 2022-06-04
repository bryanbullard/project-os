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

#ifndef _TOTEM_IA32_ABORT_H
#define _TOTEM_IA32_ABORT_H


#include <totem/ia32/arch.h>

typedef struct
{
	UINT32_T	cpu;
	UINT32_T	dispatch_level;
	UINT32_T	pid;
	UINT32_T	cr4;
	UINT32_T	cr3;
	UINT32_T	cr2;
	UINT32_T	cr0;
	UINT32_T	gs;
	UINT32_T	fs;
	UINT32_T	es;
	UINT32_T	ds;
	UINT32_T	edi;
	UINT32_T	esi;
	UINT32_T	ebp;
	UINT32_T	ebx;
	UINT32_T	edx;
	UINT32_T	ecx;
	UINT32_T	eax;
	UINT32_T	esp;
	UINT32_T	ss;
	UINT32_T	eip;
	UINT32_T	cs;
	UINT32_T	eflags;
	UINT32_T	stop_code;
} ABORT_CONTEXT_T;


#define BreakPoint() __asm__("int $3")

#endif /* _TOTEM_IA32_ABORT_H */
