
/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *                      Emotion Engine Library
 *                          Version 1.10
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       libkernel - crt0.s
 *                        kernel libraly
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.10           Oct.12.1999     horikawa    renewal
 *       1.50           May.16.2000     horikawa
 */

#ifdef __mips16
	.set nomips16	/* This file contains 32 bit assembly code. */
#endif

#define	ARG_SIZ     256 + 16*4 + 1*4

	.set noat
    	.set noreorder
	.global ENTRYPOINT
	.global _start
	.ent	_start
	.text				# 0x00200000
	nop
	nop
ENTRYPOINT:
_start:
/*
 * clear .bss
 */
zerobss:
	lui	$2, %hi(_fbss)
	lui	$3, %hi(_end)
	addiu	$2, $2, %lo(_fbss)
	addiu	$3, $3, %lo(_end)
1:
	sq	$0, ($2)
	nop
	sltu	$1, $2, $3
	nop
	nop
	bne	$1, $0, 1b
	addiu	$2, $2, 16

/*
 * initialize main thread
 */
	lui	$4, %hi(_gp)
	lui	$5, %hi(_stack)
	lui	$6, %hi(_stack_size)
	lui	$7, %hi(_args)
	lui	$8, %hi(_root)
	addiu	$4, $4, %lo(_gp)
	addiu	$5, $5, %lo(_stack)
	addiu	$6, $6, %lo(_stack_size)
	addiu	$7, $7, %lo(_args)
	addiu	$8, $8, %lo(_root)
	move	$28, $4
	addiu	$3, $0, 60
	syscall
	move	$29, $2

/*
 * initialize heap area
 */
	lui	$4, %hi(_end)
	lui	$5, %hi(_heap_size)
	addiu	$4, $4, %lo(_end)
	addiu	$5, $5, %lo(_heap_size)
	addiu	$3, $0, 61
	syscall

/*
 * initialize System
 */
	jal	_InitSys
	nop

/*
 * flush data cache
 */
	jal	FlushCache
	move	$4, $0

/*
 * call main program
 */
	ei
	lui	$2, %hi(_args)
	addiu	$2, $2, %lo(_args)
	lw	$4, ($2)
	jal	main
	addiu	$5, $2, 4

	j	Exit
	move	$4, $2
	.end	_start

/**************************************/
	.align	3
	.global	_exit
	.ent	_exit
_exit:
	j	Exit			# Exit(0);
	move	$4, $0
	.end	_exit
    
	.align	3
	.ent	_root
_root:
	addiu	$3, $0, 35		# ExitThread();
	syscall
	.end	_root

	.bss
	.align	6
_args: .space	ARG_SIZ

