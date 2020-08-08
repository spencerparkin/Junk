#
# funct1.S
#
# Question:
# Write a function "addone" that takes an ascii
# character in $a0.
# If the character is a digit, increment it by
# one. '9' should go to '0'.
# If it is not a digit, return it unaltered
# in $v0.
#
# Write a function "stradd" that goes along
# a string $a0 calling "addone" to add one
# to all the digits in the string and stores
# the returned characters into the string.
#
# Output format must be:
# "abc1067xyz"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			li			a0,'5'		# test addone function
			jal			addone
			bne			v0,'6',exit

			la			a0,str
			jal			stradd		# call stradd function
			move		a1,a0
			jal			WR_str
exit:
			li			v0,10		# return value
			
			lw			ra,(sp)
			add			sp,4

			jr			ra

#-------------- start cut --------------------

			# Spencer`s solution:

#==================================================
# addone function:
#  input -> a0 -> ascii char
#  output -> v0 -> ascii char (a0 + 1) mod '9'
addone:
			move		v0,a0				# work with return value
			blt			v0,'0',outbounds	# branch if out of bounds
			bgt			v0,'9',outbounds	# branch if out of bounds
			sub			v0,'0'				# get in range: 0-9
			add			v0,1				# increment
			rem			v0,v0,10			# wrap 10 to 0
			add			v0,'0'				# convert back to ascii
outbounds:
			jr			ra					# return from function

#=================================================
# stradd function:
#  input -> a0 -> address of string
#  output -> string at a0 w/digit ascii chars rotated once
stradd:
			sub			sp,12				# allocate stack frame
			sw			ra,(sp)				# save return address
			sw			s0,4(sp)			# save register s0
			sw			a0,8(sp)			# save register a0

			move		s0,a0				# use s0 to address string chars
loop_stradd:
			lb			a0,(s0)				# load a char from the string
			jal			addone				# add 1 to it if appropriate
			sb			v0,(s0)				# store char back into the string
			add			s0,1				# inc. char address to next byte
			bnez		a0,loop_stradd		# until we hit null-terminator

			lw			ra,(sp)				# restore return address
			lw			s0,4(sp)			# restore register s0
			lw			a0,8(sp)			# restore register a0
			add			sp,12				# deallocate stack frame

			jr			ra					# return from function

#--------------  end cut  --------------------

			.data

str:		.asciiz		"abc0956xyz\n"
