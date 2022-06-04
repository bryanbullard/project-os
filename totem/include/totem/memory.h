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

#ifndef _TOTEM_MEMORY_H
#define _TOTEM_MEMORY_H


#include <totem/base.h>
#include <totem/ia32/memory.h>


/* This will align an address to its low page boundary */
#define PAGE_ALIGN(addr)	(((PTRDIFF_T)(addr)) & (~(PAGE_SIZE - 1)))

#define MEMORY_PRESENT(f)	(MEMORY_FLAG_PRESENT & (f))
#define MEMORY_WRITE_ACCESS(f)	(MEMORY_FLAG_WRITE_ACCESS & (f))
#define MEMORY_USER_ACCESS(f)	(MEMORY_FLAG_USER_ACCESS & (f))
#define MEMORY_ACCESSED(f)	(MEMORY_FLAG_ACCESSED & (f))
#define MEMORY_WRITTEN(f)	(MEMORY_FLAG_WRITTEN & (f))
#define MEMORY_GLOBAL(f)	(MEMORY_FLAG_GLOBAL & (f))
#define MEMORY_ALLOCATED(f)	(MEMORY_FLAG_ALLOCATED & (f))
#define MEMORY_LOCKED(f)	(MEMORY_FLAG_LOCKED & (f))
#define MEMORY_SWAPPED(f)	((MEMORY_FLAG_ALLOCATED & (f)) && (!(MEMORY_FLAG_PRESENT & (f))))

#define MEMORY_NALLOC_NPRES(f)	((!(MEMORY_FLAG_ALLOCATED & (f))) && (!(MEMORY_FLAG_PRESENT & (f))))
#define MEMORY_ALLOC_PRES(f)	((MEMORY_FLAG_ALLOCATED & (f)) && (MEMORY_FLAG_PRESENT & (f)))
#define MEMORY_ALLOC_NPRES(f)	((MEMORY_FLAG_ALLOCATED & (f)) && (!(MEMORY_FLAG_PRESENT & (f))))

#define ALLOC_PAGE_NORMAL	0x00000001
#define ALLOC_PAGE_HIGH		0x00000002
#define ALLOC_PAGE_CRITICAL	0x00000003

#define ALLOC_PAGE_TYPE_NATIVE	0x00000010
#define ALLOC_PAGE_TYPE_SWAP	0x00000011

SIZE_T GetPhysicalMemorySize( VOID );

STATUS_T AllocatePage( ULONG_T type, ULONG_T priority, PHYSICAL_ADDRESS_T* page );
STATUS_T FreePage( ULONG_T type, PHYSICAL_ADDRESS_T page );
STATUS_T ZeroPage( PHYSICAL_ADDRESS_T* page );

STATUS_T GetMemoryMapping( BYTE_T* addr, ULONG_T* flags, PHYSICAL_ADDRESS_T* page );
STATUS_T SetMemoryMapping( BYTE_T* addr, ULONG_T flags, PHYSICAL_ADDRESS_T* page );

STATUS_T LockMemory( BYTE_T* addr, SIZE_T bytes );
STATUS_T UnlockMemory( BYTE_T* addr, SIZE_T bytes );

VOID* AllocateMemory( SIZE_T bytes, TAG_T tag );
VOID FreeMemory( VOID* buffer );

extern volatile ULONG_T GlobalTotalPages;
extern volatile ULONG_T GlobalCommittedPages;
extern volatile ULONG_T GlobalFreePages;
extern volatile ULONG_T GlobalSwapPages;
extern volatile ULONG_T GlobalNonSwapPages;

#define GetTotalPages()		GlobalTotalPages
#define GetCommittedPages()	GlobalCommittedPages
#define GetFreePages()		GlobalFreePages
#define GetSwapPages()		GlobalSwapPages
#define GetNonSwapPages()	GlobalNonSwapPages


#endif /* _TOTEM_MEMORY_H */
