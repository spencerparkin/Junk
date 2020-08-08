#
# math3.S
#
# Question:
# calculate (NUM-3)*(NUM+4)
#
# Output format must be:
# "answer = 98"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#---------------- start cut ---------------------
			
			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			lw			t0,(NUM)	# load NUM
			move		t1,t0		# duplicate NUM
			sub			t0,3		# subtract 3 from 1st copy
			add			t1,4		# add 4 to 2nd copy
			mul			a0,t0,t1	# calculate product of copies

			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#----------------  end cut  ---------------------

			.data

NUM:		.word		10

ans:		.asciiz		"answer = "
endl:		.asciiz		"\n"
