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
#include <totem/dispatch.h>
#include <totem/display.h>
#include <totem/portio.h>
#include <totem/status.h>

#include <totem/arch.h>


extern void ia32_divide_error_entry_();
extern void ia32_debugger_entry_();
extern void ia32_breakpoint_entry_();
extern void ia32_invalid_opcode_entry_();
extern void ia32_general_protection_fault_entry_();
extern void ia32_page_fault_entry_();
extern void ia32_irq00_entry_();
extern void ia32_irq01_entry_();
extern void ia32_irq02_entry_();
extern void ia32_irq03_entry_();
extern void ia32_irq04_entry_();
extern void ia32_irq05_entry_();
extern void ia32_irq06_entry_();
extern void ia32_irq07_entry_();
extern void ia32_irq08_entry_();
extern void ia32_irq09_entry_();
extern void ia32_irq10_entry_();
extern void ia32_irq11_entry_();
extern void ia32_irq12_entry_();
extern void ia32_irq13_entry_();
extern void ia32_irq14_entry_();
extern void ia32_irq15_entry_();
extern void ia32_syscall00_entry_();
extern void ia32_syscall01_entry_();
extern void ia32_syscall02_entry_();


volatile 
IA32_GATE_DESCRIPTOR_T GlobalIDT[IA32_MAX_INTERRUPTS] DECLARATOR_ALIGN(8);

volatile 
IA32_DESCRIPTOR_T GlobalGDT[IA32_MAX_GLOBAL_DESCRIPTORS] DECLARATOR_ALIGN(8);

volatile 
IA32_TSS_T GlobalMainTSS DECLARATOR_ALIGN(8);

volatile 
IA32_TSS_T GlobalAbortTSS DECLARATOR_ALIGN(8);



extern 
volatile 
void initial_stack_top_;

extern 
volatile 
void xcore_page_dir_;

#define GDT GlobalGDT
#define IDT GlobalIDT
#define MainTSS GlobalMainTSS
#define AbortTSS GlobalAbortTSS



/* the software aborts, non-maskable interrupt, and 
 * double fault exections are handled out-of-task
 * by means of a hardware task switch with a new
 * stack. thus, no stack is past to the handler.
 * these handlers may examine the faulting task's
 * state by the main tss (where the faulting
 * task's state was saved during task switch.
 */
VOID
temp_abort_handler_(VOID)
{	
	
	DISPLAY( DCRITICAL, "ABORT: A hardware abort was signaled at address: 0x%p\n\n", MainTSS.eip);

	DISPLAY( DCRITICAL, "eax: %p\tebx: %p\tecx: %p\tedx: %p\n", 
							MainTSS.eax, 
							MainTSS.ebx, 
							MainTSS.ecx, 
							MainTSS.edx);
	DISPLAY( DCRITICAL, "esp: %p\tebp: %p\tesi: %p\tedi: %p\n", 
							MainTSS.esp, 
							MainTSS.ebp, 
							MainTSS.esi, 
							MainTSS.edi);
	DISPLAY( DCRITICAL, " cs: %p\t ds: %p\t es: %p\t ss: %p\n", 
							MainTSS.cs, 
							MainTSS.ds, 
							MainTSS.es, 
							MainTSS.ss);
	DISPLAY( DCRITICAL, "eip: %p\tefg: %p\tcr3: %p\tblk: %p\n", 	
							MainTSS.eip, 
							MainTSS.eflags, 
							MainTSS.cr3, 
							MainTSS.backlink);
	
	ia32_cli_();
	ia32_hlt_();
}




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Exported to executive
 */



/* calling this funtion will cause the CPU 
 * to reset.
 */

VOID
reset_processors_()
{
	/* what we want to do here is
	 * zero out the page directory
	 * then invalidate the cache.
	 */
}



/* this function should be called once and 
 * only once during early initialization.
 * in this implmentation we are setting up
 * a x86 global descriptor table and 
 * interrupt descriptor table.  also, we are
 * resetting the programmable interrupt
 * controller and programmable interval
 * timer.
 * note that interrupts are expected to be 
 * disable before this function is called.
 * however, just in case, they are disabled.
 *
 */

VOID
init_processors_()
{
	UINT64_T gdtr = 0;

	UINT64_T idtr = 0;
	
	UINT32_T eflags = ia32_get_eflags_and_cli_();	/* prevent any maskable interrupt from firing */

	/* set up TSSs */
	ZeroBytes((BYTE_T*)&MainTSS, sizeof(IA32_TSS_T));
	
	ZeroBytes((VOID*)&AbortTSS, sizeof(IA32_TSS_T));
	AbortTSS.esp0 = ((UINT32_T)&initial_stack_top_);
	AbortTSS.ss0 = IA32_EXECUTIVE_DATA_SEL;
	AbortTSS.cr3 = (UINT32_T)&xcore_page_dir_;
	AbortTSS.eip = (UINT32_T)temp_abort_handler_;
	AbortTSS.esp = ((UINT32_T)&initial_stack_top_);
	AbortTSS.es = IA32_EXECUTIVE_DATA_SEL;
	AbortTSS.cs = IA32_EXECUTIVE_CODE_SEL;
	AbortTSS.ss = IA32_EXECUTIVE_DATA_SEL;
	AbortTSS.ds = IA32_EXECUTIVE_DATA_SEL;
	AbortTSS.iomap = 104;		/* limit + 1 (not using this) */
	AbortTSS.eflags = 0x2;	/* IOPL=0 IF=0 */
	
	ZeroBytes((BYTE_T*)GDT, sizeof(GDT));



	/* the first descriptor is ignored
	 * by the processor.
	 */
	GDT[0].low = 0x0;
	GDT[0].high = 0x0;

	/* system code segment */
	GDT[1].low = 0x0000ffff;
	GDT[1].high = 0x00cf9a00;

	/* system data segment */
	GDT[2].low = 0x0000ffff;
	GDT[2].high = 0x00cf9200;

	/* user code segment */
	GDT[3].low = 0x0000ffff;
	GDT[3].high = 0x00cffa00;

	/* user data segment */
	GDT[4].low = 0x0000ffff;
	GDT[4].high = 0x00cff200;

	/* reserved */
	GDT[5].low = 0x0;
	GDT[5].high = 0x0;
	
	/* main tss */
	/* limit is hard wired at 0x0067 (103) */
	GDT[6].low = (((UINT32_T)(&MainTSS) << 16) | 0x0067);
	/* PRESENT=1 DPL=0 GRAN=1 BUSY=0 */
	GDT[6].high = (((UINT32_T)(&MainTSS) & 0xff000000) | 
					0x00808900 | 
					(((UINT32_T)(&MainTSS) >> 16) & 
					0x000000ff));
	
	/* abort tss */
	/* limit is hard wired at 0x0067 (103) */
	GDT[7].low = (((UINT32_T)(&AbortTSS) << 16) | 0x0067);
	/* PRESENT=1 DPL=0 GRAN=1 BUSY=0 */
	GDT[7].high = (((UINT32_T)(&AbortTSS) & 0xff000000) | 
					0x00808900 | 
					(((UINT32_T)(&AbortTSS) >> 16) & 
					0x000000ff));
	
	gdtr = (UINT64_T)GDT;

	gdtr = (gdtr << 16);
	gdtr = (gdtr | (sizeof(GDT) - 1));
	
	/* load gdt */
	ia32_lgdt_(&gdtr);

	/* load task register with main tss selector */
	ia32_ltr_((UINT16_T)IA32_MAIN_TSS_SEL);
	
	
	/* initialize interrupts *********************************/

	ZeroBytes((BYTE_T*)IDT, sizeof(IDT));
	
	
	IDT[IA32_DE_00].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_DE_00].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_DE_00].low_offset = IA32_LOWORD(ia32_divide_error_entry_);
	IDT[IA32_DE_00].high_offset = IA32_HIWORD(ia32_divide_error_entry_);						 
	
	IDT[IA32_DB_01].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_DB_01].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_DB_01].low_offset = IA32_LOWORD(ia32_debugger_entry_);
	IDT[IA32_DB_01].high_offset = IA32_HIWORD(ia32_debugger_entry_);
	
	IDT[IA32_BP_03].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_BP_03].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_BP_03].low_offset = IA32_LOWORD(ia32_breakpoint_entry_);
	IDT[IA32_BP_03].high_offset = IA32_HIWORD(ia32_breakpoint_entry_);
	
	IDT[IA32_UD_06].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_UD_06].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_UD_06].low_offset = IA32_LOWORD(ia32_invalid_opcode_entry_);
	IDT[IA32_UD_06].high_offset = IA32_HIWORD(ia32_invalid_opcode_entry_);						 
	
	IDT[IA32_DF_08].selector = IA32_ABORT_TSS_SEL;
	IDT[IA32_DF_08].attributes = (UINT16_T)IA32_PRESENT_RING0_TASK_GATE;
	IDT[IA32_DF_08].low_offset = IA32_LOWORD(0);
	IDT[IA32_DF_08].high_offset = IA32_HIWORD(0);
	
	IDT[IA32_GP_13].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_GP_13].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_GP_13].low_offset = IA32_LOWORD(ia32_general_protection_fault_entry_);
	IDT[IA32_GP_13].high_offset = IA32_HIWORD(ia32_general_protection_fault_entry_);						 
	
	IDT[IA32_PF_14].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_PF_14].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_PF_14].low_offset = IA32_LOWORD(ia32_page_fault_entry_);
	IDT[IA32_PF_14].high_offset = IA32_HIWORD(ia32_page_fault_entry_);						 
	
	IDT[IA32_MC_18].selector = IA32_ABORT_TSS_SEL;
	IDT[IA32_MC_18].attributes = (UINT16_T)IA32_PRESENT_RING0_TASK_GATE;
	IDT[IA32_MC_18].low_offset = IA32_LOWORD(0);
	IDT[IA32_MC_18].high_offset = IA32_HIWORD(0);
	
	IDT[IA32_IRQ_00].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_IRQ_00].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_IRQ_00].low_offset = IA32_LOWORD(ia32_irq00_entry_);
	IDT[IA32_IRQ_00].high_offset = IA32_HIWORD(ia32_irq00_entry_);						 
	
	IDT[IA32_IRQ_01].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[IA32_IRQ_01].attributes = (UINT16_T)IA32_PRESENT_RING0_INT_GATE;
	IDT[IA32_IRQ_01].low_offset = IA32_LOWORD(ia32_irq01_entry_);
	IDT[IA32_IRQ_01].high_offset = IA32_HIWORD(ia32_irq01_entry_);						 
	
	IDT[SYSCALL_VECTOR_00].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[SYSCALL_VECTOR_00].attributes = (UINT16_T)IA32_PRESENT_RING3_INT_GATE;
	IDT[SYSCALL_VECTOR_00].low_offset = IA32_LOWORD(ia32_syscall00_entry_);
	IDT[SYSCALL_VECTOR_00].high_offset = IA32_HIWORD(ia32_syscall00_entry_);
	
	IDT[SYSCALL_VECTOR_01].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[SYSCALL_VECTOR_01].attributes = (UINT16_T)IA32_PRESENT_RING3_INT_GATE;
	IDT[SYSCALL_VECTOR_01].low_offset = IA32_LOWORD(ia32_syscall01_entry_);
	IDT[SYSCALL_VECTOR_01].high_offset = IA32_HIWORD(ia32_syscall01_entry_);

	IDT[SYSCALL_VECTOR_02].selector = IA32_EXECUTIVE_CODE_SEL;
	IDT[SYSCALL_VECTOR_02].attributes = (UINT16_T)IA32_PRESENT_RING3_INT_GATE;
	IDT[SYSCALL_VECTOR_02].low_offset = IA32_LOWORD(ia32_syscall02_entry_);
	IDT[SYSCALL_VECTOR_02].high_offset = IA32_HIWORD(ia32_syscall02_entry_);								 
	

	idtr = (UINT64_T)IDT;

	idtr = (idtr << 16);
	idtr = (idtr | (sizeof(IDT) - 1));
	
	ia32_lidt_(&idtr);
	
	/*
	 * this is where we re-program the progammable interupt controller.
	 * we are remapping the irqs to interrupts 32 thru 47 so there
	 * is no confusion with intel's exceptions
	 */
	/* reset the 8259 chip */
	WriteByteToPort(PIC_END_OF_INTERRUPT, PIC_MASTER_CONTROL_PORT);
	/* send init control word 1 */
	WriteByteToPort(PIC_ICW1, PIC_MASTER_CONTROL_PORT);	
	WriteByteToPort(PIC_ICW1, PIC_SLAVE_CONTROL_PORT);
	/* send init control word 2 */
	WriteByteToPort(32, PIC_MASTER_DATA_PORT);
	WriteByteToPort(40, PIC_SLAVE_DATA_PORT);
	/* send init control word 3 */
	WriteByteToPort(0x04, PIC_MASTER_DATA_PORT);
	WriteByteToPort(0x02, PIC_SLAVE_DATA_PORT);
	/* send init control word 4 */
	WriteByteToPort(PIC_ICW4, PIC_MASTER_DATA_PORT); /* note: no auto EOI */
	WriteByteToPort(PIC_ICW4, PIC_SLAVE_DATA_PORT);

	/* initialize state with all irqs disabled */
	WriteByteToPort(0xff, PIC_MASTER_DATA_PORT);
	WriteByteToPort(0xff, PIC_SLAVE_DATA_PORT);
	
	{
		/* adjust the programmable interval timer to signal 
		 * irq0 once every 10 milliseconds.
		 * example: clock runs at 1.19318 MHz, so 1,193,180Hz/1000 seconds = 1193
		 * 1193 = 0x04A9 
		 */
		UINT16_T adjust = (PIT_ORIGINAL_HZ/CLOCK_TICK_HZ);
		 
		WriteByteToPort(CLOCK_TICK_HZ, PIT_CONTROL_PORT);
		WriteByteToPort(IA32_LOBYTE(adjust),PIT_TIME_OF_DAY_REG); /* LSB of divisor */
		WriteByteToPort(IA32_HIBYTE(adjust),PIT_TIME_OF_DAY_REG); /* MSB of divisor */
	
	}

	/*
	 * Setup vector mappings
	 */
	 
	SET_VECTOR_MAPPING(IA32_DE_00,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_DB_01,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_NMI_02,PROCESSOR_SYNCH_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_BP_03,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_OF_04,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_BR_05,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_UD_06,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_NM_07,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_DF_08,ATOMIC_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_LOCKED);
	SET_VECTOR_MAPPING(IA32_CSO_09,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_TS_10,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_NP_11,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_SS_12,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_GP_13,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_PF_14,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_MF_16,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_AC_17,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_MC_18,ATOMIC_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_LOCKED);
	SET_VECTOR_MAPPING(IA32_XF_19,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_00,DEVICE_SIGNAL_LEVEL_14,0,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_01,DEVICE_SIGNAL_LEVEL_13,1,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_02,INVALID_LEVEL,2,VECTOR_MAP_SHARE_LOCKED);
	SET_VECTOR_MAPPING(IA32_IRQ_03,DEVICE_SIGNAL_LEVEL_04,3,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_04,DEVICE_SIGNAL_LEVEL_03,4,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_05,DEVICE_SIGNAL_LEVEL_02,5,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_06,DEVICE_SIGNAL_LEVEL_01,6,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_07,DEVICE_SIGNAL_LEVEL_00,7,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_08,DEVICE_SIGNAL_LEVEL_12,8,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_09,DEVICE_SIGNAL_LEVEL_11,9,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_10,DEVICE_SIGNAL_LEVEL_10,10,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_11,DEVICE_SIGNAL_LEVEL_09,11,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_12,DEVICE_SIGNAL_LEVEL_08,12,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_13,DEVICE_SIGNAL_LEVEL_07,13,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_14,DEVICE_SIGNAL_LEVEL_06,14,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(IA32_IRQ_15,DEVICE_SIGNAL_LEVEL_05,15,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(SYSCALL_VECTOR_00,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	SET_VECTOR_MAPPING(SYSCALL_VECTOR_01,TIMER_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_LOCKED);
	SET_VECTOR_MAPPING(SYSCALL_VECTOR_02,INVALID_LEVEL,INVALID_IRQ,VECTOR_MAP_SHARE_AVAIL);
	
	ia32_set_eflags_( eflags );
}

#ifndef NDEBUG

VOID
dump_vector_mapping_(
	IN ULONG_T vector_nr
	)
{
	if ( vector_nr >= NR_SYSTEM_VECTORS )
	{	
		DISPLAY(
			DDEBUG,
			"\nInvalid vector range %d.\n",
			vector_nr
			);
		return;
	}
	
	DLINK_NODE_T* pnode = GlobalVectorMap[vector_nr].vector_list.head;
	DISPATCH_VECTOR_T* pvector = NULL;
	ULONG_T vector_count = 0;
	
	DISPLAY(
		DDEBUG,
		"\nVector %d: LEVEL:%x IRQ:%x SHARE:%x\nNodes:\n",
		vector_nr,
		GlobalVectorMap[vector_nr].level,
		GlobalVectorMap[vector_nr].irq,
		GlobalVectorMap[vector_nr].sharing
		);
	
	while (pnode != NULL)
	{
		pvector = pnode->data;
		
		DISPLAY(
			DDEBUG,
			"Handler:%p Param:%p Vector:%x, NextNode:%p PrevNode:%p DataNode:%x\n",
			pvector->handler,
			pvector->param,
			pvector->vector_nr,
			pvector->node.next,
			pvector->node.prev,
			pvector->node.data
			);
		
		pnode = pnode->next;
		++vector_count;
	}
}

#endif
