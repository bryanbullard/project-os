/**
 *    Copyright (c) 2003, 2006  Bryan Bullard
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

#include <totem/abort.h>
#include <totem/arch.h>
#include <totem/assert.h>
#include <totem/atomic.h>
#include <totem/base.h>
#include <totem/bytecopy.h>
#include <totem/dispatch.h>
#include <totem/display.h>
#include <totem/mltiboot.h>
#include <totem/status.h>

#include <totem/memory.h>


#define VIRT_ADDR_TO_PDE_INDEX(virt) ((((ULONG_T)(virt)>>22)&0x3ff))
#define VIRT_ADDR_TO_PTE_INDEX(virt) ((((ULONG_T)(virt)>>12)&0x3ff))
#define VIRT_ADDR_TO_PAGE_OFFSET(virt) (((ULONG_T)(virt)&0xfff))

/* Page table entry */
typedef UINT32_T PTE;

#define GET_PTE_POINTER(virt) \
	((PTE*)(VIRTUAL_XPAGE_TBLS_BASE + \
	(VIRT_ADDR_TO_PDE_INDEX(virt) * PAGE_SIZE) + \
	(VIRT_ADDR_TO_PTE_INDEX(virt) * 4)))

#define GET_PDE_POINTER(virt) \
	((PTE*)(VIRTUAL_XPAGE_DIR_BASE + \
	(VIRT_ADDR_TO_PDE_INDEX(virt) * sizeof(PTE))))

#define PAGETABLE_MEMORY_FLAGS	( \
	MEMORY_FLAG_PRESENT | \
	MEMORY_FLAG_WRITE_ACCESS | \
	MEMORY_FLAG_ALLOCATED | \
	MEMORY_FLAG_LOCKED )

/* Attr defaults to non-present, read-only, executive-only access */ 
#define CREATE_PDE(PhyPageTblAddr,Attr) \
	((PTE)PAGE_ALIGN(PhyPageTblAddr)|(Attr))
	
#define CREATE_PTE(PhyPageAddr,Attr) \
	((PTE)PAGE_ALIGN(PhyPageAddr)|(Attr))

volatile ULONG_T GlobalTotalPages;
volatile ULONG_T GlobalCommittedPages;
volatile ULONG_T GlobalFreePages;
volatile ULONG_T GlobalSwapPages;
volatile ULONG_T GlobalNonSwapPages;

static volatile DISPATCH_VECTOR_T pf_;	/* Page Fault vector */

static volatile PHYSICAL_ADDRESS_T* free_page_list_pointer_;
static volatile PHYSICAL_ADDRESS_T* free_page_list_base_;
static volatile PHYSICAL_ADDRESS_T* free_page_list_top_;

extern VOID _end;


#define FREE_PAGE_LIST_INIT(size); \
	free_page_list_base_ = (PHYSICAL_ADDRESS_T*)&_end; \
	ASSERT(free_page_list_base_ != NULL); \
	free_page_list_top_ = &(free_page_list_base_[(size-1)]); \
	free_page_list_pointer_ = free_page_list_base_;
	
#define FREE_PAGE_LIST_PUSH(phyaddr); \
	ASSERT(free_page_list_pointer_ < free_page_list_top_); \
	++(free_page_list_pointer_); \
	*free_page_list_pointer_ = phyaddr;
	
#define FREE_PAGE_LIST_POP(phyaddr); \
	ASSERT(free_page_list_pointer_ > free_page_list_base_); \
	phyaddr = *free_page_list_pointer_; \
	--(free_page_list_pointer_);



/* Get a free mappable page from the free list or the swap device. */
STATUS_T
AllocatePage(
	IN ULONG_T type,
	IN ULONG_T priority,
	OUT PHYSICAL_ADDRESS_T* page
	)
{
	DISPATCH_LEVEL_T prev_level = INVALID_LEVEL;
	
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	
	RaiseDispatchLevel(SYNCH_LEVEL, &prev_level );
	
	if( free_page_list_pointer_ == free_page_list_base_ )
	{
		/* At the point we should free a page
		 * and return it if the priority 
		 * is high or higher and the type
		 * is 
		 */
		
		*page = 0x0;
		RestoreDispatchLevel( &prev_level );
			
		return STATUS_NOT_AVAIL;
	}	
	
	FREE_PAGE_LIST_POP( *page );
	
	AtomicIncrement( &GlobalCommittedPages );
	AtomicDecrement( &GlobalFreePages );
	
	RestoreDispatchLevel( &prev_level );

	return STATUS_SUCCESS;
}

/* Return a page to the free list or free swap. */
STATUS_T
FreePage(
	IN ULONG_T type,
	IN PHYSICAL_ADDRESS_T page
	)
{
	DISPATCH_LEVEL_T prev_level = INVALID_LEVEL;
	
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	
	RaiseDispatchLevel( SYNCH_LEVEL, &prev_level );
	
	ASSERT(page > (PHYSICAL_ADDRESS_T)free_page_list_top_);

	FREE_PAGE_LIST_PUSH( page );
	
	AtomicDecrement( &GlobalCommittedPages );
	AtomicIncrement( &GlobalFreePages );
	
	RestoreDispatchLevel( &prev_level );

	/* If this function fails we need to abort. */
	return STATUS_SUCCESS;
}

/* Get the page table info for a particular virtual address. */
STATUS_T
GetMemoryMapping(
	IN BYTE_T* addr,
	OUT ULONG_T* flags,
	OPTIONAL OUT PHYSICAL_ADDRESS_T* page
	)
{
	PTE* pte = NULL;
	PTE* pde = NULL;
	DISPATCH_LEVEL_T prev_level = INVALID_LEVEL;

	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	ASSERT( addr > (BYTE_T*)VIRTUAL_NULL_GUARD_TOP );
	ASSERT( addr < (BYTE_T*)VIRTUAL_XPAGE_TBLS_BASE );
	ASSERT( flags != NULL );

	RaiseDispatchLevel( SYNCH_LEVEL, &prev_level );

	pde = GET_PDE_POINTER(addr);

	if( (*pde) & MEMORY_FLAG_PRESENT )
	{			
		pte = GET_PTE_POINTER( addr );
		
		*flags = (ULONG_T)((*pte) & PTE_PAGE_FLAGS_MASK);
		
		if ( MEMORY_ALLOCATED( *flags ) )
		{
			if ( page != NULL )
				*page = (PHYSICAL_ADDRESS_T)((*pte) & PTE_PAGE_FRAME_MASK);
		}
		else
		{
			if ( page != NULL )
				*page = PHYSICAL_NULL;
		}
	}
	else
	{
		*flags = 0;
		
		if ( page != NULL )
			*page = PHYSICAL_NULL;	
	}
	
	RestoreDispatchLevel( &prev_level );
	
	return STATUS_SUCCESS;
}	

/* Arrange the page table info for a particular virtual address. */
STATUS_T
SetMemoryMapping(
	IN BYTE_T* addr,
	IN ULONG_T flags,
	OPTIONAL IN PHYSICAL_ADDRESS_T* page
	)
{
	PTE* pte = NULL;
	PTE* pde = NULL;
	DISPATCH_LEVEL_T prev_level = INVALID_LEVEL;
	
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	ASSERT( addr > (BYTE_T*)VIRTUAL_NULL_GUARD_TOP );
	ASSERT( addr < (BYTE_T*)VIRTUAL_XPAGE_TBLS_BASE );
	
	if ( MEMORY_PRESENT( flags ) && ( ! MEMORY_ALLOCATED( flags ) ) )
	{
		/* If present is specified so must allocated be. */
		ASSERT( MEMORY_ALLOCATED( flags ) );
		return STATUS_BAD_PARAM;
	}
	
	if ( MEMORY_LOCKED( flags ) && ( ! MEMORY_PRESENT( flags ) ) )
	{
		/* If lock is specified so must present be. */
		ASSERT( MEMORY_PRESENT( flags ) );
		return STATUS_BAD_PARAM;
	}
	
	RaiseDispatchLevel( SYNCH_LEVEL, &prev_level );
	
	pde = GET_PDE_POINTER( addr );

	if( !( (*pde) & MEMORY_FLAG_PRESENT ) )
	{
		PHYSICAL_ADDRESS_T page_tbl = 0x0; 
		
		if( AllocatePage( 
			ALLOC_PAGE_TYPE_NATIVE, 
			ALLOC_PAGE_HIGH, 
			&page_tbl 
			) != STATUS_SUCCESS ) 
		{
			RestoreDispatchLevel( &prev_level );
			return STATUS_NOT_AVAIL;
		}
		
		*pde = CREATE_PDE( page_tbl, PAGETABLE_MEMORY_FLAGS );
		
		ia32_invlpg_( addr );
	} 
	
	pte = GET_PTE_POINTER( addr );

	*pte = 0;

	if( MEMORY_ALLOCATED( flags ) )
	{
		ASSERT( page != NULL );
		
		*pte = ( (*page) & PTE_PAGE_FRAME_MASK );
	}
	
	*pte = (*pte + ( flags & PTE_PAGE_FLAGS_MASK ));
		
	ia32_invlpg_( addr );
	
	RestoreDispatchLevel( &prev_level );
		
	return STATUS_SUCCESS;
}

/* Zero out a page for security reasons (user-mode). */
STATUS_T
ZeroPage(
	OUT PHYSICAL_ADDRESS_T* page
	)
{
	STATUS_T status;
	DISPATCH_LEVEL_T prev_level = INVALID_LEVEL;
	extern VOID page_zeroing_buffer_;
	
	ASSERT( GetDispatchLevel() <= SYNCH_LEVEL );
	
	RaiseDispatchLevel( SYNCH_LEVEL, &prev_level );
	
	status = SetMemoryMapping(
			(BYTE_T*)&page_zeroing_buffer_,
			PAGETABLE_MEMORY_FLAGS,
			page
			);
	
	if( IS_SUCCESS( status ) )
	{		
		ZeroBytes( 
			(BYTE_T*)&page_zeroing_buffer_, 
			PAGE_SIZE
			);
			
		status = SetMemoryMapping(
				(BYTE_T*)&page_zeroing_buffer_,
				0,
				NULL
				);
	}

	RestoreDispatchLevel( &prev_level );

	return status;
}

static
BOOLEAN_T
pagefault_vector_handler_(
	IN ULONG_T vector_nr, 
	IN VOID* param, 
	IN OUT DISPATCH_FRAME_T* frame
	)
{
	BYTE_T* addr = (BYTE_T*)ia32_get_cr2_();
	
	ASSERT( GetDispatchLevel() == NORMAL_LEVEL );
	
	if (
	( addr <= (BYTE_T*)VIRTUAL_NULL_GUARD_TOP ) ||
	( addr >= (BYTE_T*)VIRTUAL_XPAGE_TBLS_BASE )
		) goto do_illegal_pagefault;
	
	DISPLAY(DDEBUG, "PAGE FAULT: %p\n", addr);
	
	/* This is the system wide page fault handler.
	 * The virtual address that access was attemped 
	 * is stored in CR2.  The frame error code is a 
	 * bit field that describes the context as 
	 * follows:
	 *
	 * BIT 2
	 * U/S	0 	The processor was executing in supervisor mode.
	 *	1	The processor was executing in user mode.
	 *
	 * BIT 1
	 * W/R	0	The access causing the fault was a read. 
	 *	1	The access causing the fault was a write.  
	 *
	 * BIT 0
	 * P	0	The fault was caused by a not-present page. 
	 * 	1	The fault was caused by a page-level protection violation. 
	 */
	
	if ( ( frame->error_code & 0x00000001 ) &&	/* Non-present page */
		( addr != NULL ) )			/* Not NULL pointer */
	{
		PHYSICAL_ADDRESS_T page = PHYSICAL_NULL;
		ULONG_T flags = 0;
		STATUS_T status;
		
		status = GetMemoryMapping(
				addr, 
				&flags,
				&page
				);
				
		if( ! IS_SUCCESS( status ) )
			goto do_illegal_pagefault;
		
		
		if( ! MEMORY_ALLOCATED( flags ) )
			goto do_illegal_pagefault;
		

		/* This is where we would write the
		 * the page in from swap and mapp
		 * it in.  However, that is disabled
		 * so we know if it is allocated
		 * there is a *real* page behind it.
		 * We just set the it to be present.
		 */
			
		/* At some point we need to change
		 * this to a call to AdaptMemory
		 */
		status = SetMemoryMapping(
				addr,
				( MEMORY_FLAG_ALLOCATED | 
				MEMORY_FLAG_WRITE_ACCESS | 
				MEMORY_FLAG_PRESENT ),
				&page
				);
			
		if ( ! IS_SUCCESS ( status ) )
			goto do_illegal_pagefault;
	
		return TRUE;
	}
	

do_illegal_pagefault:

	DISPLAY( 
		DCRITICAL,
		"\nAn illegal page fault occured while the instruction at 0x%p"
		" tried to access memory at 0x%p.\nError code: %p\n",
		frame->eip,
		addr,
		frame->error_code
		);
	
	GlobalAbortContext.cpu = 0;
	GlobalAbortContext.dispatch_level = frame->prev_level;
	GlobalAbortContext.pid = 0;
	GlobalAbortContext.cr4 = 0;
	GlobalAbortContext.cr3 = 0;
	GlobalAbortContext.cr2 = 0;
	GlobalAbortContext.cr0 = 0;
	GlobalAbortContext.gs = frame->gs;
	GlobalAbortContext.fs = frame->fs;
	GlobalAbortContext.es = frame->es;
	GlobalAbortContext.ds = frame->ds;
	GlobalAbortContext.edi = frame->edi;
	GlobalAbortContext.esi = frame->esi;
	GlobalAbortContext.ebp = frame->ebp;
	GlobalAbortContext.ebx = frame->ebx;
	GlobalAbortContext.edx = frame->edx;
	GlobalAbortContext.ecx = frame->ecx;
	GlobalAbortContext.eax = frame->eax;
	GlobalAbortContext.esp = frame->esp0;
	GlobalAbortContext.ss = frame->ss0;
	GlobalAbortContext.eip = frame->eip;
	GlobalAbortContext.cs = frame->cs;
	GlobalAbortContext.eflags = frame->eflags;
	
	AbortWithContext( NULL );
	
	return TRUE;	

}

VOID
init_paging_()
{

	extern VOID xcore_page_dir_;
	extern VOID xcore_page_table0_;
	extern VOID xcore_page_table1_;
	extern VOID xcore_page_table2_;
	extern VOID xcore_page_table3_;
	
	PTE* page_dir = (PTE*)&xcore_page_dir_;
	PTE* page_tbl0 = (PTE*)&xcore_page_table0_;
	PTE* page_tbl1 = (PTE*)&xcore_page_table1_;
	PTE* page_tbl2 = (PTE*)&xcore_page_table2_;
	PTE* page_tbl3 = (PTE*)&xcore_page_table3_;
	
	PHYSICAL_ADDRESS_T page_addr = PHYSICAL_NULL;
	
	ULONG_T page_flags = (
		MEMORY_FLAG_PRESENT |
		MEMORY_FLAG_WRITE_ACCESS |
		MEMORY_FLAG_GLOBAL |
		MEMORY_FLAG_ALLOCATED |
		MEMORY_FLAG_LOCKED
		);
	
	GlobalTotalPages = (
		PAGE_ALIGN(
			(GlobalMultibootInfo.mem_upper * 1024) + 0x100000
			) / PAGE_SIZE
			);
	
	if( GlobalTotalPages < 2048 )
	{
		DISPLAY(
		DCRITICAL, 
		"\nABORT: There is less then 8 MBs of RAM available.\n"
		"A probe reports %dk is installed.  The system requires at least 8 MBs of RAM.",
		(GlobalTotalPages * 1000)
		);
		Abort(0);
	}
		
	if( GlobalTotalPages % 2048 )
	{
		GlobalTotalPages = ( GlobalTotalPages & (~(2048-1)) );
		
		DISPLAY(
		DERROR, 
		"\nERROR: RAM increments other than 8 MBs are not supported.\n"
		"A probe reports %dk is installed.  Only %dk will be addressed by the system.\n",
		( GlobalMultibootInfo.mem_upper + 1024 ),
		( (GlobalTotalPages * PAGE_SIZE) / 1024 )
		);
	}
	
	FREE_PAGE_LIST_INIT( GlobalTotalPages );
	
	ZeroBytes( (BYTE_T*)page_dir, PAGE_SIZE );
	
	if( GlobalTotalPages == 2048 )
	{
		/* We have 8 MBs.  We identity map the bottom 4 and 
		 * make the rest available for demand paging. 
		 */
		ZeroBytes( (BYTE_T*)page_tbl0, PAGE_SIZE );
		page_dir[0] = CREATE_PDE( page_tbl0, page_flags );
		
		page_addr = 0x00001000;
		
		while( (page_addr <= 0x003FF000) )
		{
			page_tbl0[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00400000;

		while ( page_addr <= 0x004FF000 )
		{	
			FREE_PAGE_LIST_PUSH( page_addr );
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		GlobalCommittedPages = 1024;
		GlobalFreePages = 1024;
		GlobalSwapPages = GlobalFreePages;
		GlobalNonSwapPages = GlobalCommittedPages;
	}

	if( GlobalTotalPages == 4096 )
	{
		/* We have 16 MBs.  We identity map the bottom 8 and 
		 * make the rest available for demand paging. 
		 */
		ZeroBytes( (BYTE_T*)page_tbl0, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl1, PAGE_SIZE );
		page_dir[0] = CREATE_PDE( page_tbl0, page_flags );
		page_dir[1] = CREATE_PDE( page_tbl1, page_flags );
		
		page_addr = 0x00001000;
		
		while( (page_addr <= 0x003FF000) )
		{
			page_tbl0[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00400000;
		
		while( (page_addr <= 0x007FF000) )
		{
			page_tbl1[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		 
		page_addr = 0x00800000;

		while ( page_addr <= 0x00FFF000 )
		{	
			FREE_PAGE_LIST_PUSH( page_addr );
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		GlobalCommittedPages = 2048;
		GlobalFreePages = 2048;
		GlobalSwapPages = GlobalFreePages;
		GlobalNonSwapPages = GlobalCommittedPages;
	}
	
	if( GlobalTotalPages == 6144 )
	{
		/* We have 24 MBs.  We identity map the bottom 12 and 
		 * make the rest available for demand paging. 
		 */
		ZeroBytes( (BYTE_T*)page_tbl0, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl1, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl2, PAGE_SIZE );
		page_dir[0] = CREATE_PDE( page_tbl0, page_flags );
		page_dir[1] = CREATE_PDE( page_tbl1, page_flags );
		page_dir[2] = CREATE_PDE( page_tbl2, page_flags );
		
		page_addr = 0x00001000;
		
		while( (page_addr <= 0x003FF000) )
		{
			page_tbl0[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00400000;
		
		while( (page_addr <= 0x007FF000) )
		{
			page_tbl1[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00800000;
		
		while( (page_addr <= 0x00BFF000) )
		{
			page_tbl2[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00C00000;

		while ( page_addr <= 0x017FF000 )
		{	
			FREE_PAGE_LIST_PUSH( page_addr );
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		GlobalCommittedPages = 3072;
		GlobalFreePages = 3072;
		GlobalSwapPages = GlobalFreePages;
		GlobalNonSwapPages = GlobalCommittedPages;
	}

	if ( GlobalTotalPages >= 8192 )
	{
		/* We have 32+ MBs.  We identity map the bottom 16 and 
		 * make the rest available for demand paging. 
		 */
		ZeroBytes( (BYTE_T*)page_tbl0, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl1, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl2, PAGE_SIZE );
		ZeroBytes( (BYTE_T*)page_tbl3, PAGE_SIZE );
		page_dir[0] = CREATE_PDE( page_tbl0, page_flags );
		page_dir[1] = CREATE_PDE( page_tbl1, page_flags );
		page_dir[2] = CREATE_PDE( page_tbl2, page_flags );	
		page_dir[3] = CREATE_PDE( page_tbl3, page_flags );
		
		page_addr = 0x00001000;
		
		while( (page_addr <= 0x003FF000) )
		{
			page_tbl0[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00400000;
		
		while( (page_addr <= 0x007FF000) )
		{
			page_tbl1[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00800000;
		
		while( (page_addr <= 0x00BFF000) )
		{
			page_tbl2[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x00C00000;
		
		while( (page_addr <= 0x00FFF000) )
		{
			page_tbl3[(page_addr/PAGE_SIZE)] = CREATE_PTE( page_addr, page_flags );					
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		page_addr = 0x01000000;

		while ( page_addr <= ( (GlobalTotalPages * PAGE_SIZE) - PAGE_SIZE ) )
		{	
			FREE_PAGE_LIST_PUSH( page_addr );
			page_addr = (page_addr + PAGE_SIZE);
		}
		
		GlobalCommittedPages = 4096;
		GlobalFreePages = (GlobalTotalPages - GlobalCommittedPages);
		GlobalSwapPages = GlobalFreePages;
		GlobalNonSwapPages = GlobalCommittedPages;
		
	}
		
	/* We map the page directory into itself as the last
	 * page table.  This way we can see everything with
	 * out doing temporary mappings.
	 */	
	page_dir[1023] = CREATE_PDE( page_dir, page_flags );
	
	ia32_set_cr3_( (UINT32_T)page_dir );
	
	/* Setup page fault handler */
	if( RequestDispatchVector(
		IA32_PF_14, 
		pagefault_vector_handler_, 
		NULL, 
		FALSE, 
		&pf_
		) != STATUS_SUCCESS
		)
	{
		DISPLAY( 
			DCRITICAL, 
			"\nCould not acquire page fault vector.  These exceptions will not be reported.\n"
			);
		/* We should usually abort here */
			
	}
	
	/* Set the paging bit */
	ia32_set_cr0_( (ia32_get_cr0_() | 0x80000000) );

}
