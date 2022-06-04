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


#ifndef _TOTEM_BASE_H
#define _TOTEM_BASE_H


#include <totem/ia32/base.h>


/* Generic Types *****************************************/

typedef void VOID;



/* Integral Types ****************************************/


/* unsigned 8-bit precision */
typedef unsigned char BYTE_T; /* ansi characters (8-bit precision) */

/* unsigned 16-bit precision */
typedef unsigned short int USHORT_T;
typedef unsigned short int WCHAR_T;

/* unsigned 32-bit precision */
typedef unsigned int ULONG_T;

/* unsigned 64-bit precision */
typedef unsigned long long int ULONGLONG_T;

/* signed 8-bit precision */
typedef signed char CHAR_T; /* ansi characters (8-bit precision) */

/* signed 16-bit precision */
typedef signed short int SHORT_T;

/* signed 32-bit precision */
typedef signed int LONG_T;

/* signed 64-bit precision */
typedef signed long long int LONGLONG_T;




/* Floating-Point Types **********************************/

/* signed 32-bit precision */
typedef float FLOAT_T;

/* signed 64-bit precision */
typedef double DOUBLE_T;




/* Boolean Type *****************************************/

typedef ULONG_T BOOLEAN_T;

#define FALSE (BOOLEAN_T)0
#define TRUE (BOOLEAN_T)(~FALSE)




/* Function Status Return Type ***************************/

typedef LONG_T STATUS_T;



#ifndef NULL
#define NULL (VOID*)0
#endif






#endif /* _TOTEM_BASE_H */


