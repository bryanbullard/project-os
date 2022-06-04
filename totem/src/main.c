/**
 *    Copyright (c) 2003, 2006 Bryan Bullard
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

#include <totem/assert.h>
#include <totem/atomic.h>
#include <totem/arch.h>
#include <totem/build.h>
#include <totem/dispatch.h>
#include <totem/display.h>
#include <totem/memory.h>
#include <totem/mltiboot.h>
#include <totem/syscall.h>
#include <totem/process.h>

LONG_T
main2_( VOID* args )
{
	DISPLAY(DDEBUG, "HELLO FROM PROCESS 1 !!!\n" );
	
	StallProcessor();
	
	return 0;
}

VOID 
main_(
	VOID* args	
	)
{	
	ULONG_T id = 0;

	DISPLAY(
		DINFO,
#if TOTEM_BUILD_DEBUG
		"%s\nVersion %u.%u.%u debug (built: %s %s)\n",
#else
		"%s\nVersion %u.%u.%u (built: %s %s)\n",
#endif 
		TOTEM_COPYRIGHT,
		TOTEM_MAJOR_VERSION,
		TOTEM_MINOR_VERSION,
		TOTEM_BUILD_VERSION,
		TOTEM_BUILD_DATE,
		TOTEM_BUILD_TIME
		);

	if (GlobalMultibootMagic != MULTIBOOT_BOOTLOADER_MAGIC)
		DISPLAY(
			DNOTICE,
			"WARNING: Attempting to load without "
			"a Mulitboot compliant bootloader.\n"
			);

	CreateProcess( 
		PROCESS_STATE_RUNNABLE,
		PROCESS_PRIORITY_LOWEST,
		0,
		main2_, 
		NULL, 
		StallProcessor, 
		&id
		);

	/* DISPLAY(DDEBUG, "COUNTER: %d\n", SystemQueryCounter());
	DISPLAY(DDEBUG, "SystemNoOp status:%d\n", SystemNoOp());		
	*/
	
	DISPLAY(DDEBUG, "HELLO FROM IDLE !!!\n" );
	
	StallProcessor();
}

