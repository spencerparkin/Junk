#
# loop2.S
#
# Question:
# calculate the number of occurrences of "letter"
# in the string "chararray"
#
# Output format must be:
# "number of occurrences = 6"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#-------------- start cut --------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			xor			a0,a0			# zero count
			lb			t0,(letter)		# load test char
			la			t1,chararray	# load address of first byte in array
loop:
			lb			t2,(t1)			# load a char
			beqz		t2,exit_loop	# hit null-terminator?
			add			t1,1			# bump char address
			bne			t0,t2,loop		# hit an occurance of letter?
			add			a0,1			# yes, bump count
			j			loop			# continue processing
exit_loop:
			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#--------------  end cut  --------------------

			.data

chararray:	.asciiz		"abbbaabbbababab"
letter:		.byte		'a'
ans:		.asciiz		"number of occurences = "
endl:		.asciiz		"\n"
