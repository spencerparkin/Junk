// convert.S	Copyright (c) 2002	By Spencer T. Parkin

// This file implements the convertion routines.
// These functions convert data to and from
// different formats and/or representations.

#include <asm/regdef.h>
#include <asm/unistd.h>
#include <risclib.h>
#include <data.h>

			.text
#-------------------------------------------------------------------------------
# function: str2num		convert string to number
# input:	a1			address of string
#			a2			radix (number base)
# output:	v0			v0 holds magnitude of the number represented
#						in base a2 in the string at a1
# note:		-->			it is assumed that the string at a0 holds only
#						the chars 0-9, -, a-z, and A-Z.  the results of
#						uncountering unknown chars is undefined, but
#						shouldn`t crash the program.
# note:		-->			it is assumed the string at a0 is null terminated
# note:		-->			radix is assumed to be less than 37 and greater than 0
#-------------------------------------------------------------------------------
	// bug
			.globl		str2num
			.ent		str2num
str2num:	.frame		sp,28,ra
			.mask		0x80e10010,-8
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
#			.cprestore	24
			sub			sp,28
			sw			ra,(sp)
			sw			a0,4(sp)
			sw			s5,8(sp)
			sw			s6,12(sp)
			sw			s7,16(sp)
			sw			s0,20(sp)
			li			s0,1				# assume sign positive
			jal			str_len				# find 1+LSD position
			add			s5,a1,v0			# so we can go LSD to MSD
			xor			s7,s7				# zero total magnitude
			li			s6,1				# load a2-ary digit shifter
s2n_loop:	sub			s5,1				# bump char address
			lb			a0,(s5)				# load a digit-char or sign-char
			bne			a0,'-',s2n_mag		# branch if neg. not encountered
			mul			s0,-1				# toggle sign
			bgt			s5,a1,s2n_loop		# while we have chars left
			j			s2n_exit			# skip over other half of loop
s2n_mag:	jal			ascii2num			# calculate its magnitude
			mul			v0,s6				# multiply by a power of the base
			add			s7,v0				# keep a running total
			mul			s6,a2				# adjust a2-ary digit shifter
			bgt			s5,a1,s2n_loop		# while we have chars left
s2n_exit:	mul			v0,s7,s0			# register return value
			lw			ra,(sp)
			lw			a0,4(sp)
			lw			s5,8(sp)
			lw			s6,12(sp)
			lw			s7,16(sp)
			lw			s0,20(sp)
			add			sp,28
			jr			ra
			.end		str2num
#-------------------------------------------------------------------------------
# function: ascii2num	convert ascii char to number
# input:	a0			ascii char
# output:	v0			magnitude of ascii char as a number
# note:		-->			a0 is assumed to be one of 0-9, a-z, or A-Z
# note:		-->			v0 is 0 if a0 is unknown
#-------------------------------------------------------------------------------
			.globl		ascii2num
			.ent		ascii2num
ascii2num:	xor			v0,v0				# assume zero magnitude
			blt			a0,'0',a2n_exit		# out of digit range?
			bgt			a0,'9',cap_alpha	# out of digit range?
			sub			v0,a0,48			# convert ascii digit to value
			jr			ra					# return w/result in v0
cap_alpha:	blt			a0,'A',a2n_exit		# out of cap-alpha range?
			bgt			a0,'Z',low_alpha	# out of cap-alpha range?
			sub			v0,a0,55			# convert ascii cap-alpha to value
			jr			ra					# return w/result in v0
low_alpha:	blt			a0,'a',a2n_exit		# out of low-alpha range?
			bgt			a0,'z',a2n_exit		# out of low-alpha range?
			sub			v0,a0,87			# convert ascii low-alpha to value
a2n_exit:	jr			ra					# return w/result in v0
			.end		ascii2num
#-------------------------------------------------------------------------------
# function:	num2str		number to string
# input:	a0			number
#			a1			address of string
#			a2			radix (number base)
# output:	-->			string at a1 is formatted in base a2 of the magnitude a0
# note:		-->			string at a1 is assumed to be long enough
# note:		-->			radix is assumed to be less than 37 and greater than 0
# note:		-->			string at a1 becomes null terminated
#-------------------------------------------------------------------------------
	// bug
			.globl		num2str
			.ent		num2str
num2str:	.frame		sp,8,ra
			.mask		0x00000030,-4
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
			sub			sp,8
			sw			a0,(sp)
			sw			a1,4(sp)
			bgez		a0,n2s_cvt		# branch if not negative
			mul			a0,-1			# make a0 positive
			li			t0,'-'			# load negative char
			sb			t0,(a1)			# store sign in string
			add			a1,1			# bump char address
n2s_cvt:	move		fp,sp			# remember stack bottom address
n2s_push:	rem			t0,a0,a2		# compute LSD
			lb			t1,digit(t0)	# load LSD char
			sub			sp,1			# allocate room for a push
			sb			t1,(sp)			# push LSD char on stack
			div			a0,a2			# shift a0 by 1 a2-ary digit
			bnez		a0,n2s_push		# while we have digits left
n2s_pop:	lb			t0,(sp)			# pop MSD char off stack
			add			sp,1			# deallocate room for push
			sb			t0,(a1)			# store MSD char in string
			add			a1,1			# bump char address
			blt			sp,fp,n2s_pop	# while stack not empty
			sb			a0,(a1)			# null terminate string
			lw			a0,(sp)
			lw			a1,4(sp)
			add			sp,8
			jr			ra
			.end		num2str
#-------------------------------------------------------------------------------
# function:	str2upper	convert all lower-case chars to upper-case
# input:	a1			address of string
# output:	-->			all lower-case chars in the string at a1
#						converted to upper-case
# note:		-->			the string at a1 is assumed to be null-terminated
#-------------------------------------------------------------------------------
			.globl		str2upper
			.ent		str2upper
str2upper:	move		t1,a1				# copy string address
s2u_loop:	lb			t0,(t1)				# load a char
			add			t1,1				# bump char address
			beqz		t0,s2u_exit			# null terminator?
			blt			t0,'a',s2u_loop		# before alphabet?
			bgt			t0,'z',s2u_loop		# after alphabet?
			sub			t0,'q'-'Q'			# convert lower to upper case
			sb			t0,-1(t1)			# store converted char
			j			s2u_loop			# continue
s2u_exit:	jr			ra
			.end		str2upper
#-------------------------------------------------------------------------------
# function:	str2lower	convert all upper-case chars to lower-case
# input:	a1			address of string
# output:	-->			all upper-case chars in the string at a1
#						converted to lower-case
# note:		-->			the string at a1 is assumed to be null-terminated
#-------------------------------------------------------------------------------
			.globl		str2lower
			.ent		str2lower
str2lower:	move		t1,a1				# copy string address
s2l_loop:	lb			t0,(t1)				# load a char
			add			t1,1				# bump char address
			beqz		t0,s2l_exit			# null terminator?
			blt			t0,'A',s2l_loop		# before alphabet?
			bgt			t0,'Z',s2l_loop		# after alphabet?
			add			t0,'q'-'Q'			# convert upper to lower case
			sb			t0,-1(t1)			# store converted char
			j			s2l_loop			# continue
s2l_exit:	jr			ra
			.end		str2lower
