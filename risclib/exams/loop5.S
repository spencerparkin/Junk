#
# loop5.S
#
# Question:
# Replace every second character in the
# string "charstr" with 'X'.  That is
# the first, third, fifth etc.
# There will always be an even number
# of characters in "charstr".
#
# Then print the resulting string.
#
# Output format must be:
# "XbXdXf"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#--------- start cut ------------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t0,charstr		# load address of 1st char in string
			xor			t1,t1			# zero counter
			li			t2,'X'			# have our 'X' ready to go
loop:
			lb			t3,(t0)			# load a char
			beqz		t3,exit_loop	# hit null-terminator?
			add			t1,1			# increment count
			rem			t4,t1,2			# mod count by 2
			beqz		t4,skip			# skip if count is even
			sb			t2,(t0)			# count is odd, store 'X'
skip:		
			add			t0,1			# bump char address
			j			loop			# continue processing
exit_loop:
			la			a1,charstr
			jal			WR_str

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#---------  end cut  ------------------------

			.data

charstr:	.asciiz		"abcdef"
endl:		.asciiz		"\n"
