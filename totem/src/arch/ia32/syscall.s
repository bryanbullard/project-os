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

%define SYSCALL_VECTOR_00	48	; scheduler
%define SYSCALL_VECTOR_01	49	; get tick count
%define SYSCALL_VECTOR_02	50	; general system calls

%define SYSCALL_NR_NO_OP	0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	Exported Functions
;;

align 4
;	TOTEMCALL VOID invoke_scheduler_()
;
[global invoke_scheduler_]
invoke_scheduler_:
	push	ebp
	mov	ebp, esp
	int	SYSCALL_VECTOR_00
	pop	ebp
	ret

align 4	
;	TOTEMCALL ULONG_T SystemQueryCounter()
;
[global SystemQueryCounter]
SystemQueryCounter:
	push	ebp
	mov	ebp, esp
	int	SYSCALL_VECTOR_01
	pop	ebp
	ret

align 4
;	TOTEMCALL STATUS_T SystemNoOp()
;
[global SystemNoOp]
SystemNoOp:
	push	ebp
	mov	ebp, esp
	mov	eax, SYSCALL_NR_NO_OP
	int	SYSCALL_VECTOR_02
	pop	ebp
	ret



