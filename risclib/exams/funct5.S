#
# funct5.S
#
# Question:
# Write a function "check3" that takes a
# number in $a0 and returns that number if
# three consecutive bits are set.  Otherwize
# it returns zero.
#
# Output format must be:
# "Number is = 39"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			a1,ans
			jal			WR_str

			li			a0,39
			jal			check3		# call function

			move		a0,v0
			li			a2,10
			jal			WR_num

			jal			WR_endl

			lw			ra,(sp)
			add			sp,4

			jr			ra

#---------------- start cut --------------------

			# Spencer`s solution:

			.extern		findpat		# See funct4.S

#=====================================
# check3 function: check for pattern 111 in the given bitfield
#   input -> a0 -> bitfield
#   output -> v0 -> a0 if 111 found, else 0
check3:
			sub			sp,8
			sw			ra,(sp)
			sw			a1,4(sp)

			li			a1,7		# find 111 pattern
			jal			findpat		# a0 already setup
			beqz		v0,nope		# branch if pattern not found
			move		v0,a0		# return a0 in v0
nope:
			lw			ra,(sp)
			lw			a1,4(sp)
			add			sp,8

			jr			ra

#----------------  end cut  --------------------

			.data

ans:		.asciiz		"Number is = "
endl:		.asciiz		"\n"
