;;
;
;   Copyright (c) 2003, 2006 Bryan Bullard.  All rights reserved.
;
; Permission is hereby granted, free of charge, to any person 
; obtaining a copy of this software and associated documentation 
; files (the "Software"), to deal in the Software without 
; restriction, including without limitation the rights to use, 
; copy, modify, merge, publish, distribute, sublicense, and/or 
; sell copies of the Software, and to permit persons to whom 
; the Software is furnished to do so, subject to the following 
; conditions:
;
;   The above copyright notice and this permission notice 
;   shall be included in all copies or substantial portions 
;   of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
; OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
; WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
; OTHER DEALINGS IN THE SOFTWARE.
;
;;

[bits 32]

[section .text]

[extern dispatch_]
[extern GlobalClockTickCounter]
[extern GlobalCurrentDispatchLevel]
[extern GlobalCurrentProcessPointer]
[extern GlobalProcessorInfoTable]
[extern schedule_]

;	dispatch levels
%define ATOMIC_LEVEL		19
%define PROCESSOR_SYNCH_LEVEL	18
%define TIMER_LEVEL		17
%define DEVICE_SIGNAL_LEVEL_14	17	; IRQ00
%define DEVICE_SIGNAL_LEVEL_13	16	; IRQ01
%define DEVICE_SIGNAL_LEVEL_12	15	; IRQ08 or IRQ02
%define DEVICE_SIGNAL_LEVEL_11	14	; IRQ09
%define DEVICE_SIGNAL_LEVEL_10	13	; IRQ10
%define DEVICE_SIGNAL_LEVEL_09	12	; IRQ11
%define DEVICE_SIGNAL_LEVEL_08	11	; IRQ12
%define DEVICE_SIGNAL_LEVEL_07	10	; IRQ13
%define DEVICE_SIGNAL_LEVEL_06	9	; IRQ14
%define DEVICE_SIGNAL_LEVEL_05	8	; IRQ15
%define DEVICE_SIGNAL_LEVEL_04	7	; IRQ03
%define DEVICE_SIGNAL_LEVEL_03	6	; IRQ04
%define DEVICE_SIGNAL_LEVEL_02	5	; IRQ05
%define DEVICE_SIGNAL_LEVEL_01	4	; IRQ06
%define DEVICE_SIGNAL_LEVEL_00	3	; IRQ07
%define SYNCH_LEVEL		2
%define NORMAL_LEVEL		1
%define INVALID_LEVEL		0

;	executive data selector
;
%define EXEC_DATA_SEL 0x10

;	ia-32 vectors reserved for execeptions
;
%define IA32_DE_00		0 ; divide error
%define IA32_DB_01		1 ; debug
%define IA32_NMI_02		2 ; non-maskable interrupt
%define IA32_BP_03		3 ; breakpoint
%define IA32_OF_04		4 ; overflow
%define IA32_BR_05		5 ; bound range exceeded
%define IA32_UD_06		6 ; Undefined opcode
%define IA32_NM_07		7 ; no math coprocessor
%define IA32_DF_08		8 ; double fault
%define IA32_CSO_09		9 ; coprocessor segment overrun
%define IA32_TS_10		10 ; invalid tss
%define IA32_NP_11		11 ; segment not present
%define IA32_SS_12		12 ; stack-segment fault
%define IA32_GP_13		13 ; general protection fault
%define IA32_PF_14		14 ; page fault
%define IA32_RESERVED_15	15 ; reserved by Intel for future use
%define IA32_MF_16		16 ; math fault
%define IA32_AC_17		17 ; alignment check
%define IA32_MC_18		18 ; machine check
%define IA32_XF_19		19 ; simd floating point exception
%define IA32_RESERVED_20	20 ; reserved by Intel for future use
%define IA32_RESERVED_21	21 ; reserved by Intel for future use
%define IA32_RESERVED_22	22 ; reserved by Intel for future use
%define IA32_RESERVED_23	23 ; reserved by Intel for future use
%define IA32_RESERVED_24	24 ; reserved by Intel for future use
%define IA32_RESERVED_25	25 ; reserved by Intel for future use
%define IA32_RESERVED_26	26 ; reserved by Intel for future use
%define IA32_RESERVED_27	27 ; reserved by Intel for future use
%define IA32_RESERVED_28	28 ; reserved by Intel for future use
%define IA32_RESERVED_29	29 ; reserved by Intel for future use
%define IA32_RESERVED_30	30 ; reserved by Intel for future use
%define IA32_RESERVED_31	31 ; reserved by Intel for future use

;	standard pc irq numbers
;
%define IA32_IRQ_00		32
%define IA32_IRQ_01		33
%define IA32_IRQ_02		34
%define IA32_IRQ_03		35
%define IA32_IRQ_04		36
%define IA32_IRQ_05		37
%define IA32_IRQ_06		38
%define IA32_IRQ_07		39
%define IA32_IRQ_08		40
%define IA32_IRQ_09		41
%define IA32_IRQ_10		42
%define IA32_IRQ_11		43
%define IA32_IRQ_12		44
%define IA32_IRQ_13		45
%define IA32_IRQ_14		46
%define IA32_IRQ_15		47

;	system defined
;
%define SYSCALL_VECTOR_00	48	; general system calls
%define SYSCALL_VECTOR_01	49	; get tick count
%define SYSCALL_VECTOR_02	50	; reserved
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Exported Functions
;;

align 4

;	FASTCALL DISPATCH_LEVEL_T GetDispatchLevel( VOID )
;
[global GetDispatchLevel]
GetDispatchLevel:
	mov	eax, dword [GlobalProcessorInfoTable]	; Assumes uni-processor
	ret
	
align 4

;	FASTCALL DISPATCH_LEVEL_T SetDispatchLevel( DISPATCH_LEVEL_T )
;
[global SetDispatchLevel]
SetDispatchLevel:
	push	ebp
	mov	ebp, esp
	
	mov	eax, 0
	
	mov	ax, 0xffff		; Technically we should disable NMI here
	cmp	dword [ebp+8], ATOMIC_LEVEL
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xffff		; Technically we should disable NMI here
	cmp	dword [ebp+8], PROCESSOR_SYNCH_LEVEL
	jz	near SetDispatchLevel_L1

	mov	ax, 0xffff	
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_14
	jz	near SetDispatchLevel_L1

	mov	ax, 0xfffe
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_13
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xfff8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_12
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xfef8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_11
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xfcf8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_10
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xf8f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_09
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xf0f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_08
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xe0f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_07
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0xc0f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_06
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x80f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_05
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x00f8
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_04
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x00f0
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_03
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x00e0
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_02
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x00c0
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_01
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x0080
	cmp	dword [ebp+8], DEVICE_SIGNAL_LEVEL_00
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x0000
	cmp	dword [ebp+8], SYNCH_LEVEL
	jz	near SetDispatchLevel_L1
	
	mov	ax, 0x0000
	cmp	dword [ebp+8], NORMAL_LEVEL
	jz	near SetDispatchLevel_L1
	
	mov	eax, 0		; Abort
	int3
	
SetDispatchLevel_L1:
	pushf
	cli
	out	0x21, al
	shr	ax, 8
	out	0xA1, al
	mov	eax, dword [ebp+8]
	mov	dword [GlobalProcessorInfoTable+0], eax	; Assumes uni-processor
	popf
	
	pop	ebp
	
	ret	
	
;;
;;	Exported End
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 4

entry_:
	
	; Save processor state
	
	; Save ring0 ss:esp in case RPL = DPL
	; NOTE: this field will be only valid if RPL = DPL.
	push	ss
	push	esp
	add	dword [esp], 28
	
	; Save general registers
	push	eax
	push	ecx
	push	edx
	push	ebx
	push	ebp
	push	esi
	push	edi
	
	push	ds
	push	es
	push	fs
	push	gs
	
	; Update data selector
	mov	eax, EXEC_DATA_SEL
	mov	ds, eax
	mov	es, eax

	mov	eax, dword [GlobalProcessorInfoTable+0]	; Assumes uni-processor

	; If we are transitioning from NORMAL_LEVEL to TIMER_LEVEL, 
	; save *current* ESP to current process struct.
	cmp	eax, NORMAL_LEVEL
	jnz	skip_saving_process_stack_pointer_
	cmp	dword [esp+52], TIMER_LEVEL
	jnz	skip_saving_process_stack_pointer_
	mov	ebp, [GlobalCurrentProcessPointer]
	mov	[ebp+28], esp	; Save the stack pointer to the process struct
skip_saving_process_stack_pointer_:

	; Push current dispatch level
	push	eax

	; Change the current dispatch level if
	; the new requested dispatch level is valid.
	mov	eax, [esp+56]
	cmp	eax, INVALID_LEVEL
	jz	skip_dispatch_level_change_
	mov	dword [GlobalProcessorInfoTable], eax	; Assumes uni-processor
skip_dispatch_level_change_:

	; Enable interrupts on the current processor
	sti

	; Clear direction flag.
	cld
	
	; Run the dispatcher logic.
	call	dispatch_

	; Disable interrupts on the current processor
	cli
	
	; Determine if we need to send EOI to the master PIC.
	mov	eax, dword [GlobalProcessorInfoTable]
	cmp	eax, DEVICE_SIGNAL_LEVEL_00
	jb	skip_pic_eoi_
	cmp	eax, DEVICE_SIGNAL_LEVEL_14
	ja	skip_pic_eoi_
	mov	eax, 0x20
	out	0x20, al
	; If we get here determine if we need to send EOI to 
	; slave PIC also.
	cmp	eax, DEVICE_SIGNAL_LEVEL_05
	jb	skip_pic_eoi_
	cmp	eax, DEVICE_SIGNAL_LEVEL_12
	ja	skip_pic_eoi_
	out	0xa0, al
skip_pic_eoi_:
	
	; Restore saved dispatch level
	pop	eax
	mov	dword [GlobalProcessorInfoTable], eax	; Assumes uni-processor

	; If we are now transitioning to NORMAL_LEVEL from TIMER_LEVEL,
	; then update ESP.
	cmp	eax, NORMAL_LEVEL
	jnz	skip_restore_process_stack_pointer_
	cmp	dword [esp+52], TIMER_LEVEL
	jnz	skip_restore_process_stack_pointer_
	mov	ebp, [GlobalCurrentProcessPointer]
	mov	esp, [ebp+28]	; Load the stack pointer from the current process struct
skip_restore_process_stack_pointer_:

	; Restore processor state
	pop	gs
	pop	fs
	pop	es
	pop	ds

	pop	edi
	pop	esi
	pop	ebp
	pop	ebx
	pop	edx
	pop	ecx
	pop	eax
	
	; Remove ring0 ss:esp, new interrupt level, vector #, 
	; and error code.
	add	esp, 20

	iret




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Entry points for all the signal vectors on the system.
;;
;;
align 4

[global ia32_divide_error_entry_]
ia32_divide_error_entry_:
	push	0x0	; push fake error code for divide error
	push	IA32_DE_00
	push	INVALID_LEVEL
	jmp	entry_
	
align 4
	
[global ia32_debugger_entry_]
ia32_debugger_entry_:
	push	0x0	; push fake error code for debugger entry
	push	IA32_DB_01
	push	INVALID_LEVEL
	jmp	entry_
	
align 4	
	
[global ia32_breakpoint_entry_]
ia32_breakpoint_entry_:
	push	0x0	; push fake error code for breakpoint
	push	IA32_BP_03
	push	INVALID_LEVEL
	jmp	entry_

align 4

[global ia32_invalid_opcode_entry_]
ia32_invalid_opcode_entry_:
	push	0x0	; push fake error code for invalid opcode
	push	IA32_UD_06
	push	INVALID_LEVEL
	jmp	entry_

align 4

[global ia32_general_protection_fault_entry_]
ia32_general_protection_fault_entry_:
	push	IA32_GP_13
	push	INVALID_LEVEL
	jmp	entry_

align 4

[global ia32_page_fault_entry_]
ia32_page_fault_entry_:
	push	IA32_PF_14
	push	INVALID_LEVEL
	jmp	entry_

align 4

[global ia32_irq00_entry_]
ia32_irq00_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_00
	push	DEVICE_SIGNAL_LEVEL_14
	jmp	entry_

align 4

[global ia32_irq01_entry_]
ia32_irq01_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_01
	push	DEVICE_SIGNAL_LEVEL_13
	jmp	entry_

align 4

; should probably remove this one.
[global ia32_irq02_entry_]
ia32_irq02_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_02, 
	push	DEVICE_SIGNAL_LEVEL_12
	jmp	entry_

align 4

[global ia32_irq03_entry_]
ia32_irq03_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_03, 
	push	DEVICE_SIGNAL_LEVEL_04
	jmp	entry_

align 4

[global ia32_irq04_entry_]
ia32_irq04_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_04, 
	push	DEVICE_SIGNAL_LEVEL_03
	jmp	entry_

align 4

[global ia32_irq05_entry_]
ia32_irq05_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_05, 
	push	DEVICE_SIGNAL_LEVEL_02
	jmp	entry_

align 4

[global ia32_irq06_entry_]
ia32_irq06_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_06
	push	DEVICE_SIGNAL_LEVEL_01
	jmp	entry_

align 4

[global ia32_irq07_entry_]
ia32_irq07_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_07
	push	DEVICE_SIGNAL_LEVEL_00
	jmp	entry_

align 4

[global ia32_irq08_entry_]
ia32_irq08_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_08 
	push	DEVICE_SIGNAL_LEVEL_12
	jmp	entry_

align 4

[global ia32_irq09_entry_]
ia32_irq09_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_09
	push	DEVICE_SIGNAL_LEVEL_11
	jmp	entry_

align 4

[global ia32_irq10_entry_]
ia32_irq10_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_10
	push	DEVICE_SIGNAL_LEVEL_10
	jmp	entry_

align 4

[global ia32_irq11_entry_]
ia32_irq11_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_11
	push	DEVICE_SIGNAL_LEVEL_09
	jmp	entry_

align 4

[global ia32_irq12_entry_]
ia32_irq12_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_12
	push	DEVICE_SIGNAL_LEVEL_08
	jmp	entry_

align 4

[global ia32_irq13_entry_]
ia32_irq13_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_13
	push	DEVICE_SIGNAL_LEVEL_07
	jmp	entry_

align 4

[global ia32_irq14_entry_]
ia32_irq14_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_14
	push	DEVICE_SIGNAL_LEVEL_06
	jmp	entry_

align 4

[global ia32_irq15_entry_]
ia32_irq15_entry_:
	push	0x0	; push 0x0 (resevered for error code)
	push	IA32_IRQ_15
	push	DEVICE_SIGNAL_LEVEL_05
	jmp	entry_

align 4

; Used for system calls (executive/user)
[global ia32_syscall00_entry_]
ia32_syscall00_entry_:
	push	0x0	; push fake error code
	push	SYSCALL_VECTOR_00
	push	INVALID_LEVEL
	jmp	entry_

align 4

; Used to get the tick count (executive/user)
[global ia32_syscall01_entry_]
ia32_syscall01_entry_:
	push	ds
	mov	eax, EXEC_DATA_SEL
	mov	ds, eax
	mov	eax, [GlobalClockTickCounter]
	pop	ds
	iret

align 4

; Reserved NoOp
[global ia32_syscall02_entry_]
ia32_syscall02_entry_:
	push	0x0	; push fake error code
	push	SYSCALL_VECTOR_02
	push	INVALID_LEVEL
	jmp	entry_
