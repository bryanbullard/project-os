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

OUTPUT_FORMAT(pei-i386)
ENTRY(_start)


SECTIONS
{
	.text 0x100000 :
	{

		code = .; _code = .; __code = .;
		*(.text)
		ecode = .; _ecode = .; __ecode = .;
		
	}

	. = ALIGN(4096);

	.data :
	{

		data = .; _data = .; __data = .;
		*(.data)
		edata = .; _edata = .; __edata = .;
		
	}
    
	. = ALIGN(4096);
    
	.bss :
	{

		bss = .; _bss = .; __bss = .;
		*(.bss)
		*(COMMON)
		ebss = .; _ebss = .; __ebss = .;

	}

	. = ALIGN(4096);

	.pages :
	{
		pages = .; _pages = .;
		*(.pages)
		_initial_stack_base_ = .;
		initial_stack_base_ = .;
		. = (. + 4092);
		_initial_stack_top_ = .;
		initial_stack_top_ = .;
		. = (. + 4);
		_xcore_page_dir_ = .;
		xcore_page_dir_ = .;
		. = (. + 4096);
		_xcore_page_table0_ = .;
		xcore_page_table0_ = .;
		. = (. + 4096);
		_xcore_page_table1_ = .;
		xcore_page_table1_ = .;
		. = (. + 4096);
		_xcore_page_table2_ = .;
		xcore_page_table2_ = .;
		. = (. + 4096);
		_xcore_page_table3_ = .;
		xcore_page_table3_ = .;
		. = (. + 4096);
		_page_zeroing_buffer_ = .;
		page_zeroing_buffer_ = .;
		. = (. + 4096);
		epages = .; _epages = .;

	}

	. = ALIGN(4096);

	end = .; _end = .; __end = .;
}