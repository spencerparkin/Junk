#
# funct3.S
#
# Question:
# Write a function "setbits" that takes a
# number in $a0 and sets all the bits from
# the most significan bit down.
#
# Return the resulting number in the register %v0
# For example 100111 base two becomes 111111.
#
# Output format must be:
# "Number is = 63"
#

#include <asm/regdef.h>

#ifndef _MODULE		// This is done so that we can re-use code in this file
			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			a1,ans
			jal			WR_str

			li			a0,39
			jal			setbits		# call function

			move		a0,v0
			li			a2,10		# print in decimal format
			jal			WR_num		# print number

			jal			WR_endl

			lw			ra,(sp)
			add			sp,4

			jr			ra

#endif _MODULE

#------------- start cut --------------------

			# Spencer`s solution:

			# The question isn`t quite clear.
			# I`m assuming that these examples
			# demonstrate what they want us
			# to do.
			#
			// 00001011 -> 00001111
			// 01000101 -> 01111111
			// 10001110 -> 11111111
			// 00000011 -> 00000011
			// 00011101 -> 00011111
			// 00000000 -> 00000000
			// 00100000 -> 00111111
			# etc...
			#
			# Except we`ll be working with
			# a word, not a byte.

#====================================
# setbits function: set all bits on from last bit set to the LSB
#  input -> a0 -> bitfield
#  output -> v0 -> modified a0 bitfield
			.globl		setbits
setbits:
			li			v0,0x80000000		# mask for MSB
loop:
			and			t0,v0,a0			# mask all but this bit
			bnez		t0,break			# break if this bit set
			srl			v0,1				# search from MSB toward LSB
			bnez		v0,loop				# until out of bits to test
			jr			ra					# no bits left, return 0
break:
			sll			v0,1				# get ready to sub 1
			sub			v0,1				# ex. 001000 - 1 = 000111
			jr			ra

#-------------  end cut  --------------------

#ifndef _MODULE

			.data

ans:		.asciiz		"Number is = "

#endif _MODULE
