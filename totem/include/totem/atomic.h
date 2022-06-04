/**
 *    Copyright (c) 2003, 2006 Bryan Bullard.  All Rights Reserved.
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

#ifndef _TOTEM_ATOMIC_H
#define _TOTEM_ATOMIC_H


#include <totem/base.h>


/*
 * Atomic arithmetic, logic operations, and memory tests
 */
FASTCALL LONG_T AtomicIncrement( volatile LONG_T* augend );
FASTCALL LONG_T AtomicDecrement( volatile LONG_T* augend );
FASTCALL LONG_T AtomicAdd( volatile LONG_T* augend, LONG_T v );
FASTCALL LONG_T AtomicSubtract( volatile LONG_T* augend, LONG_T v );
FASTCALL LONG_T AtomicOr( volatile LONG_T* augend, IN LONG_T v );
FASTCALL LONG_T AtomicAnd( volatile LONG_T* augend, IN LONG_T v );
FASTCALL LONG_T AtomicXOr( volatile LONG_T* augend, IN LONG_T v );
FASTCALL LONG_T AtomicExchange( volatile LONG_T* target, LONG_T v );
FASTCALL LONG_T AtomicRead( volatile LONG_T* v);


#endif /* _TOTEM_ATOMIC_H */
