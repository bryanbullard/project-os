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

#ifndef _TOTEM_STATUS_H
#define _TOTEM_STATUS_H


#include <totem/base.h>


/* System status codes */

#define STATUS_SUCCESS		0	/* operation was successful */

#define STATUS_BAD_PARAM	512	/* param pasted to function was invalid */
#define STATUS_NOT_AVAIL	513	/* resource is not available */
#define STATUS_UNKNOWN		514	/* error cannot be determined */
#define STATUS_VIOLATION	515	/* system or hardware inforced protection would be violated */
#define STATUS_NO_ACCESS	516	/* process has insufficient credentials to perform operation */
#define STATUS_ASSERT		517	/* assertion failure */


#define IS_SUCCESS(s) ((s) == STATUS_SUCCESS)

#endif /* _TOTEM_STATUS_H */
