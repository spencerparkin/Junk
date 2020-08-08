#
# loop8.S
#
# Question:
# Replace the last character in the
# string "charstr" with the first character.
#
# Then print the resulting string.
#
# Output format must be:
# "abcdea"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------ start cut -------------------
		
			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t0,charstr		# load address of first char in string
			lb			t5,(t0)			# load first char and keep it for later
			beqz		t5,oops			# branch if string is empty
loop:			
			add			t0,1			# increment char address
			lb			t1,(t0)			# load this char
			bnez		t1,loop			# until we hit the null termintor

			sb			t5,-1(t0)		# overwrite last char w/first char

			la			a1,charstr		# reload address of string
			jal			WR_str			# print out the string

			la			a1,endl
			jal			WR_str
oops:
			lw			ra,(sp)
			add			sp,4

			jr			ra

#------------  end cut  -------------------

			.data

charstr:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
