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
#include <totem/status.h>
#include <totem/bytecopy.h>
#include <totem/portio.h>
#include <totem/arch.h>
#include <totem/display.h>




/**
 * These routines handle writing to the console
 * until a proper video device is loaded.
 */

#define VGA_ADDR_PORT 			(IOPORT_ADDRESS_T)0x03d4
#define VGA_DATA_PORT 			(IOPORT_ADDRESS_T)0x03d5
#define CURSOR_LOC_HIGHBYTE_INDEX 	0x0e
#define CURSOR_LOC_LOWBYTE_INDEX 	0x0f
#define SCREEN_START_HIGHBYTE_INDEX	0x0c
#define SCREEN_START_LOWBYTE_INDEX 	0x0d


#define VBE_MODE_25X80			0x2580

/* supported gray-scale */
#define VGA_COLOR_BLACK 		0x0
#define VGA_COLOR_DARK_GRAY		0x8
#define VGA_COLOR_LIGHT_GRAY		0x7
#define VGA_COLOR_WHITE			0xf

/* supported color-scale */
#define VGA_COLOR_BLUE  		0x1
#define VGA_COLOR_GREEN 		0x2
#define VGA_COLOR_RED			0x4
#define VGA_COLOR_YELLOW		0xe


#define DEFAULT_TABSIZE			8


typedef struct
{
	ULONG_T columns; 	/* # of supported columns */
	ULONG_T rows;		/* # of supported rows */
	ULONG_T text;		/* hardware text color */
	ULONG_T fill;		/* hardware fill color */
	ULONG_T last;		/* the last character printed */
	ULONG_T offset;		/* screen offset of where next character will be printed */
	ULONG_T tab;		/* the tab size */
	ULONG_T x;		/* the zero base x coord of where next character will be printed */
	ULONG_T y;		/* the zero base y coord of where next character will be printed */
	BOOLEAN_T cursor; 	/* is the hardware cursor enabled */
	ULONG_T origin;		/* the hardware screen origin offset */
	ULONG_T mode;		/* the vbe display mode (set by system) */
	ULONG_T base;		/* the base address of the hardware screen buffer */
	ULONG_T size;		/* the size of the hardware screen buffer */
} DISPLAYSTATE;


static volatile DISPLAYSTATE ds_;
static volatile BOOLEAN_T ds_init_ = FALSE;



#define DO_TAB_OFFSET(POS,TABSIZE)((POS + TABSIZE) & ( ~(TABSIZE - 1)))
#define CHAR_ATTR(fc, tc) ((UINT32_T)(((fc) << 4) | (0x0f & (tc))))

static
ULONG_T
display_strlen_( 
	IN CHAR_T* szStr 
	)
{
	CHAR_T* len;

	for (len = szStr; *len; ++len);

	return (ULONG_T)(len - szStr);
}

static
ULONG_T
display_convert_unsigned_(
	IN UINT32_T uVal, 
	OUT CHAR_T* szBuffer, 
	IN UINT32_T uRadix, 
	IN UINT32_T uPrec, 
	IN CHAR_T* szDigits
	)
{
	CHAR_T buf[80];
	CHAR_T* s_base = szBuffer;
	CHAR_T* pbuf = buf;
	INT32_T len = 0;
	
	if (
	(szBuffer == NULL) || 
	(uRadix < 0) || 
	(uRadix > 36) || 
	(uPrec < 0) ||
	(uPrec > 40) ||
	(szDigits == NULL)
	) return 0;
	
	if (uRadix == 0) uRadix = 10;
	
	if (uVal == 0)
	{ 
		*pbuf++ = '0';
	} 
	else
	{
		while (uVal)
		{
			*pbuf++ = szDigits[uVal % uRadix];
			
			uVal /= uRadix;
		}
	}
	
	while (pbuf != buf)
	{
		*szBuffer++ = *--pbuf;
		
		len++;
	}
	
	*szBuffer = '\0';
	
	if ((uPrec > 0) && (uPrec != len))
	{	
		szBuffer = s_base;
	
		if (uPrec < len)
		{
			CHAR_T* offset = (CHAR_T*)((ULONG_T)szBuffer + (len - uPrec));
			CopyBytes(szBuffer, offset, (uPrec + 1));
			return uPrec;	
			
		}
		
		if (uPrec > len)
		{
			CHAR_T* offset = (CHAR_T*)((ULONG_T)buf + (uPrec - len));
			
			SetBytes(buf, '0', 80);
			CopyBytes(offset, szBuffer, (len + 1));
			
			CopyBytes(szBuffer, buf, (uPrec + 1));
			
			return uPrec;
		}
	}
	
	return (ULONG_T)len;
}

static
CHAR_T*
display_convert_signed_(
	IN INT32_T iVal, 
	OUT CHAR_T* szBuffer, 
	IN UINT32_T uRadix, 
	IN UINT32_T uPrec, 
	IN CHAR_T* szDigits
	)
{
	if ( iVal < 0 )
	{
		iVal = -iVal;
		
		*szBuffer++ = '-';
	}

	display_convert_unsigned_(
		(UINT32_T)iVal, 
		szBuffer, 
		uRadix, 
		uPrec, 
		szDigits
		);
	
	return &szBuffer[0];
}

static
VOID
vga_set_screen_origin_( 
	IN UINT16_T uOffset 
	)
{
	WriteByteToPort(SCREEN_START_LOWBYTE_INDEX, VGA_ADDR_PORT);
	WriteByteToPort((UINT8_T)(uOffset & 0x00ff), VGA_DATA_PORT);
	WriteByteToPort(SCREEN_START_HIGHBYTE_INDEX, VGA_ADDR_PORT);
	WriteByteToPort((UINT8_T)((uOffset & 0xff00) >> 8), VGA_DATA_PORT);

}

static
VOID
vga_set_cursor_offset_( 
	IN UINT32_T uOffset 
	) 
{
	WriteByteToPort(CURSOR_LOC_LOWBYTE_INDEX, VGA_ADDR_PORT);
	WriteByteToPort((UINT8_T)(uOffset & 0xff), VGA_DATA_PORT);
	WriteByteToPort(CURSOR_LOC_HIGHBYTE_INDEX, VGA_ADDR_PORT);
	WriteByteToPort((UINT8_T)((uOffset >> 8) & 0xff), VGA_DATA_PORT);

}

static
UINT32_T
vga_blt_char_(
	IN UINT32_T uChar, 
	IN UINT32_T uAttributes, 
	IN UINT32_T uBufferAddr, 
	IN UINT32_T uOffset
	)
{
	UINT8_T *vga_mem = (UINT8_T *)uBufferAddr;
	
	vga_mem = (vga_mem + (uOffset * 2));
	*vga_mem++ = (UINT8_T)uChar;
	
	/* note that we are setting the character attribute with
	 * with the current color selection.  when we do this we
	 * are assuming that character blinking is disabled.
	 */
	*vga_mem++ = (UINT8_T)uAttributes;
	
	return uChar;
}

static
VOID
do_scroll_down_(VOID) 
{
	
	/* move all but the top line up one line */
	UINT16_T i = 0;
	
	CopyBytes(
		(BYTE_T*)ds_.base, 
		(BYTE_T*)(ds_.base + (ds_.columns * 2)), 
		(((ds_.columns * ds_.rows) * 2) - (ds_.columns * 2))
		);
	
	/* clear the bottom line */
	for(i = 0; i < (ds_.columns); i++)
		vga_blt_char_(' ', 
		CHAR_ATTR(ds_.fill, ds_.text),
		ds_.base,
		(((ds_.columns * ds_.rows) - ds_.columns) + i)
		);

}

static
ULONG_T
display_string_(
	IN CHAR_T* szString
	)
{
	ULONG_T len;
	ULONG_T i;
	
	if (
	(ds_.x >= ds_.columns) || 
	(ds_.y >= ds_.rows)
	) return 0;
	
	len = display_strlen_(szString);
	
	for(i = 0; i < len; i++)
	{	
		ds_.last = (*szString++);

		switch( ds_.last )
		{
		case '\a':
			/* soundBeep(); */
			/* TODO */
			break;
		case '\b':
			/* TODO */	
			break;
		case '\f':
			/* TODO */
			break;
		case '\n':
			ds_.x = 0;
			if ( ds_.y >= (ds_.rows - 1) )
			{
				do_scroll_down_();
				ds_.y = (ds_.rows - 1);
			}
			else
			{
				++(ds_.y);
			}
			break;

		case '\r':
			ds_.x = 0;
			break;
		case '\t':
			if ( ds_.tab % 2 ) break;
			
			ds_.x = DO_TAB_OFFSET(ds_.x, ds_.tab);
			
			if( ds_.x > (ds_.columns - 1) ) 
			{
				/* do a new line */ 
				ds_.x = 0;
				if ( ds_.y >= (ds_.rows - 1) )
				{
					do_scroll_down_();
					ds_.y = (ds_.rows - 1);
				}
				else
				{
					++(ds_.y);
				}
			}
			break;
		case '\v':
			if( ds_.y ) --(ds_.y);
			break;

		default: /* print a character */
		
			/* update char offset value */	
			ds_.offset = (ds_.y * ds_.columns) + ds_.x;
			
			/* blit the character at the offset */
			vga_blt_char_(
				ds_.last,
				CHAR_ATTR(ds_.fill, ds_.text),
				ds_.base,
				ds_.offset
				);
		
			/* if we're at the end of the line */
			if( ds_.x >= (ds_.columns - 1) )
			{
				/* do a new line */
				ds_.x = 0;
				if(ds_.y >= (ds_.rows - 1))
				{
					do_scroll_down_();
				}
				else
				{
					++(ds_.y);
				}
			}
			else
			{
				++(ds_.x);		
			}

			break;
		}	

		/* update char offset value */	
		ds_.offset = (ds_.y * ds_.columns) + ds_.x;
	
		if( ds_.cursor )
			vga_set_cursor_offset_(ds_.offset);

	}
	
	/* update x y value from offset */
	ds_.y = (ds_.offset / ds_.columns);
	ds_.x = (ds_.offset - (ds_.y * ds_.columns));

	return len;
}

static
VOID
display_clear_()
{
	UINT16_T pos = 0;
	
	for(pos = 0; 
		pos < (ds_.columns * ds_.rows); 
		pos++)
	{	
		vga_blt_char_(
			' ', 
			CHAR_ATTR(ds_.fill, ds_.text),
			ds_.base,
			pos
			); 
	}
	
	ds_.offset = 0;
	ds_.y = 0; 
	ds_.x = 0; 
	
	if ( ds_.cursor )
		vga_set_cursor_offset_(ds_.offset);
		
}

static
VOID
display_init_()
{
	/* we need to get this from multiboot */
	ULONG_T mode = VBE_MODE_25X80; 
	
	switch( mode )
	{
	case VBE_MODE_25X80:
	
		ds_.columns = 80;
		ds_.rows = 25;
		ds_.text = VGA_COLOR_LIGHT_GRAY;
		ds_.fill = VGA_COLOR_BLACK;
		ds_.last = ' ';
		ds_.offset = 0;
		ds_.tab = DEFAULT_TABSIZE;
		
		ds_.x = 0;
		ds_.y = 0;

		/* for right now we disable the cursor by 
		 * moving it off the viewable region */
		if( ds_.cursor == FALSE )
			vga_set_cursor_offset_( 
				(ds_.columns * ds_.rows) 
				);

		ds_.origin = 0;
		vga_set_screen_origin_(ds_.origin);
		
		ds_.mode = VBE_MODE_25X80;
		ds_.base = 0xb8000;
		ds_.size = 0x1000;
	
		break;
		
	default:
		break;
	}
	
	ds_init_ = TRUE;
}


/* Functions Exported to Executive * * * * * * * * * * * * *
 *
 */

ULONG_T
DisplayString(
	IN ULONG_T level,
	IN CHAR_T* format, 
	IN ...
	)
{
	CHAR_T** arg = (CHAR_T**) &format;
	CHAR_T c;
	
	ULONG_T len = 0;
	
	CHAR_T buf[64];
	CHAR_T* str = NULL;

	if( ds_init_ == FALSE )
	{
		display_init_();
		display_clear_();
	}

	arg++;
  
	while ( (c = *format++) != 0 )
	{
		if (c != '%')
		{
			buf[0] = c;
			buf[1] = '\0';
			len = (len + display_string_( buf ));
		}
		else
		{	
	  		c = *format++;
		
			switch (c)
	    		{
	    		case '%':
	    			str = "%";
	    			len = (len + display_string_( str ));
	    			break;
	    			
	    		case 'b':
	      		case 'B':
	    			display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 2, 0, "01");
	      			len = (len + display_string_( buf ));
	      			break;
	    		
	    		case 'c':
	    		case 'C':
	    			buf[0] = (CHAR_T)*((INT32_T *) arg++);
	    			buf[1] = '\0';
	    			len = (len + display_string_( buf ));
	    			break;
	    		
	    		case 'd':
	    		case 'D':
	    		case 'i':
	    		case 'I':
	    			display_convert_signed_(*((INT32_T *) arg++), buf, 10, 0, "0123456789");
	      			len = (len + display_string_( buf ));
	      			break;
	      		
	      		case 'n':
	    		case 'N':
	    			display_convert_unsigned_(len, buf, 10, 0, "0123456789");
				len = (len + display_string_( buf ));
	      			break;
	    		
	    		case 'o':
			case 'O':
	      			display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 8, 0, "01234567");
	      			len = (len + display_string_( buf ));
	      			break;
	    		
	    		case 'u':
			case 'U':
	    			display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 10, 0, "0123456789");
	      			len = (len + display_string_( buf ));
	      			break;
	    		
	    		case 'x':
	    			display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 16, 0, "0123456789abcdef");
	    			len = (len + display_string_( buf ));
	      			break;
	      			
	    		case 'X':
	    			display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 16, 0, "0123456789ABCDEF");
	    			len = (len + display_string_( buf ));
	      			break;
	      			
	    		case 'p':
			case 'P':
			case 'a':
			case 'A':
				display_convert_unsigned_((UINT32_T)*((INT32_T *) arg++), buf, 16, 8, "0123456789ABCDEF");
	      			len = (len + display_string_( buf ));
	      			break;

	    		case 's':
			case 'S':
	      			str = *arg++;
	      			if (str == NULL) str = "(null)";
				len = (len + display_string_( str ));
	      			break;

	    		default:
				buf[0] = '%';
				buf[1] = c;
				buf[2] = '\0';
				len = (len + display_string_( buf ));
				
	      			break;
	    		}
		}
	}
	
	return len;
}



