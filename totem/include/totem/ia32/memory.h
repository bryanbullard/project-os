/**
 *    Copyright (c) 2003, 2006 Bryan Bullard
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

#ifndef _TOTEM_IA32_MEMORY_H
#define _TOTEM_IA32_MEMORY_H

#include <totem/base.h>
#include <totem/linklist.h>
#include <totem/ia32/arch.h>


/* Memory paging * * * * * * * * * * * * * * * * * * * * *
 */

#define PAGE_SIZE IA32_PAGE_SIZE

#define PTE_PAGE_FRAME_MASK		0xfffff000
#define PTE_PAGE_FLAGS_MASK		0x00000fff

#define MEMORY_FLAG_PRESENT		0x00000001 /* Is RAM memory present */ 
#define MEMORY_FLAG_WRITE_ACCESS 	0x00000002 /* Is memory writable */
#define MEMORY_FLAG_USER_ACCESS		0x00000004 /* Can memory be accessed in usermode */
#define MEMORY_FLAG_ACCESSED		0x00000020 /* Has memory been accessed (Read only) */
#define MEMORY_FLAG_WRITTEN		0x00000040 /* Has memory been written (Read only) */
#define MEMORY_FLAG_GLOBAL		0x00000100 /* Is memory mapped into all processes */
#define MEMORY_FLAG_ALLOCATED		0x00000200 /* Is memory allocated for use in RAM or swap */
#define MEMORY_FLAG_LOCKED		0x00000400 /* Memory will not be swapped to disk */

#define MEMORY_FLAG_MAX			PTE_PAGE_FLAGS_MASK
#define MEMORY_FLAG_MIN			0x00000000

#define MEMORY_STORAGE_MAX		PTE_PAGE_FRAME_MASK
#define MEMORY_STORAGE_MIN		PTE_PAGE_FLAGS_MASK



/* Layout of virtual address space * * * * * * * * * * * *
 */

#define VIRTUAL_ARCH_BASE		(VOID*)0x00000000

#define VIRTUAL_NULL_GUARD_TOP		(VOID*)0x00000FFF

#define VIRTUAL_XCORE_BASE		(VOID*)0x00100000 /* 16 MBs wide */
#define VIRTUAL_XCORE_TOP		(VOID*)0x00FFFFFF

#define VIRTUAL_USPACE_BASE		(VOID*)0x01000000 /* Nearly 3 GBs wide */

#define VIRTUAL_UCODEDATA_BASE		(VOID*)0x01000000 /* Nearly 1 GB wide */
#define VIRTUAL_UCODEDATA_TOP		(VOID*)0x3FFFFFFF

#define VIRTUAL_UHEAP_BASE		(VOID*)0x40000000 /* 1 GB wide */
#define VIRTUAL_UHEAP_TOP		(VOID*)0x7FFFFFFF

#define VIRTUAL_USHARED_BASE		(VOID*)0x80000000 /* 1 GB wide */
#define VIRTUAL_USHARED_TOP		(VOID*)0xBFFFFFFF

#define VIRTUAL_USPACE_TOP		(VOID*)0xBFFFFFFF

#define VIRTUAL_XSPACE_BASE		(VOID*)0xC0000000 /* 1 GB wide */

#define VIRTUAL_XMODULES_BASE		(VOID*)0xC0000000 /* 256 MBs wide */
#define VIRTUAL_XMODULES_TOP		(VOID*)0xCFFFFFFF

#define VIRTUAL_XHEAP_BASE 		(VOID*)0xD0000000 /* 512 MBs wide */
#define VIRTUAL_XHEAP_TOP		(VOID*)0xEFFFFFFF

#define VIRTUAL_XPROCESSES_BASE		(VOID*)0xF0000000 /* 128 MBs wide */
#define VIRTUAL_XPROCESSES_TOP		(VOID*)0xF7FFFFFF

#define VIRTUAL_XRESERVED_BASE		(VOID*)0xF8000000 /* 124 MBs wide */
#define VIRTUAL_XRESERVED_TOP		(VOID*)0xFFBFFFFF

#define VIRTUAL_XPAGE_TBLS_BASE		(VOID*)0xFFC00000 /* Mapped page tables take 4 MBs */
#define VIRTUAL_XPAGE_TBLS_TOP		(VOID*)0xFFFFFFFF
#define VIRTUAL_XPAGE_DIR_BASE		(VOID*)0xFFFFF000
#define VIRTUAL_XPAGE_DIR_TOP		(VOID*)0xFFFFFFFF		

#define VIRTUAL_XSPACE_TOP		(VOID*)0xFFFFFFFF

#define VIRTUAL_ARCH_TOP		(VOID*)0xFFFFFFFF

/* IA23 cache line boundry is 32 bytes */
#define CACHE_GRANULARITY		32


#endif /* _TOTEM_IA32_MEMORY_H */
