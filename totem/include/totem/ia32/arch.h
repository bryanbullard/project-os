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

#ifndef _TOTEM_IA32_ARCH_H
#define _TOTEM_IA32_ARCH_H


#include <totem/base.h>



typedef signed char INT8_T;
typedef unsigned char UINT8_T;
typedef signed short int INT16_T;
typedef unsigned short int UINT16_T;
typedef signed int INT32_T;
typedef unsigned int UINT32_T;
typedef signed long long int INT64_T;
typedef unsigned long long int UINT64_T;

typedef UINT32_T ARCH_WORD_T;
typedef UINT32_T PHYSICAL_ADDRESS_T;
typedef UINT32_T IOPORT_ADDRESS_T;

#define PHYSICAL_NULL	(PHYSICAL_ADDRESS_T)0
#define IOPORT_NULL	(IOPORT_ADDRESS_T)0

#define ARCH_WORD_SIZE	sizeof(ARCH_WORD_T)
#define CACHE_LINE_SIZE 32

/* align an address to the machine word boundry (4 bytes) */
#define ARCH_WORD_ALIGN(addr) ((addr) & (~(ARCH_WORD_SIZE-1)))

/* align an address to the cacheline boundry (32 bytes) */
#define CACHE_ALIGN(addr) ((addr) & (~(CACHE_LINE_SIZE-1)))

#define IA32_LOWORD(dw)	((UINT16_T)(dw))
#define IA32_HIWORD(dw)	((UINT16_T)(((UINT32_T)(dw) >> 16) & 0xFFFF))
#define IA32_LOBYTE(w)	((UINT8_T)(w))
#define IA32_HIBYTE(w)	((UINT8_T)(((UINT16_T)(w) >> 8) & 0xFF))

/* this macros will convert a binary coded digit to 
 * a normal binary and back */
#define IA32_BCDTOBIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define IA32_BINTOBCD(val) ((val)=(((val)/10)<<4) + (val)%10)



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CPU Configuration
 */
 
/* these are arch dependand declares */
#define IA32_DESCRIPTOR_SIZE		8 /* bytes */
#define IA32_MAX_INTERRUPTS		256
#define IA32_MAX_GLOBAL_DESCRIPTORS	64

#define IA32_PRESENT_RING0_INT_GATE 	0x8E00 /* present, ring0, int gate */
#define IA32_PRESENT_RING3_INT_GATE	0xEE00 /* present, ring3, int gate */
#define IA32_PRESENT_RING0_CALL_GATE 	0x8F00 /* present, ring0, call gate */
#define IA32_PRESENT_RING3_CALL_GATE 	0xEF00 /* present, ring3, call gate */
#define IA32_PRESENT_RING0_TASK_GATE	0x8500 /* present, ring0, task gate */

#define IA32_PAGE_SIZE			0x1000

#define IA32_RESERVED_RAM_BASE		0x000A0000 /* to 0x000FFFFF hw roms and rams */
#define IA32_RESERVED_RAM_TOP		0x000FFFFF

#define IA32_EXECUTIVE_CODE_SEL 	0x0008
#define IA32_EXECUTIVE_DATA_SEL 	0x0010
#define IA32_USER_CODE_SEL 		0x0018
#define IA32_USER_DATA_SEL 		0x0020
#define IA32_RESERVED_SEL 		0x0028
#define IA32_MAIN_TSS_SEL	 	0x0030
#define IA32_ABORT_TSS_SEL 		0x0038


/* Exception types/vectors into the IDT */
#define IA32_DE_00		0 /* divide error */
#define IA32_DB_01		1 /* debug */
#define IA32_NMI_02		2 /* non-maskable interrupt */
#define IA32_BP_03		3 /* breakpoint */
#define IA32_OF_04		4 /* overflow */
#define IA32_BR_05		5 /* bound range exceeded */
#define IA32_UD_06		6 /* Undefined opcode */
#define IA32_NM_07		7 /* no math coprocessor */
#define IA32_DF_08		8 /* double fault */
#define IA32_CSO_09		9 /* coprocessor segment overrun */
#define IA32_TS_10		10 /* invalid tss */
#define IA32_NP_11		11 /* segment not present */
#define IA32_SS_12		12 /* stack-segment fault */
#define IA32_GP_13		13 /* general protection fault */
#define IA32_PF_14		14 /* page fault */
#define IA32_RESERVED_15	15 /* reserved by intel for future use */
#define IA32_MF_16		16 /* math fault */
#define IA32_AC_17		17 /* alignment check */
#define IA32_MC_18		18 /* machine check */
#define IA32_XF_19		19 /* simd floating point exception */

#define IA32_RESERVED_20	20 /* reserved by intel for future use */
#define IA32_RESERVED_21	21 /* reserved by intel for future use */
#define IA32_RESERVED_22	22 /* reserved by intel for future use */
#define IA32_RESERVED_23	23 /* reserved by intel for future use */
#define IA32_RESERVED_24	24 /* reserved by intel for future use */
#define IA32_RESERVED_25	25 /* reserved by intel for future use */
#define IA32_RESERVED_26	26 /* reserved by intel for future use */
#define IA32_RESERVED_27	27 /* reserved by intel for future use */
#define IA32_RESERVED_28	28 /* reserved by intel for future use */
#define IA32_RESERVED_29	29 /* reserved by intel for future use */
#define IA32_RESERVED_30	30 /* reserved by intel for future use */
#define IA32_RESERVED_31	31 /* reserved by intel for future use */

#define IA32_IRQ_00		32
#define IA32_IRQ_01		33
#define IA32_IRQ_02		34
#define IA32_IRQ_03		35
#define IA32_IRQ_04		36
#define IA32_IRQ_05		37
#define IA32_IRQ_06		38
#define IA32_IRQ_07		39
#define IA32_IRQ_08		40
#define IA32_IRQ_09		41
#define IA32_IRQ_10		42
#define IA32_IRQ_11		43
#define IA32_IRQ_12		44
#define IA32_IRQ_13		45
#define IA32_IRQ_14		46
#define IA32_IRQ_15		47

#define SYSCALL_VECTOR_00	48
#define SYSCALL_VECTOR_01	49
#define SYSCALL_VECTOR_02	50

#define INVALID_IRQ		0xFFFFFFFF


/* this is how we're representing a x86 non-gate descirptor
 * residing in the gdt.
 */ 
typedef struct
{
	UINT32_T	low;
	UINT32_T	high;
} DECLARATOR_PACKED IA32_DESCRIPTOR_T;

/* this is how we're representing a x86 gate descirptor
 * residing in the gdt.
 */ 
typedef struct
{
	UINT16_T	low_offset;
	UINT16_T	selector;
	UINT16_T	attributes;
	UINT16_T	high_offset;
} DECLARATOR_PACKED IA32_GATE_DESCRIPTOR_T;

/* this is how we're representing a x86 task state segment
 * residing in memory.
 */
typedef struct
{
	UINT32_T	backlink;	/* high word is reserved */
	UINT32_T	esp0;
	UINT32_T	ss0;		/* high word is reserved */
	UINT32_T	esp1;
	UINT32_T	ss1;		/* high word is reserved */
	UINT32_T	esp2;
	UINT32_T	ss2;		/* high word is reserved */
	UINT32_T	cr3;
	UINT32_T	eip;
	UINT32_T	eflags;
	UINT32_T	eax;
	UINT32_T	ecx;
	UINT32_T	edx;
	UINT32_T	ebx;
	UINT32_T	esp;
	UINT32_T	ebp;
	UINT32_T	esi;
	UINT32_T	edi;
	UINT32_T	es;		/* high word is reserved */
	UINT32_T	cs;		/* high word is reserved */
	UINT32_T	ss;		/* high word is reserved */
	UINT32_T	ds;		/* high word is reserved */
	UINT32_T	fs;		/* high word is reserved */
	UINT32_T	gs;		/* high word is reserved */
	UINT32_T	ldt;
	UINT32_T	iomap;		/* 15:1 is reserved */
} DECLARATOR_PACKED IA32_TSS_T;

/* helpers */
UINT32_T ia32_get_frame_pointer_( VOID );
VOID ia32_invlpg_( VOID* p );
VOID ia32_lgdt_( VOID* p );
VOID ia32_lidt_( VOID* p );
VOID ia32_ltr_( UINT16_T v );
VOID ia32_hlt_( VOID );
VOID ia32_set_cr3_( UINT32_T v );
UINT32_T ia32_get_cr3_( VOID );
UINT32_T ia32_get_cr2_( VOID );
VOID ia32_set_cr0_( UINT32_T v );
UINT32_T ia32_get_cr0_( VOID );
UINT32_T ia32_get_eflags_( VOID );
UINT32_T ia32_get_eflags_and_cli_( VOID );
UINT32_T ia32_set_eflags_( UINT32_T v );
VOID ia32_cli_( VOID );
VOID ia32_sti_( VOID );
VOID ia32_issue_master_pic_eoi_( VOID );
VOID ia32_issue_slave_pic_eoi_( VOID );



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * programable interrupt controller 
 */
/* pic ports */
#define PIC_MASTER_CONTROL_PORT	0x20
#define PIC_MASTER_DATA_PORT	0x21
#define PIC_SLAVE_CONTROL_PORT	0xA0
#define PIC_SLAVE_DATA_PORT	0xA1
#define PIC_END_OF_INTERRUPT	0x20
/* pic initialization control words */
#define PIC_ICW1		0x11
#define PIC_ICW4		0x01 /* x86 chip w/o Auto EOI */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Programable interval timer 
 */
#define PIT_CONTROL_PORT	0x43
#define PIT_TIME_OF_DAY_REG	0x40
#define PIT_ADJUST_COUNTER	0x36

#define CLOCK_TICK_HZ		100
#define PIT_ORIGINAL_HZ		1193180 /* clock runs at 1.19318 MHz */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Processor Data 
 */

/* Don't change this without changing assembler code */

typedef struct
{
	DISPATCH_LEVEL_T	current_level;
	PROCESS_T*		current_process;
	ULONG_T			number;
#ifndef TOTEM_NSTATS
	ULONG_T			process_switches;
#endif	
	IA32_TSS_T		tss;
	
} DECLARATOR_PACKED PROCESSOR_T;


 
#endif /* _TOTEM_IA32_ARCH_H */


