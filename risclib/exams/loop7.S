#
# loop7.S
#
# Question:
# Replace the first and last character in the
# string "charstr" with 'X'.
#
# Then print the resulting string.
#
# Output format must be:
# "XbcdeX"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#----------- start cut -----------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			li			t9,'X'			# have our 'X' on stand-by
			la			t0,charstr		# load address of first char in string
			lb			t1,(t0)			# load the first char
			beqz		t1,oops			# branch if string is empty
			sb			t9,(t0)			# store 'X' in first char
loop:
			add			t0,1			# increment char address
			lb			t1,(t0)			# load this char
			bnez		t1,loop			# loop until we hit null terminator

			# iff the string only has 1 char in it, it wont
			# hert to store 'X' twice in the same location.
			sb			t9,-1(t0)		# store 'X' in last char

			la			a1,charstr		# load string address again
			jal			WR_str			# print out the string

			la			a1,endl
			jal			WR_str
oops:
			lw			ra,(sp)
			add			sp,4

			jr			ra

#-----------  end cut  -----------------------

			.data

charstr:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
