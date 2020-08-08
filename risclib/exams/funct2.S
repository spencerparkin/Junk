#
# funct2.S
#
# Question:
# Write a function "hexint" that takes the address
# of an ascii character string in $a0.
# The string will representa number in
# hexadecimal and will only conatin '0' to '9'
# and 'A' to 'F'.
# Return the actual number in the register $v0.
# Remember that the most significan nibble
# will be first in the string.
#
# Output format must be:
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			a1,ans
			jal			WR_str

			la			a0,str
			jal			hexint		# get magnitude of hex-number in string

			move		a0,v0		# number to print
			li			a2,10		# format to print it in (decimal)
			jal			WR_num

			la			a1,endl
			jal			WR_endl

			li			v0,10		# return value

			lw			ra,(sp)
			add			sp,4

			jr			ra

#------------ start cut ------------------

			# Spencer`s solution:

#===============================================
# hexint function:
#  input -> a0 -> address of hex-string
#  output -> v0 -> magnitude of hex string
hexint:
			sub			sp,12
			sw			ra,(sp)
			sw			a1,4(sp)
			sw			a2,8(sp)

			move		a1,a0		# we need the address in a1
			li			a2,16		# base of number format in string
			jal			str2num		# See file: utils.S

			lw			ra,(sp)
			lw			a1,4(sp)
			lw			a2,8(sp)
			add			sp,12

			jr			ra			# return from function

#------------  end cut  ------------------

			.data

str:		.asciiz		"7A8"
ans:		.asciiz		"Number is = "
endl:		.asciiz		"\n"
