/* 
	Copyright (C) 1999, 2001  Free Software Foundation, Inc.
     
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
     
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
     
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
*/

#ifndef _TOTEM_MLTIBOOT_H
#define _TOTEM_MLTIBOOT_H


#include <totem/base.h>

typedef struct multiboot_header MULTIBOOT_HEADER_T;
typedef struct multiboot_info MULTIBOOT_INFO_T;

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/*
 * Features flags for 'flags'.
 * If a boot loader sees a flag in MULTIBOOT_MUSTKNOW set
 * and it doesn't understand it, it must fail.
 */
#define MULTIBOOT_MUSTKNOW		0x0000FFFF

/* currently unsupported flags...  this is a kind of version number.  */
#define MULTIBOOT_UNSUPPORTED		0x0000FFF8

/* Align all boot modules on i386 page (4KB) boundaries.  */
#define MULTIBOOT_PAGE_ALIGN		0x00000001

/* Must pass memory information to OS.  */
#define MULTIBOOT_MEMORY_INFO		0x00000002

/* Must pass video information to OS.  */
#define MULTIBOOT_VIDEO_MODE		0x00000004

/* This flag indicates the use of the address fields in the header.  */
#define MULTIBOOT_AOUT_KLUDGE		0x00010000


#define CHECK_MULTIBOOT_FLAG(flags,bit)	((flags) & (1 << (bit)))

/* Types.  */

/* The Multiboot header.  */
typedef struct multiboot_header
{
  /* Must be MULTIBOOT_MAGIC - see below.  */
  unsigned magic;
  
  /* Feature flags - see below.  */
  unsigned flags;
  
  /*
   * Checksum
   *
   * The above fields plus this one must equal 0 mod 2^32.
   */
  unsigned checksum;
  
  /* These are only valid if MULTIBOOT_AOUT_KLUDGE is set.  */
  unsigned header_addr;
  unsigned load_addr;
  unsigned load_end_addr;
  unsigned bss_end_addr;
  unsigned entry_addr;

  /* These are only valid if MULTIBOOT_VIDEO_MODE is set.  */
  unsigned mode_type;
  unsigned width;
  unsigned height;
  unsigned depth;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  unsigned long tabsize;
  unsigned long strsize;
  unsigned long addr;
  unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
/*
 *  MultiBoot Info description
 *
 *  This is the struct passed to the boot image.  This is done by placing
 *  its address in the EAX register.
 */

typedef struct multiboot_info
{
  /* MultiBoot info version number */
  unsigned long flags;
  
  /* Available memory from BIOS */
  unsigned long mem_lower;
  unsigned long mem_upper;
  
  /* "root" partition */
  unsigned long boot_device;
  
  /* Kernel command line */
  unsigned long cmdline;
  
  /* Boot-Module list */
  unsigned long mods_count;
  unsigned long mods_addr;
  
  union
  {
    struct
    {
      /* (a.out) Kernel symbol table info */
      unsigned long tabsize;
      unsigned long strsize;
      unsigned long addr;
      unsigned long pad;
    }
    a;
    
    struct
    {
      /* (ELF) Kernel section header table */
      unsigned long num;
      unsigned long size;
      unsigned long addr;
      unsigned long shndx;
    }
    e;
  }
  syms;
  
  /* Memory Mapping buffer */
  unsigned long mmap_length;
  unsigned long mmap_addr;
  
  /* Drive Info buffer */
  unsigned long drives_length;
  unsigned long drives_addr;
  
  /* ROM configuration table */
  unsigned long config_table;
  
  /* Boot Loader Name */
  unsigned long boot_loader_name;

  /* APM table */
  unsigned long apm_table;

  /* Video */
  unsigned long vbe_control_info;
  unsigned long vbe_mode_info;
  unsigned short vbe_mode;
  unsigned short vbe_interface_seg;
  unsigned short vbe_interface_off;
  unsigned short vbe_interface_len;
} multiboot_info_t;

/* The module structure.  */
typedef struct multiboot_module /* changed to multiboot_module from module */
{
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
} multiboot_module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct multiboot_memory_map /* changed to multiboot_memory_map from memory_map */
{
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
} multiboot_memory_map_t;


/*
 *  Flags to be set in the 'flags' parameter above
 */

/* is there basic lower/upper memory information? */
#define MB_INFO_MEMORY			0x00000001
/* is there a boot device set? */
#define MB_INFO_BOOTDEV			0x00000002
/* is the command-line defined? */
#define MB_INFO_CMDLINE			0x00000004
/* are there modules to do something with? */
#define MB_INFO_MODS			0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MB_INFO_AOUT_SYMS		0x00000010
/* is there an ELF section header table? */
#define MB_INFO_ELF_SHDR		0x00000020

/* is there a full memory map? */
#define MB_INFO_MEM_MAP			0x00000040

/* Is there drive info?  */
#define MB_INFO_DRIVE_INFO		0x00000080

/* Is there a config table?  */
#define MB_INFO_CONFIG_TABLE		0x00000100

/* Is there a boot loader name?  */
#define MB_INFO_BOOT_LOADER_NAME	0x00000200

/* Is there a APM table?  */
#define MB_INFO_APM_TABLE		0x00000400

/* Is there video information?  */
#define MB_INFO_VIDEO_INFO		0x00000800


extern ULONG_T GlobalMultibootMagic;
extern MULTIBOOT_INFO_T GlobalMultibootInfo;
extern MULTIBOOT_HEADER_T GlobalMultibootHeader;


#endif /* _TOTEM_MLTIBOOT_H */
