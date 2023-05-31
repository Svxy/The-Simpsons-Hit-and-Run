/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *                      Emotion Engine Library
 *                          Version 1.70
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libkernel - app.cmd
 *                        kernel libraly
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.11            May.10.1999     horikawa    
 *      1.10            Oct.12.1999     horikawa   add .stack section
 *      1.20            Nov.09.1999     horikawa   add _heap_end
 *      1.50            Feb.29.2000     horikawa   remove .indata section
 *      1.60            May.08.2000     horikawa   modify start address
 *      1.70            Jun.08.2000     horikawa   modify .text section
 */

_stack_size = 0x00010000;
_stack = 0x01ff0000;  /* set to 0x01ff0000 to simulate 32MB */
_heap_size = -1;
_VU_DMA_OFFSET_size = 0x00000010; /*DO NOT CHANGE!*/

/* GROUP(-lc -lkernl -lgcc) Uncomment this to go back to gcc */
GROUP()
ENTRY(ENTRYPOINT)
SECTIONS {
	.text		0x00100000: { _codestart = .;
		crt0.o(.text)
		*(.text)
		QUAD(0)
		_codeend = .;
	}
	_datastart = .;
	.reginfo		  : { KEEP(*(.reginfo)) }
	.data		ALIGN(128): { *(.data) }
	.dmacache	ALIGN(128)(NOLOAD):{ . += _VU_DMA_OFFSET_size; }
	.rodata		ALIGN(128): { *(.rodata) }
	.rdata		ALIGN(128): { *(.rdata) }
	.gcc_except_table ALIGN(128): { *(.gcc_except_table) }
	_gp = ALIGN(128) + 0x7ff0;
	.lit8       	ALIGN(128): { *(.lit8) }
	.lit4       	ALIGN(128): { *(.lit4) }
	.sdata		ALIGN(128): { *(.sdata) }
	.sbss		ALIGN(128): { _fbss = .; *(.sbss) *(.scommon) }
	.bss		ALIGN(128): { *(.bss) }
	_dataend = .;
	/*
        .stack          ALIGN(128)(NOLOAD): {
		 _stack = .;
                 . += _stack_size;
        }
	*/
	end = .;
	_end = .;
	/* _heap_end = . + _heap_size */
	.spad		0x70000000: {
		 crt0.o(.spad)
		 *(.spad)
	}
}
