// write.S		Copyright (c) 2002	By Spencer T. Parkin

// This file implements the write functions.
// Most of these write to stdout.  As of this
// typeing, strings and signed integers are
// all that is supported.

#include <asm/regdef.h>
#include <asm/unistd.h>
#include <risclib.h>
#include <data.h>

			.text
#-------------------------------------------------------------------------------
# function:	WR_str		write string to stdout
# input:	a1			address of string
# output:	-->			string at a1 printed to stdout
# note:		-->			string at a1 is assumed to be null-terminated
# note:		-->			write return value v0 ignored
#-------------------------------------------------------------------------------
			.globl		WR_str
			.ent		WR_str
WR_str:		.frame		sp,20,ra
			.mask		0x80000054,-4
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
#			.cprestore	16
			sub			sp,20
			sw			ra,(sp)
			sw			a0,4(sp)
			sw			a2,8(sp)
			sw			v0,12(sp)
			xor			a0,a0				# fileno(stdout) = 0
			jal			str_len				# v0 = sting length of a1
			move		a2,v0				# number of chars to write
			li			v0,__NR_write		# use write system service
			syscall							# call kernel (print string)
			lw			ra,(sp)
			lw			a0,4(sp)
			lw			a2,8(sp)
			lw			v0,12(sp)
			add			sp,20
			jr			ra
			.end		WR_str
#-------------------------------------------------------------------------------
# function: WR_num		write number to stdout
# input:	a0			number
#			a2			radix (number base)
# output:	-->			number in a0 written to stdout in a2 base format 
#-------------------------------------------------------------------------------
			.globl		WR_num
			.ent		WR_num
WR_num:		.frame		sp,12,ra
			.mask		0x80000020,-8
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
#			.cprestore	8
			sub			sp,12
			sw			ra,(sp)
			sw			a1,4(sp)
			la			a1,scratch		# load address of scratch-pad string
			jal			num2str			# convert number to string
			jal			WR_str			# write the string to stdout
			lw			ra,(sp)
			lw			a1,4(sp)
			add			sp,12
			jr			ra
			.end		WR_num
#-------------------------------------------------------------------------------
# function: WR_fnum		write float to stdout
# input:
# output:				not implemented yet
#-------------------------------------------------------------------------------
			.globl		WR_fnum
			.ent		WR_fnum
WR_fnum:	jr			ra
			.end		WR_fnum
#-------------------------------------------------------------------------------
# function:	WR_endl		write newline char
# input:	-->			zero input
# output:	-->			newline char written to stdout
# note:		-->			write return value v0 is assumed to be 1
#-------------------------------------------------------------------------------
			.globl		WR_endl
			.ent		WR_endl
WR_endl:	.frame		sp,16,ra
			.mask		0x00000074,-4
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
			sub			sp,16
			sw			a0,(sp)
			sw			a1,4(sp)
			sw			a2,8(sp)
			sw			v0,12(sp)
			xor			a0,a0			# fileno(stdout) = 0
			la			a1,scratch		# load scratch-pad string
			li			t0,'\n'			# load newline char
			sb			t0,(scratch)	# store newline char in scratch
			li			a2,1			# write 1 char
			li			v0,__NR_write	# use system write service
			syscall						# call kernel (write newline)
			lw			a0,(sp)
			lw			a1,4(sp)
			lw			a2,8(sp)
			lw			v0,12(sp)
			add			sp,16
			jr			ra
			.end		WR_endl
