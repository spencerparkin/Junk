#
# loopA.S
#
# Question:
# Replace the first character in the
# string "charstr" with the last character.
# The string will contain at least 2 characters.
#
# Then print the resulting string.
#
# Output format must be:
# "fbcdef"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------ start cut ----------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t6,charstr		# load address of first char in string
			lb			t0,(t6)			# load first char
			beqz		t0,oops			# branch if string is empty
loop:
			add			t6,1			# inc. char address by sizeof(char)
			lb			t0,(t6)			# load this char
			bnez		t0,loop			# until we load null terminator

			lb			t0,-1(t6)		# load last char in string
			la			a1,charstr		# load addres of string again
			sb			t0,(a1)			# store last char over first char
			jal			WR_str			# print out the modified string

			la			a1,endl
			jal			WR_str
oops:
			lw			ra,(sp)
			add			sp,4

			jr			ra

#------------  end cut  ----------------

			.data

charstr:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
