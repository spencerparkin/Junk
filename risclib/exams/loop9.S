#
# loop9.S
#
# Question:
# Replace the second to last character in the
# string "charstr" with the last character.
# The string will contain at least 2 characters.
#
# Then print the resulting string.
#
# Output fromat must be:
# "abcdff"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#-------------- start cut -------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t3,charstr		# load address of string
			lb			t0,(t3)			# load first char in string
			beqz		t0,oops			# branch if string is empty
			add			t3,1			# inc. char address by sizeof(char)
			lb			t0,(t3)			# load second char in string
			beqz		t0,oops			# branch if string has only 1 char
loop:
			add			t3,1			# inc. char address by sizeof(char)
			lb			t0,(t3)			# load this char from the string
			bnez		t0,loop			# loop until we load the null terminator

			# can now safely back-index from the t3 pointer
			lb			t0,-1(t3)		# load last char
			sb			t0,-2(t3)		# store it over the 2nd to last char

			la			a1,charstr
			jal			WR_str

			la			a1,endl
			jal			WR_str
oops:
			lw			ra,(sp)
			add			sp,4

			jr			ra

#--------------  end cut  -------------------

			.data

charstr:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
