#
# loop4.S
#
# Question:
# Swap each pair of elements in
# the string "chararray" and
# print the resulting string.
# There will always be an even number
# of character in "chararray".
#
# Output format must be:
# "badcfe"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------ start cut -------------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t0,chararray	# load address of first char in array
loop:
			lb			t1,(t0)			# load the 1st char
			beqz		t1,exit_loop	# break on null terminator
			lb			t2,1(t0)		# load the 2nd char
			beqz		t2,exit_loop	# incase string has odd number of chars
			sb			t1,1(t0)		# store the 1st in the 2nd`s position
			sb			t2,(t0)			# store the 2nd in the 1st`s position
			add			t0,2			# go by 2`s
			j			loop			# continue processing string
exit_loop:
			la			a1,chararray
			jal			WR_str

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#------------  end cut  -------------------------

			.data

chararray:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
