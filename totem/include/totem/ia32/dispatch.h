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

#ifndef _TOTEM_IA32_DISPATCH_H
#define _TOTEM_IA32_DISPATCH_H

#include <totem/base.h>
#include <totem/ia32/arch.h>


#define NR_DISPATCH_VECTORS	SYSCALL_VECTOR_02+1


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Dispatch levels
 */
#define HIGHEST_LEVEL		19
#define ATOMIC_LEVEL		19	/* Non-preemptable execution */
#define PROCESSOR_SYNCH_LEVEL	18	/* Inter-processor interrupts */
#define TIMER_LEVEL		17	/* System timer */
#define DEVICE_SIGNAL_LEVEL_14	17	/* IRQ00 */
#define DEVICE_SIGNAL_LEVEL_13	16	/* IRQ01 */
#define DEVICE_SIGNAL_LEVEL_12	15	/* IRQ08 or IRQ02 */
#define DEVICE_SIGNAL_LEVEL_11	14	/* IRQ09 */
#define DEVICE_SIGNAL_LEVEL_10	13	/* IRQ10 */
#define DEVICE_SIGNAL_LEVEL_09	12	/* IRQ11 */
#define DEVICE_SIGNAL_LEVEL_08	11	/* IRQ12 */
#define DEVICE_SIGNAL_LEVEL_07	10	/* IRQ13 */
#define DEVICE_SIGNAL_LEVEL_06	9	/* IRQ14 */
#define DEVICE_SIGNAL_LEVEL_05	8	/* IRQ15 */
#define DEVICE_SIGNAL_LEVEL_04	7	/* IRQ03 */
#define DEVICE_SIGNAL_LEVEL_03	6	/* IRQ04 */
#define DEVICE_SIGNAL_LEVEL_02	5	/* IRQ05 */
#define DEVICE_SIGNAL_LEVEL_01	4	/* IRQ06 */
#define DEVICE_SIGNAL_LEVEL_00	3	/* IRQ07 */
#define SYNCH_LEVEL		2	/* Synchronous execution */
#define NORMAL_LEVEL		1	/* Normal (Asynchronous execution) */
#define LOWEST_LEVEL		1
#define INVALID_LEVEL		0	/* Invalid */

typedef struct
{
	UINT32_T	prev_level;
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
	UINT32_T	esp0;
	UINT32_T	ss0;
	UINT32_T	signal_level;
	UINT32_T	vector_nr;
	UINT32_T	error_code;
	UINT32_T	eip;
	UINT32_T	cs;
	UINT32_T	eflags;
	UINT32_T	esp3;
	UINT32_T	ss3;
} DISPATCH_FRAME_T;


#endif /* _TOTEM_IA32_DISPATCH_H */
