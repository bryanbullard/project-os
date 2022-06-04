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
;	CDECL BYTE ReadByteFromPort ( IN PORTADDR port )
;
[global ReadByteFromPort]
ReadByteFromPort:
	push	ebp
	mov	ebp, esp
	xor	eax, eax
	push	edx
	mov	edx, [ebp+8]
	in	al, dx
	pop	edx
	pop	ebp
	ret

align 4	
;	CDECL BYTE ReadShortFromPort ( IN PORTADDR port )
;
[global ReadShortFromPort]
ReadShortFromPort:
	push	ebp
	mov	ebp, esp
	xor	eax, eax
	push	edx
	mov	edx, [ebp+8]
	in	ax, dx
	pop	edx
	pop	ebp
	ret

align 4	
;	CDECL BYTE ReadLongFromPort ( IN PORTADDR port )
;
[global ReadLongFromPort]
ReadLongFromPort:
	push	ebp
	mov	ebp, esp
	push	edx
	mov	edx, [ebp+8]
	in	eax, dx
	pop	edx
	pop	ebp
	ret

align 4	
;	CDECL VOID WriteByteToPort ( IN PORTADDR port, IN BYTE val )	
;
[global WriteByteToPort]
WriteByteToPort:
	push	ebp
	mov	ebp, esp
	push	edx
	mov	eax, [ebp+8]
	mov	dx, [ebp+12]
	out	dx, al
	pop	edx
	pop	ebp
	ret

align 4	
;	CDECL VOID WriteShortToPort ( IN PORTADDR port, IN USHORT val )	
;
[global WriteShortToPort]
WriteShortToPort:
	push	ebp
	mov	ebp, esp
	push	edx
	mov	eax, [ebp+8]
	mov	dx, [ebp+12]
	out	dx, ax
	pop	edx
	pop	ebp
	ret

align 4	
;	CDECL VOID WriteLongToPort ( IN PORTADDR port, IN BYTE val )	
;
[global WriteLongToPort]
WriteLongToPort:
	push	ebp
	mov	ebp, esp
	push	edx
	mov	eax, [ebp+8]
	mov	dx, [ebp+12]
	out	dx, eax
	pop	edx
	pop	ebp
	ret
