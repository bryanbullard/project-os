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



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Exported Functions
;;
;;	IA-32 Specific Macros
;;

align 4
; PUBLIC UINT32 ia32_get_frame_pointer_ ( VOID )
;
[global ia32_get_frame_pointer_]
ia32_get_frame_pointer_:
	mov	eax, ebp
	ret

align 4
; PUBLIC VOID ia32_invlpg_ ( IN PVOID pAddr )
;
[global ia32_invlpg_]
ia32_invlpg_:
	push	ebp
	mov	ebp, esp
	invlpg	[ebp+8]
	pop	ebp
	ret

align 4	
;	PUBLIC VOID ia32_lgdt_ ( IN PVOID pGDTR )
;	
[global ia32_lgdt_]
ia32_lgdt_:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	lgdt	[eax]
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_lidt_ ( IN PVOID pIDTR )
;
[global ia32_lidt_]
ia32_lidt_:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	lidt	[eax]
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_ltr_ ( IN UINT16 TR )
;
[global ia32_ltr_]
ia32_ltr_:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	ltr	ax
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_hlt_ (VOID)	
;
[global ia32_hlt_]
ia32_hlt_:
	push	ebp
	mov	ebp, esp
	hlt
	pop	ebp
	ret

align 4	
;	PUBLIC UINT32 ia32_get_eflags_ (VOID)
;
[global ia32_get_eflags_]
ia32_get_eflags_:
	push	ebp
	mov	ebp, esp
	pushf
	mov	eax, [esp]
	add	esp, 4
	pop	ebp
	ret

align 4
;	PUBLIC UINT32 ia32_get_eflags_and_cli_ (VOID)
;
[global ia32_get_eflags_and_cli_]
ia32_get_eflags_and_cli_:
	push	ebp
	mov	ebp, esp
	pushf
	cli
	mov	eax, [esp]
	add	esp, 4
	pop	ebp
	ret

align 4	
;	PUBLIC VOID ia32_set_eflags_ ( UINT32 EFLAGS )
;   
[global ia32_set_eflags_]
ia32_set_eflags_:
	push	ebp
	mov	ebp, esp
	push	dword [ebp+8]
	popf
	pop	ebp
	ret

align 4
;	PUBLIC UINT32 ia32_get_cr0_ (VOID)
;
[global ia32_get_cr0_]
ia32_get_cr0_:
	push	ebp
	mov	ebp, esp
	mov	eax, cr0
	pop	ebp
	ret

align 4	
;	PUBLIC VOID ia32_set_cr0_ (DWORD CR0)
;
[global ia32_set_cr0_]
ia32_set_cr0_:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	mov	cr0, eax
	pop	ebp
	ret

align 4
;	PUBLIC UINT32 ia32_get_cr2_ (VOID)	
;
[global ia32_get_cr2_]
ia32_get_cr2_:
	push	ebp
	mov	ebp, esp
	mov	eax, cr2
	pop	ebp
	ret

align 4
;	PUBLIC UINT32 ia32_get_cr3_ (VOID)
;
[global ia32_get_cr3_]
ia32_get_cr3_:
	push	ebp
	mov	ebp, esp
	mov	eax, cr3
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_set_cr3_ (UINT32 CR3)
;
[global ia32_set_cr3_]
ia32_set_cr3_:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	mov	cr3, eax
	pop	ebp
	ret

align 4	
;	PUBLIC UINT32 ia32_rdtsc_low_ (VOID)
;	
[global ia32_rdtsc_low_]
ia32_rdtsc_low_:
	push	ebp
	mov	ebp, esp
	cpuid
	rdtsc
	pop	ebp
	ret

align 4	
;	PUBLIC UINT32 ia32_rdtsc_high_ (VOID)
;	
[global ia32_rdtsc_high_]
ia32_rdtsc_high_:
	push	ebp
	mov	ebp, esp
	cpuid
	rdtsc
	mov	eax, edx
	pop	ebp
	ret	

align 4	
;	PUBLIC VOID ia32_cli_ (VOID)
;
[global ia32_cli_]
ia32_cli_:
	push	ebp
	mov	ebp, esp
	cli
	pop	ebp
	ret

align 4
;	PUBLIC VOID _ia32_sti_ (VOID)	
;
[global ia32_sti_]
ia32_sti_:
	push	ebp
	mov	ebp, esp
	sti
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_issue_master_pic_eoi_ (VOID)
;
[global ia32_issue_master_pic_eoi_]
ia32_issue_master_pic_eoi_:
	push	ebp
	mov	ebp, esp
	pushf
	cli
	mov	eax, 0x20
	out	0x20, al
	popf
	pop	ebp
	ret

align 4
;	PUBLIC VOID ia32_issue_slave_pic_eoi_ (VOID)
;
[global ia32_issue_slave_pic_eoi_]
ia32_issue_slave_pic_eoi_:
	push	ebp
	mov	ebp, esp
	pushf
	cli
	mov	eax, 0x20
	out	0xa0, al
	out	0x20, al
	popf
	pop	ebp
	ret