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

#include <totem/base.h>
#include <totem/bytecopy.h>





VOID* 
CopyBytes(
	OUT BYTE_T* dest, 
	IN BYTE_T* src, 
	IN SIZE_T bytes
	)
{
	BYTE_T* d = dest;
	BYTE_T* s = src;
	
	if (dest <= src) 
	{
		
		while (bytes--)
			*d++ = *s++;
	
	}
	
	else 
	{
		
		d = d + bytes;
		s = s + bytes;
	
		while (bytes--)
			*--d = *--s;
	
	}

	return (VOID*)dest;

}



VOID*
SetBytes(
	OUT BYTE_T* addr, 
	IN BYTE_T val, 
	IN SIZE_T bytes
	)
{

	BYTE_T* d = addr;


	while (bytes--)
		*d++ = val;


	return (VOID*)addr;

}




VOID*
ZeroBytes(
	OUT BYTE_T* addr,
	IN SIZE_T bytes
	)
{

	BYTE_T* d = addr;


	while (bytes--)
		*d++ = 0;


	return (VOID*)addr;
	
}
