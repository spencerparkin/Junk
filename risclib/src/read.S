// read.S	Copyright (c) 2002	By Spencer T. Parkin

// This file implements the read functions.
// Most functions will read from stdin.  As
// of this typing, strings and signed integers
// are all that is supported.

#include <asm/regdef.h>
#include <asm/unistd.h>
#include <risclib.h>
#include <data.h>

			.text
#-------------------------------------------------------------------------------
# function:	RD_str		read string from stdin
# input:	a1			address of string
# 			a2			char limit of string at a1
# output:	-->			string at a1 read from stdin
# note:		-->			string at a1 becomes null terminated
# note:		-->			read return value v0 assumed to be correct
#-------------------------------------------------------------------------------
			.globl		RD_str
			.ent		RD_str
RD_str:		.frame		sp,12,ra
			.mask		0x00010034,-4
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
			sub			sp,12
			sw			a0,(sp)
			sw			a1,4(sp)
			sw			v0,8(sp)
			sub			a2,1			# reserve a byte for null
			li			a0,1			# fileno(stdin) = 1
			li			v0,__NR_read	# use system read service
			syscall						# call kernel (read string)
			add			a1,v0			# address 2 past last byte
			xor			t0,t0			# null terminator
			sb			t0,-1(a1)		# null terminate string
			add			a2,1			# restore a2 register
			lw			a0,(sp)
			lw			a1,4(sp)
			lw			v0,8(sp)
			add			sp,12
			jr			ra
			.end		RD_str
#-------------------------------------------------------------------------------
# function: RD_num		read number from stdin
# input:	a0			radix (number base)
# output:	v0			magnitude of number read from stdin in a2-ary format
#-------------------------------------------------------------------------------
			.globl		RD_num
			.ent		RD_num
RD_num:		.frame		sp,16,ra
			.mask		0x80010060,-8
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
#			.cprestore	12
			sub			sp,16
			sw			ra,(sp)
			sw			a1,4(sp)
			sw			a2,8(sp)
			la			a1,scratch		# load scratch-pad string
			li			a2,PADLEN		# load char limit of scratch
			jal			RD_str			# read number from stdin to scratch
			move		a2,a0			# specify number base
			jal			str2num			# convert scratch in a2-ary format
			lw			ra,(sp)
			lw			a1,4(sp)
			lw			a2,8(sp)
			add			sp,16
			jr			ra
			.end		RD_num
#-------------------------------------------------------------------------------
# function:	RD_fnum		read float from stdin
# input:	
# output:				not implemented yet
#-------------------------------------------------------------------------------
			.globl		RD_fnum
			.ent		RD_fnum
RD_fnum:	jr			ra
			.end		RD_fnum
