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

align 4
;	FASTCALL LONG AtomicIncrement( OUT LONG volatile* augend )
;
[global @AtomicIncrement@4]
@AtomicIncrement@4:
	lock 
	inc	dword[ecx]
	mov	eax, [ecx]
	ret

align 4		
;	FASTCALL LONG AtomicDecrement( OUT LONG volatile* augend )
;
[global @AtomicDecrement@4]
@AtomicDecrement@4:
	lock
	dec	dword[ecx]
	mov 	eax, [ecx]
	ret

align 4
;	FASTCALL LONG AtomicAdd( OUT LONG volatile* augend, IN LONG v )
;
[global @AtomicAdd@8]
@AtomicAdd@8:
	lock
	add	[ecx], edx
	mov	eax, [ecx]
	ret

align 4
;	FASTCALL LONG AtomicSubtract( OUT LONG volatile* augend, IN LONG v )
;
[global @AtomicSubtract@8]
@AtomicSubtract@8:
	lock 
	sub	[ecx], edx
	mov	eax, [ecx]
	ret

align 4	
;	FASTCALL LONG AtomicOr( OUT LONG volatile* augend, IN LONG v )
;
[global @AtomicOr@8]
@AtomicOr@8:
	lock 
	or	[ecx], edx
	mov	eax, [ecx]
	ret
	
align 4	
;	FASTCALL LONG AtomicAnd( OUT LONG volatile* augend, IN LONG v )
;
[global @AtomicAnd@8]
@AtomicAnd@8:
	lock 
	and	[ecx], edx
	mov	eax, [ecx]
	ret
	
align 4	
;	FASTCALL LONG AtomicXOr( OUT LONG volatile* augend, IN LONG v )
;
[global @AtomicXOr@8]
@AtomicXOr@8:
	lock 
	xor	[ecx], edx
	mov	eax, [ecx]
	ret

align 4	
;	FASTCALL LONG AtomicExchange( OUT LONG volatile* target, OUT LONG v )
;
[global @AtomicExchange@8]
@AtomicExchange@8:
	lock
	xchg	[ecx], edx
	mov	eax, edx
	ret

align 4	
;	FASTCALL LONG AtomicRead( OUT LONG volatile* v)
;
[global @AtomicRead@4]
@AtomicRead@4:
	lock
	mov	eax, [ecx]
	ret

