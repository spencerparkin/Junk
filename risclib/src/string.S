// string.S		Copyright (c) 2002	By Spencer T. Parkin

// This file implements common string operations.
// Many other modules implement functions that
// operate on strings too, but the generic ones
// get put here.

#include <asm/regdef.h>
#include <asm/unistd.h>
#include <risclib.h>
#include <data.h>

			.text
#-------------------------------------------------------------------------------
# function: str_len		compute string length
# input:	a1			address of string
# output:	v0			length of string in bytes
# note:		-->			string at a1 is assumed to be null-terminated
#-------------------------------------------------------------------------------
			.globl		str_len
			.ent		str_len
str_len:	move		t0,a1			# copy string address
len_loop:	lb			t1,(t0)			# load a char
			beqz		t1,len_calced	# null terminator?
			add			t0,1			# bump char address
			j			len_loop		# continue
len_calced:	sub			v0,t0,a1		# calculate length in bytes
			jr			ra
			.end		str_len
#-------------------------------------------------------------------------------
# function:	str_cpy		copy string
# input:	a0			address of source string
# 			a1			address of destination string
# output:	-->			string at a1 over-written with string at a0
# note:		-->			it is assumed that the string at a0 will fit
#						inside the string at a1
# note:		-->			it is assumed that the string at a0 is null terminated
#-------------------------------------------------------------------------------
			.globl		str_cpy
			.ent		str_cpy
str_cpy:	move		t0,a0			# copy source string address
			move		t1,a1			# copy destination string address
cpy_loop:	lb			t7,(t0)			# load a char from the source
			sb			t7,(t1)			# store the char to the destination
			add			t0,1			# bump source address
			add			t1,1			# bump destination address
			nop							# extend loop length for r5900
			bnez		t7,cpy_loop		# until we hit the null-terminator
			jr			ra
			.end		str_cpy
#-------------------------------------------------------------------------------
# function:	del_char	delete character in string
# input:	a1			address of char in string
# output:	-->			all chars to the right of a1 in string containing
#						a1 shifted left 1 position.  this overwrites the
#						char at a1 with the char at a1+1.
# note:		-->			it is assumed a1 points to some char in a null
#						terminated string
# note:		-->			if a1 address the null terminator, nothing happens
#-------------------------------------------------------------------------------
			.globl		del_char
			.ent		del_char
del_char:	.frame		sp,4,ra
			.mask		0x0000020,-4
			.set		noreorder
			.cpload		t9
			.set		reorder
			sub			sp,4
			sw			a1,(sp)
			lb			t0,(a1)			# load doomed char
dchar_loop:	beqz		t0,dchar_exit	# until null byte is shifted over
			lb			t0,1(a1)		# load next byte to shift
			sb			t0,(a1)			# overwrite previous byte
			add			a1,1			# bump char address
			j			dchar_loop		# continue
dchar_exit:	lw			a1,(sp)
			add			sp,4
			jr			ra
			.end		del_char
#-------------------------------------------------------------------------------
# function:	new_char	make new character in string
# input:	a1			address of char in string
#			a2			character to fill space
# output:	-->			all chars to the right of a1 in string containing
#						a1 shifted right 1 position.  the space made is
#						filled with the char at a2.
# note:		-->			it is assumed a1 points to some char in a null
#						terminated string
# note:		-->			it is assumed the string about a1 is big enough
#-------------------------------------------------------------------------------
			.globl		mak_char
			.ent		mak_char
mak_char:	move		t9,a1				# remember char address
mcz_loop:	lb			t0,(t9)				# load in a char
			add			t9,1				# bump char address
			nop								# extend loop length for r5900
			bnez		t0,mcz_loop			# null terminator?
mchar_loop:	lb			t0,-1(t9)			# load next byte to shift
			sb			t0,(t9)				# overwrite previous byte
			sub			t9,1				# bump char address
			bgt			t9,a1,mchar_loop	# until we have no chars left
			sb			a2,(t9)				# fill new space
			jr			ra
			.end		mak_char
