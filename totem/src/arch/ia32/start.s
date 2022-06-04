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

[extern main_]
[extern CopyBytes]
[extern ZeroBytes]
[extern bss]
[extern ebss]
[extern initial_stack_top_]
[extern idle_process_entry]
[extern GlobalCurrentProcessPointer]
[extern init_processors_]
[extern init_runqueue_]
[extern init_fault_handlers_]
[extern init_paging_]
[extern init_system_calls_]
[extern TerminateProcess]
[extern CreateProcess]
[extern StallProcessor]
[extern SetDispatchLevel]

[global start]
[global GlobalMultibootHeader]
[global GlobalMultibootInfo]
[global GlobalMultibootMagic]


[section .text]


align 4

MBOOT_FLAG_PAGE_ALIGN	equ 0x00000001
MBOOT_FLAG_MEMORY_INFO	equ 0x00000002
MBOOT_FLAG_VIDEO_INFO	equ 0x00000004

MBOOT_MAGIC		equ 0x1BADB002
MBOOT_FLAGS		equ MBOOT_FLAG_PAGE_ALIGN | MBOOT_FLAG_MEMORY_INFO | MBOOT_FLAG_VIDEO_INFO
MBOOT_CHECKSUM		equ -(MBOOT_MAGIC + MBOOT_FLAGS)
MBOOT_VIDEO_MODE	equ 1	; EGA-standard text mode
MBOOT_VIDEO_WIDTH	equ 80
MBOOT_VIDEO_HEIGHT	equ 60
MBOOT_VIDEO_DEPTH	equ 0

align 4

start:

	; Make sure interrupts are disabled
	; for this initialization.  If the 
	; dispatcher is call during
	; this time the system could become
	; unstable.  THIS INCLUDES FAULTS! 
	cli								

	; Use inital stack
	mov	esp, initial_stack_top_

	; Initialize global multiboot data
	mov	[GlobalMultibootMagic], eax
	push	88	; size of multiboot info struct
	push	ebx	; pointer to the multiboot data
	push	GlobalMultibootInfo
	call	CopyBytes
	sub	esp, 12

	; Zero-out BSS section
	mov	eax, ebss
	sub	eax, bss
	push	eax,
	push	bss
	call	ZeroBytes
	sub	esp, 8

	; Setup context for calling init_ c funtions
	mov	eax, 0
	mov	ecx, 0
	mov	edx, 0
	mov	ebx, 0
	mov	esp, initial_stack_top_
	mov	ebp, 0
	mov	edi, 0
	mov	esi, 0
	cld

	; Initialize the essentials
	call	init_processors_
	call	init_fault_handlers_
	call	init_runqueue_
	call	init_paging_
	;call	init_system_calls_

	; Patch the stack for the idle process
	mov	ebp, [GlobalCurrentProcessPointer]
	mov	esp, [ebp+32]
	
	; We need to lower the dispatcher to NORMAL_LEVEL so the
	; initial process will run.
	push	1	; NORMAL_LEVEL
	call	SetDispatchLevel
	sub	esp, 4
	
	; Enable interrupts and enter main_
	mov	eax, 0
	mov	ecx, 0
	mov	edx, 0
	mov	ebx, 0
	mov	esp, initial_stack_top_
	mov	ebp, 0
	mov	edi, 0
	mov	esi, 0
	cld
	sti
	push	dword [GlobalMultibootInfo+16]  ;cmdline see mltiboot.h
	call	main_
	add	esp, 4
	
	; After the entering main_ the dispatcher starts and we never
	; return here unless main_ returns which would be a bug.
	cli
	hlt

align 4

GlobalMultibootHeader:
	dd MBOOT_MAGIC
	dd MBOOT_FLAGS
	dd MBOOT_CHECKSUM
	dd 0
	dd 0
	dd 0			
	dd 0
	dd 0
	dd MBOOT_VIDEO_MODE
	dd MBOOT_VIDEO_WIDTH
	dd MBOOT_VIDEO_HEIGHT
	dd MBOOT_VIDEO_DEPTH




[section .data]

align 4

GlobalMultibootInfo:
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0			
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0

align 4	

GlobalMultibootMagic:
	dd 0