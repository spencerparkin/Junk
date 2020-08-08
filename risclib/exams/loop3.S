#
# loop3.S
#
# Question:
# Replace all occurrences of 'a' with
# 'A' in the string "chararray" and
# print the resulting string.
#
# Output format must be:
# "AbbbAAbbbAbAbAb"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#----------- start cut ---------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			la			t0,chararray	# load address of 1st char in array
loop:		
			lb			t1,(t0)			# load a char
			add			t0,1			# bump char address
			beqz		t1,exit_loop	# hit null-terminator?
			bne			t1,'a',loop		# hit an 'a'?
			sub			t1,'j'-'J'		# convert to upper case (ascii)
			sb			t1,-1(t0)		# overwrite char
			j			loop			# continue processing
exit_loop:
			la			a1,chararray
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#-----------  end cut  ---------------------

			.data

chararray:	.asciiz		"abbbaabbbababab\n"
