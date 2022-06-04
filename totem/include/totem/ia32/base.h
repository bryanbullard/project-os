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


#ifndef _TOTEM_IA32_BASE_H
#define _TOTEM_IA32_BASE_H

#ifdef __GNUC__

#define DECLARATOR_ALIGN(x) __attribute__ ((aligned (x)))
#define DECLARATOR_PACKED __attribute__ ((packed))

/* Function calling convention */
#define CDECL __cdecl
#define FASTCALL __fastcall
#define TOTEMCALL CDECL
#define CALLBACK CDECL

/* Function parameter attributes */
/* IN - A parameter that is allocated and set by caller 
 *		and not modified by the called function. If the argument
 *		is a reference, the called function doesn't modify
 *		what the reference points to.
 * OUT - A parameter that is usually a reference to a buffer
 *		(allocated by the caller) that could be modified by the 
 *		called function.
 * RESERVED - A parameter that is ignored by the called
 *		function and doesn't have a docmented use but may 
 *		in the future.  Should be set to zero on call.
 * OPTIONAL - A parameter is not required for all operations.  
 *		However, the stack frame must be preserved.  A caller 
 *		that wishes not to use the parameter must send an 
 *		argument of 0 or NULL or called function may return 
 *		invalid parameter status or signal an abort.  
 */
#define IN
#define OUT
#define RESERVED
#define OPTIONAL

#else

#error NON_GNU_COMPILERS_ARE_NOT_SUPPORTED

#endif

/* Use for pointer arithmetic */
typedef int PTRDIFF_T;
typedef unsigned int SIZE_T;


/* Memory tags */
typedef unsigned int TAG_T;

#define MAKE_TAG(char1,char2,char3,char4) \
(TAG_T)((char1<<24)|(char2<<16)|(char3<<8)|(char4))

#define EMPTY_TAG (TAG_T)0

#endif /* _TOTEM_IA32_BASE_H */


